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
#include "ofxKinect.h"

class meshMan {
public:
    meshMan();
    
    void setupNetwork(int local_port, string remote_ip, int remote_port); // TODO: set parameters
    void setupKinect(ofxKinect* kinect);
    
    void update();
    
    void draw();
    
    ofParameter<float> near_threshold, far_threshold;
    ofParameter<float> depth_threshold_max;
    ofParameter<float> depth_threshold_min;
    ofParameter<int> mesh_resolution;
    
    bool isConnected(){return connected;}
    
    
private:
    ofMesh mesh;
    ofColor color;
    ofVec3f center;
    ofxTCPServer tcp_server;
    ofxTCPClient tcp_client;
    
    ofxKinect* kinect;
    
    bool connected;
    
    enum modes{ mode_kinect, mode_network} mode;

    void updateFromKinect();
    void updateFromNetwork();
    
    void sendMeshTCP();
    void receiveTCP();
    void addMeshVertex(float x, float y, float z);
    void refreshRemoteMesh();

};

#endif /* defined(__readytocloud__meshMan__) */
