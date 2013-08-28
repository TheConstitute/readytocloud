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
#include "ofxOpenCv.h"
#include "meshTransceiver.h"

class meshMan {
public:
    meshMan();
    
    void setup(meshTransceiver* transceiver, ofxKinect* kinect = NULL);
    
    void update();
    
    void draw();
    void drawDebug();
    
    ofParameter<float> near_threshold, far_threshold;
    ofParameter<float> depth_threshold_max;
    ofParameter<int> mesh_resolution_x;
    ofParameter<int> mesh_resolution_y;
    
    ofParameter<bool> draw_contour;
    ofParameter<int> mesh_mode;
    
    ofParameter<int> cv_near_threshold, cv_far_threshold;
    
    bool isConnected(){return connected;}
    
    ofMesh* getMesh(){return &mesh;}
    
    
private:
    ofMesh mesh, temp_mesh;
    ofColor color;
    ofVec3f center;
    
    meshTransceiver* transceiver;
    ofxKinect* kinect;
    
    void drawContour();
    
    bool connected;
    
    enum modes{ mode_kinect, mode_network} mode;
    enum mesh_modes {mesh_mode_triangles, mesh_mode_quads, mesh_mode_lines, mesh_mode_points, mesh_mode_cross_lines};

    void updateFromKinect();
    void updateFromNetwork();
    
    void addMeshVertex(float x, float y, float z);
    void refreshRemoteMesh();
    
    //opencv stuff
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    ofxCvContourFinder contourFinder;

};

#endif /* defined(__readytocloud__meshMan__) */
