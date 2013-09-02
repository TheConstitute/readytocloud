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
#include "meshFlash.h"

class meshMan {
public:
    meshMan();
    
    void setup(meshTransceiver* transceiver, ofxKinect* kinect = NULL);
    void update();
    void draw();
    void drawBeamInOut(float fader);
    void drawDebug();
    void beamIn();
    void beamOut();
    
    ofParameter<float> near_threshold, far_threshold;
    ofParameter<float> depth_threshold_max;
    ofParameter<int> mesh_resolution_x;
    ofParameter<int> mesh_resolution_y;
    
    ofParameter<bool> draw_contour;
    ofParameter<bool> mirror;
    ofParameter<int> mesh_mode;
    
    ofParameter<int> cv_near_threshold, cv_far_threshold;
    
    ofParameter<ofVec3f> offset;
    
    bool isConnected(){return connected;}
    
    ofMesh* getMesh(){return &mesh;}
    
    bool isFrameNew(){return frame_new;}
    
private:
    
    void updateFromKinect();
    void updateFromNetwork();
    void addMeshVertex(float x, float y, float z);
    void refreshRemoteMesh();
    void tryCreateFlash(const ofVec3f &start, const ofVec3f &end);
    void beamFlash(const ofVec3f &vertex);
    
    ofMesh mesh, temp_mesh;
    ofColor color;
    
    meshTransceiver* transceiver;
    ofxKinect* kinect;
    
    void drawContour();
    
    bool connected;
    bool frame_new;
    
    enum modes{ mode_kinect, mode_network} mode;
    enum mesh_modes {mesh_mode_triangles, mesh_mode_quads, mesh_mode_lines, mesh_mode_points, mesh_mode_cross_lines};
    enum beam_states {beaming_in, beamed_in, beaming_out, beamed_out} beam_state;
    
    //opencv stuff
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    ofxCvContourFinder contourFinder;
    

    
    float fader = 0;
    bool hide = false;
    const int max_flashes = 200;
    const float flash_amplitude = 300.0f;
    const float flash_line_width = 2.0f;
    const int beam_flash_skip_vertices = 10;
    const float beam_flash_vertex_range = 10.0f;
    vector<meshFlash *> flash_list;
};

#endif /* defined(__readytocloud__meshMan__) */
