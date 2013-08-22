//
//  meshMan.h
//  readytocloud
//
//  Created by Julian Adenauer on 21.08.13.
//
//

#ifndef __readytocloud__meshMan__
#define __readytocloud__meshMan__

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxNetwork.h"


class meshMan {
public:
    meshMan();
    
    void setupNetwork(int local_server_port, string server_ip, int remote_server_port); // TODO: set parameters
    
    void updateFromKinect(ofxKinect* kinect);
    void updateFromNetwork();
    
    void draw();
    
    ofParameter<float> near_threshold, far_threshold;
    ofParameter<float> depth_threshold;
    ofParameter<int> mesh_resolution;
    
    inline bool isConnected(){return connected;}
    
    
private:
    ofMesh mesh;
    ofColor color;
    ofVec3f center;
    ofxTCPServer tcpServer;
    ofxTCPClient tcpClient;
    
    bool connected;
    
    void sendMeshTCP();
    void receiveTCP();
    void addMeshVertex(float x, float y, float z);
    void refreshRemoteMesh();

};

#endif /* defined(__readytocloud__meshMan__) */
