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
#include "meshTransceiver.h"

class meshMan {
public:
    meshMan();
    
    void setup(meshTransceiver* transceiver, ofxKinect* kinect = NULL);
    
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
    
    meshTransceiver* transceiver;
    ofxKinect* kinect;
    
    bool connected;
    
    enum modes{ mode_kinect, mode_network} mode;

    void updateFromKinect();
    void updateFromNetwork();
    
    void addMeshVertex(float x, float y, float z);
    void refreshRemoteMesh();

};

#endif /* defined(__readytocloud__meshMan__) */
