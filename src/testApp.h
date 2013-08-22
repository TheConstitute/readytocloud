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
    
        void drawWireframeRemote();
        void nextState();
    
        // KINECT STUFF
        ofxKinect kinect;
    
        // GUI PARAMETERS
    	ofxPanel gui;
    
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
    
        ofxOscReceiver oscReceiver;
        ofxOscSender oscSender;
    
        ofVec3f mesh_local_center;
        ofVec3f mesh_remote_center;
    
    /* NETWORK SETTINGS */
    int PORT_OSC_CONTROL_SEND = 7001;
    int PORT_OSC_CONTROL_RECEIVE = 7002;
    string IPAD_IP = "192.168.0.101";
    string SERVER_IP = "localhost";
    int LOCAL_SERVER_PORT = 7000;
    int REMOTE_SERVER_PORT = 8000;
    bool isServer = true;
    
    float lastConnectionCheck = 0;

    bool connected;
    
    /* DMX */
   	ofxDmx dmx;
    string ENTTEC_PORT = "tty.usbserial-EN118363";

    /* lights */
    StairvilleLEDParSpot spotInteraction1, spotInteraction2, spotCloud1, spotCloud2;
    LEDRing ledRingInteraction;
    FogMachine fogMachine;
    
    int dmx_state = 0;
    
    bool lightStateChanged = true;
    bool settingsChanged = true;
    bool firstFrame_local = false;
    bool firstFrame_remote = false;
    
    /* SETTINGS */
    bool local_autocenter = true;
    bool remote_autocenter = false;
    ofParameter<float> mesh_scale_local = 1500;
    float mesh_scale_remote = 1500;
    float xCorrection_local = 0;
    float xCorrection_remote = 0;
    float yCorrection_local = 0;
    float yCorrection_remote = 0;
    
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
    
    bool showDebug = false;
    
    meshMan local_mesh;
    meshMan remote_mesh;
};
