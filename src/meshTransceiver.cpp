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
}


void meshTransceiver::setup(int local_server_port, string server_ip, int remote_server_port){
    this->server_ip = server_ip;
    this->remote_server_port = remote_server_port;
    
    if(tcp_server.setup(local_server_port))
        ofLog() << "server is set up on port " << ofToString(local_server_port);
    else "server setup failed";
    
    tcp_server.setMessageDelimiter("\n");
    
    connectToRemoteHost();
    
}

void meshTransceiver::connectToRemoteHost(){
    ofLog() << "trying to establish a connection to the remote server: " << ofToString(server_ip) << ofToString(remote_server_port);
    connected = tcp_client.setup(server_ip, remote_server_port);
    tcp_client.setMessageDelimiter("\n");
    
    if(connected)
        ofLog() << "client is connected to server " << tcp_client.getIP() << ":" << tcp_client.getPort();
    
    last_connection_check = ofGetElapsedTimef();
}

bool meshTransceiver::send(ofMesh *mesh)
{
    this->mesh = mesh;
    
    if (connected && tcp_client.isConnected()) {
        encode();
        
        unsigned int countBytes = buffer_index;
        ofLog() << "sent " << countBytes << " bytes";
        
        try
        {
            tcp_client.sendRawBytes((const char*)&countBytes, sizeof(unsigned int));
            tcp_client.sendRawBytes((const char*)buffer.data(), countBytes);
            return true;
        }
        catch (int e) { ofLog() << "caught an tcp-exception in meshTransceiver::send()" << e; }
    }
    return false;
}

bool meshTransceiver::receive(ofMesh *mesh)
{
    mesh->clear();
    
    ofLog() << "receive() called";
    if(!connected && ofGetElapsedTimef() - last_connection_check > 5.0)
        connectToRemoteHost();
    
    if (tcp_server.getNumClients() == 0) {
        ofLog() << "tcp_server.getNumClients() == 0";
        return false;
    }
    
    unsigned int countBytes;
    
    try {
        ofLog() << tcp_server.receiveRawBytes(0 , (char *)&countBytes, sizeof(unsigned int)); // (may be incompatible with other OS!)
        if (buffer.size() < countBytes) buffer.resize(countBytes);
        ofLog() << "received " << tcp_server.receiveRawBytes(0 , (char *)buffer.data(), countBytes);
        decode(countBytes);
        ofLog() << "should have received " << countBytes << " bytes";
        return true;
    }
    catch (int e) { ofLog() << "caught an tcp-exception in meshTransceiver::receive() " << e; }
    return false;
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
    if (size == 0)
        return;
    
    buffer_index = 0;
    
    //mesh->setMode((ofPrimitiveMode)decReducedInt()); //TODO: wieder rein nehmen
    decReducedInt();
    
    
    int numVerts = decReducedInt();
    
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

