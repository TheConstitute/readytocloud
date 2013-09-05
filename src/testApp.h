#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxDmx.h"
#include "ofxKinect.h"
#include "StairvilleLEDParSpot.h"
#include "LEDRing.h"
#include "FogMachine.h"
#include "ofxGui.h"
#include "meshMan.h"
#include "meshInteractor.h"
#include "ofxOscParameterSync.h"

class testApp : public ofBaseApp{

	public:
    testApp(){}

		void setup();
		void update();
		void draw();
        void exit();
        void keyPressed(int key);
            
        void polylineToPath(ofPath* path, ofPolyline* polyline, int smoothing);
        void addMeshVertex(float x, float y, float z, int user);
        void refreshRemoteMesh(int user);
    
        // event listeners
        void stateChanged(int &state);
    
        // INITIALIZATION & UPDATER
        void oscUpdate();
        void oscUpdateAll();
        void dmxUpdate();
        void updateScene();
    
        void nextState();
    
        // KINECT STUFF
        ofxKinect kinect;
    
        // GUI 
        ofxPanel gui;
        ofxPanel gui3d;
        bool draw_gui, draw_gui3d;
    
        // VIDEO OVERLAYS
        ofVideoPlayer overlay_in_local;
        ofVideoPlayer overlay_out_local;
        ofVideoPlayer overlay_in_remote;
        ofVideoPlayer overlay_out_remote;

        ofCamera camera;
        ofEasyCam easyCam;
    
        ofxOscReceiver ipadReceiver;
        ofxOscSender ipadSender;
        ofxOscReceiver state_receiver;
        ofxOscSender state_sender;

    
    
    /* NETWORK PARAMETERS */
    ofParameterGroup network_parameters;
    ofParameter<bool> activate_network;
    ofParameter<string> server_ip;
    ofParameter<int> local_port, remote_port;
    ofParameter<string> remote_ip;
    
    ofParameterGroup osc_parameters;
    ofParameter<int> osc_port_send, osc_port_receive;
    ofParameter<string> osc_ipad_ip;
    
    ofParameterGroup dmx_parameters;
    ofParameter<string> dmx_entec_port;
    

    /* RENDERING PARAMETERS */
    ofParameterGroup local_mesh_parameters;
    ofParameter<float> local_mesh_scale;
    ofParameter<float> x_correction_local = 0;
    ofParameter<float> y_correction_local = 0;
    ofParameter<float> kinect_angle;
    ofParameter<float> fov;
    ofParameter<float> camera_offset_y;
    ofParameter<float> camera_angle;
    ofParameter<float> line_width;
    ofParameter<bool> hide_remote;
    
    ofParameterGroup interaction_parameters;
    
    /* LIGHT PARAMETERS */
    ofParameterGroup light_parameters;
    
    /* DMX */
   	ofxDmx dmx;
    ofParameter<int> state_sync_port_local;
    ofParameter<int> state_sync_port_remote;
    ofParameter<int> dmx_state = 0;
    ofParameter<int> dmx_state_remote;

    /* lights */
    StairvilleLEDParSpot spotInteraction1, spotInteraction2, spotCloud1, spotCloud2;
    LEDRing ledRingInteraction;
    FogMachine fogMachine;
    
    // DEBUG DRAW SETTINGS
    bool use_easy_cam;
    bool draw_grid;
    bool draw_setup_cams;
    
    bool lightStateChanged = true;
    bool settingsChanged = true;
    bool firstFrame_local = false;
    bool firstFrame_remote = false;
    
    
    bool local_autocenter = true;
    bool remote_autocenter = false;
    
    ofVec3f mesh_local_center;
    ofVec3f mesh_remote_center;
    
    ofColor colorCharacter_local = ofColor(255, 255, 255);
    ofColor colorCharacter_remote = ofColor(255, 255, 255);
    
    bool draw_local_mesh = true;
    bool draw_remote_mesh = true;
    
    bool mirror = true;

    
    ofFbo fboLocal;
    
    int alpha_local = 255;
    int alpha_remote = 255;
    
    bool fadeAlpha_local = true;
    bool fadeAlpha_remote = true;
    
    int numTrackedUsers_remote;
    
    float fadeStartTime_local = 0;
    float fadeStartTime_remote = 0;
    
    bool draw_debug = false;
    
    meshMan local_mesh;
    meshMan remote_mesh;
    
    meshTransceiver mesh_transceiver;
    meshInteractor mesh_interactor;

    ofColor colors[7] = { ofColor(255, 255, 255), ofColor(255,0,0), ofColor(0,255,0), ofColor(0,133,255), ofColor(255,255,50), ofColor(255,0,130), ofColor(180,0, 255) };
    string color_names[7] = {"white", "red", "green", "blue", "yellow", "pink", "violet"};
    
};
