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


class meshMan {
public:
    meshMan();
    void updateFromKinect(ofxKinect* kinect);
    void updateFromNetwork();
    
    void draw();
    
    ofParameter<float> near_threshold, far_threshold;
    ofParameter<float> depth_threshold;
    ofParameter<int> mesh_resolution;
    
private:
    ofMesh mesh;
    ofColor color;
    ofVec3f center;    
};




#endif /* defined(__readytocloud__meshMan__) */
