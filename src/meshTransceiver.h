//
//  meshTransceiver.h
//  readytocloud
//
//  Created by The Constitute on 23.08.13.
//
//

#ifndef readytocloud_meshTransceiver_h
#define readytocloud_meshTransceiver_h

#define MESH_ENCODER_DEBUG

#include "ofMain.h"
#include "ofxNetwork.h"

class meshTransceiver
{

public:
    
    meshTransceiver(void);
    
    void setup(int local_server_port, string server_ip, int remote_server_port);
    
    bool send(ofMesh *mesh);
    bool receive(ofMesh *mesh);
    
    void disconnect();
    
    int getNumBytesReceived();
    int getNumBytesSent();
    
    bool isConnected(){ return connected;}
    
private:
    
    void connectToRemoteHost();
    void calcCoordTransform();
    
    void encode();

//    void encValueShort(int val);
    void encReducedInt(int val);
    void encVector(ofVec3f &vec);
    
    
    void decode(int size);
    
    int decReducedInt();
    
    int decVector(ofVec3f &vec);
    
    
    inline void writeBuffer(unsigned char byte)
    {
        if (buffer_index < buffer.size())
            buffer[buffer_index] = byte;
        else
            buffer.push_back(byte);
        
        buffer_index++;
    }
    
    inline unsigned char readBuffer()
    {
        unsigned char val = buffer[buffer_index];
        buffer_index++;
        return val;
    }
    
//    int bit_index;

    const char* data_start_id = "mesh";
    
    ofxTCPServer tcp_server;
    ofxTCPClient tcp_client;
    
    bool connected;
    string server_ip;
    int remote_server_port;
    float last_connection_check;
    
    ofMesh *mesh = NULL;
    
    ofVec3f coord_scale;
    ofVec3f coord_offset;

#ifdef MESH_ENCODER_DEBUG
    bool value_was_clamped;
#endif
    
    int buffer_index;
    
    vector<unsigned char> buffer;
    
    int num_bytes_sent, num_bytes_received;
    
};

#endif
