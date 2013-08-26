//
//  meshTransceiver.cpp
//  readytocloud
//
//  Created by The Constitute on 23.08.13.
//
//

#include "meshTransceiver.h"

#define meshTransceiver_Z_OFFSET 3000f

meshTransceiver::meshTransceiver(void)
{
    connected = false;
    
    calcCoordTransform();
    
    last_connection_check = -1000.0; // make sure that the connection is checked on startup
}

void meshTransceiver::setup(int local_server_port, string server_ip, int remote_server_port){
    this->server_ip = server_ip;
    this->remote_server_port = remote_server_port;
    
    // setup the local server
    if(tcp_server.setup(local_server_port))
        ofLog() << "server is set up on port " << ofToString(local_server_port);
    else "server setup failed";
    tcp_server.setMessageDelimiter("\n");
    
    connectToRemoteHost();
}


void meshTransceiver::connectToRemoteHost(){
    if(ofGetElapsedTimef() - last_connection_check > 5.0){
        ofLog() << "trying to establish a connection to the remote server: " << ofToString(server_ip) << ofToString(remote_server_port);
        connected = tcp_client.setup(server_ip, remote_server_port);
        tcp_client.setMessageDelimiter("\n");
        
        if(connected){
            ofLog() << "client is connected to server " << tcp_client.getIP() << ":" << tcp_client.getPort();
        }
        
        last_connection_check = ofGetElapsedTimef();
    }
}

	
bool meshTransceiver::send(ofMesh *mesh)
{
    this->mesh = mesh;
    
    if (connected) {
        encode();
        
        unsigned int countBytes = buffer_index;
        bool send_succeeded = tcp_client.sendRawBytes(data_start_id, strlen(data_start_id));

        send_succeeded = send_succeeded && tcp_client.sendRawBytes((const char*)&countBytes, sizeof(unsigned int));
        send_succeeded = send_succeeded && tcp_client.sendRawBytes((const char*)buffer.data(), countBytes);
        
        if(send_succeeded){
            ofLog() << "successfully sent " << countBytes << " bytes";
            return true;
        }
        else if (!tcp_client.isConnected())
            connected = false;
    }
    else{
        connectToRemoteHost();
    }
    
    return false;

}


/*
 if (m_hSocket == INVALID_SOCKET) return(SOCKET_ERROR);
 
 if (m_dwTimeoutSend	!= NO_TIMEOUT)
 {
 fd_set fd;
 FD_ZERO(&fd);
 FD_SET(m_hSocket, &fd);
 timeval	tv=	{m_dwTimeoutSend, 0};
 if(select(m_hSocket+1,&fd,NULL,NULL,&tv)== 0)
 {
 return(SOCKET_TIMEOUT);
 }
 */

bool meshTransceiver::receive(ofMesh *mesh)
{
    this->mesh = mesh;
    
    int ret;
    
    for(int client = 0; client < tcp_server.getLastID(); client++) {
		if( !tcp_server.isClientConnected(client) ) continue;
        
        unsigned int countBytes;
        
        // search for start-id of data
        char strbuf[256];
        int n = strlen(data_start_id);
        
        ret = tcp_server.receiveRawBytes(client, strbuf, n);
        if (ret != n) return false;
        
        int test = 0;
        
        while (strncmp(strbuf, data_start_id, n) != 0)
        {
            memcpy(strbuf, &(strbuf[1]), n-1);
            
            ret = tcp_server.receiveRawBytes(client, &(strbuf[n-1]), 1);
            if (ret != 1) return false;
            
            test++;
        }
        //ofLog() << "found signature after " << test << " bytes (receive)	";
 
        
        // read number of bytes
        ret = tcp_server.receiveRawBytes(client , (char *)&countBytes, sizeof(unsigned int)); // (may be incompatible with other OS!)
        if (ret != sizeof(unsigned int)) return false;
        
        // make sure that we don't get a malloc error in case the number is far too high
        if(countBytes < 100000){
            if (buffer.size() < countBytes) buffer.resize(countBytes);
        }
        else{
            ofLog() << "meshTransceiver: " << countBytes << " exceeds limit of 100.000 bytes for receiving";
            return false;
        }
        
        // read data
        int i = 0;
        do {
            ret = tcp_server.receiveRawBytes(client , (char *)(buffer.data() + i), countBytes - i);
            if (ret == SOCKET_TIMEOUT || ret == SOCKET_ERROR)
            {
                if (ret == SOCKET_TIMEOUT) ofLog() << "meshTransceiver: Socket timeout while receiving mesh";
                else ofLog() << "meshTransceiver: Socket error while receiving mesh";
                return false;
            }
            else i += ret;
        } while (i < countBytes);
        
        ofLog() << "received " << countBytes << " bytes";
        
        decode(countBytes);

        return true;
    }

}

