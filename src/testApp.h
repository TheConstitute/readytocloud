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
#include "meshInteractor.h";

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
        bool draw_gui;

    
        // VIDEO OVERLAYS
        ofVideoPlayer overlay_in_local;
        ofVideoPlayer overlay_out_local;
        ofVideoPlayer overlay_in_remote;
        ofVideoPlayer overlay_out_remote;
    
        void beamIn_local();
        void beamOut_local();
        void beamIn_remote();
        void beamOut_remote();

        bool b_overlay_in_local = false;
        bool b_overlay_out_local = false;
        bool b_overlay_in_remote = false;
        bool b_overlay_out_remote = false;
        bool b_clouds = false;

        ofCamera camera;
    ofEasyCam easyCam;
    
        ofxOscReceiver oscReceiver;
        ofxOscSender oscSender;
    
    /* NETWORK PARAMETERS */
    ofParameterGroup network_parameters;
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
    
    ofParameterGroup remote_mesh_parameters;
    ofParameter<float> remote_mesh_scale;
    
    ofParameter<bool> mirror_scene; // TODO: implent mirroring
    
    ofParameter<float> x_correction_local = 0;
    ofParameter<float> x_correction_remote = 0;
    ofParameter<float> y_correction_local = 0;
    ofParameter<float> y_correction_remote = 0;
    
    ofParameter<float> kinect_angle;
    
    
    bool use_easy_cam;
    bool draw_grid;
    
    /* LIGHT PARAMETERS */
    ofParameterGroup light_parameters;
    
    
    /* DMX */
   	ofxDmx dmx;

    /* lights */
    StairvilleLEDParSpot spotInteraction1, spotInteraction2, spotCloud1, spotCloud2;
    LEDRing ledRingInteraction;
    FogMachine fogMachine;
    
    int dmx_state = 0;
    
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
    bool activate_network = false;
    
    ofFbo fboLocal;
    ofFbo fboRemote;
    
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
    
    // TEST
    ofParameter<float> fov;
    ofParameter<float> camera_offset_y;
    ofParameter<float> line_width;
    
    
};