/* 
 
 protocol (so far):
 ------------------
 
 1 unsigned int : number of bytes of encoded mesh-data
 
 1 reduced int : ofPrimitiveMode
 
 1 reduced int : number of vertices
 
 vertices consisting of 3 reduced int values

*/

// Set or determine scale and offset for coordinate transformation
void meshTransceiver::calcCoordTransform()
{
    coord_scale.x = ((float)(1 << 15)) / 3000.0f;     // range +/- 3m
    coord_scale.y = ((float)(1 << 15)) / 2000.0f;     // range +/- 2m
    coord_scale.z = ((float)(1 << 15)) / 3000.0f;     // range +/- 3m
    
    coord_offset.z = -3000.0f;     // kinect returns only positive z values but we want zero-centered values
}


//---------------- encode ----------------


void meshTransceiver::encode()
{
#ifdef MESH_ENCODER_DEBUG
    value_was_clamped = false;
#endif
 
    buffer_index = 0;
    
//    bit_index = 0;
    
    buffer.clear();
    encReducedInt(mesh->getMode());
   
    encReducedInt(mesh->getNumVertices());
    
    for(int i = 0; i < mesh->getNumVertices(); i++)
    {
        ofVec3f vertex = mesh->getVertex(i);
        encVector(vertex);
    }
    
#ifdef MESH_ENCODER_DEBUG
    if (value_was_clamped)
    {
        ofLog() << "meshTransceiver: value was clamped during encoding, change coordinate scaling/offset";
    }
#endif
    
    
}

/*
void encFloat(float val)
{
    // this way to serialize a float might lead to problems when trying to communicate between machines
    // with different float implementation - especially different byte order (little-/big endian?)
    
    for (int i=0; i<sizeof(float);i++)
        buffer.push_back( ((unsigned char *)&val)[i] );
}
*/

//void meshTransceiver::encValueShort(int val) {}


void meshTransceiver::encReducedInt(int val)
{
    val += (1 <<15);    // make unsigned (2^16 bit)
#ifdef MESH_ENCODER_DEBUG
    // check range and clamp
    if (val >= (1 << 16))
    {
        val = (1 << 15)-1;
        value_was_clamped = true;
    }
    else if (val < 0)
    {
        val = 0;
        value_was_clamped = true;
    }
#endif
    
    writeBuffer( (unsigned char) (val & 0xff) ); // low byte
    writeBuffer( (unsigned char) ((val >> 8) & 0xff) ); // high byte
}


void meshTransceiver::encVector(ofVec3f &vec)
{
    int comp;
    
    ofVec3f ov = vec + coord_offset;
    
    comp = (int)(ov.x * coord_scale.x);
    encReducedInt(comp);

    comp = (int)(ov.y * coord_scale.y);
    encReducedInt(comp);

    comp = (int)(ov.z * coord_scale.z);
    encReducedInt(comp);
}



//---------------- decode ----------------

void meshTransceiver::decode(int size)
{
    //ofLog() << "decode(" << size << ") called";
    
    if (size == 0)
        return;

    mesh->clear();

    
    buffer_index = 0;
    
    mesh->setMode((ofPrimitiveMode)decReducedInt());

    int numVerts = decReducedInt();
    
    //ofLog() << "decode: numVerts = " << numVerts;
    
    ofVec3f vertex;
    
    while ((numVerts > 0) && (buffer_index < buffer.size()))
    {
        decVector(vertex);
        mesh->addVertex(vertex);
        numVerts--;
    }
    
    if (numVerts > 0)
        ofLog() << "meshTransceiver: buffer was smaller than indicated by number of vertices during decode()!";

}


int meshTransceiver::decReducedInt()
{
    int val = readBuffer() + (readBuffer() << 8);
    val -= (1 <<15);    // make signed (2^16 bit)
    return val;
}

int meshTransceiver::decVector(ofVec3f &vec)
{
    int comp;
    
    comp = decReducedInt();
    vec.x = (((float)comp) / coord_scale.x) - coord_offset.x;
    
    comp = decReducedInt();
    vec.y = (((float)comp) / coord_scale.y) - coord_offset.y;
    
    comp = decReducedInt();
    vec.z = (((float)comp) / coord_scale.z) - coord_offset.z;
}

