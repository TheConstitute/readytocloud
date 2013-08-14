#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxOsc.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxNetwork.h"
#include "ofxDmx.h"
#include "StairvilleLEDParSpot.h"
#include "LEDRing.h"
#include "FogMachine.h"


#define ADD_MESH_VERTEX_USER0 0
#define ADD_MESH_VERTEX_USER1 1
#define ADD_MESH_VERTEX_USER2 2
#define ADD_MESH_VERTEX_USER3 3
#define ADD_MESH_VERTEX_USER4 4
#define CLEAR_MESH_USER0 5
#define CLEAR_MESH_USER1 6
#define CLEAR_MESH_USER2 7
#define CLEAR_MESH_USER3 8
#define CLEAR_MESH_USER4 9

#define ADD_GHOST_VERTEX_USER0 10
#define ADD_GHOST_VERTEX_USER1 11
#define ADD_GHOST_VERTEX_USER2 12
#define ADD_GHOST_VERTEX_USER3 13
#define ADD_GHOST_VERTEX_USER4 14
#define CLEAR_GHOST_USER0 15
#define CLEAR_GHOST_USER1 16
#define CLEAR_GHOST_USER2 17
#define CLEAR_GHOST_USER3 18
#define CLEAR_GHOST_USER4 19


#define CLEAR_GHOSTS 19

#define NUM_MAX_USERS 5

#define RENDER_MODE_WIREFRAME 0
#define RENDER_MODE_GHOST 1
#define RENDER_MODE_SKELETON 2

class testApp : public ofBaseApp{

	public:
    testApp(){}

		void setup();
		void update();
		void draw();
        void exit();
        void keyPressed(int key);
    
        void drawPointCloud();
        
        void getTriangleMesh(ofMesh* userMesh, ofMesh* newMesh, int depthThreshold);
        void polylineToPath(ofPath* path, ofPolyline* polyline, int smoothing);
        void addMeshVertex(float x, float y, float z, int user);
        void refreshRemoteMesh(int user);
        void addGhostVertex(float x, float y, int user);
        void refreshRemoteGhost(int user);
        void changeScreenRes(int h, int v);
    
        // INITIALIZATION & UPDATER
        void initPanel();
        void oscUpdate();
        void oscUpdateAll();
        void ghostUpdate();
        void wireframeUpdate();
        void dmxUpdate();
        void updateScene();
    
    void drawWireframeLocal();
    void drawWireframeRemote();
    void drawGhostLocal();
    void drawGhostRemote();
    void drawSkeletonLocal();
    void drawSkeletonRemote();

    void nextState();

        // NETWORK FUNCTIONS
        void sendMeshTCP(const ofMesh* mesh, int user);
        void sendGhostTCP(ofPolyline* ghost_local_polyline, int user);
        void receiveTCP();
    
        void beamIn_local();
        void beamOut_local();
        void beamIn_remote();
        void beamOut_remote();
    
        float _(string name);
    
        ofxOpenNI kinect;
    
        ofVideoPlayer overlay_in_local;
        ofVideoPlayer overlay_out_local;
        ofVideoPlayer overlay_in_remote;
        ofVideoPlayer overlay_out_remote;

        ofVideoPlayer clouds;
    
        bool b_overlay_in_local = false;
        bool b_overlay_out_local = false;
        bool b_overlay_in_remote = false;
        bool b_overlay_out_remote = false;
        bool b_clouds = false;
    
        vector<ofMesh> userMeshes;
        ofMesh remoteMeshes[NUM_MAX_USERS];
        ofMesh remoteMeshesTemp[NUM_MAX_USERS];
    
        ofPath ghosts_local[5];
        ofPath ghosts_remote[5];
        ofPolyline ghosts_remote_polyline[NUM_MAX_USERS];

        ofCamera camera;
    
        ofxOscReceiver oscReceiver;
        ofxOscSender oscSender;
    
        ofVec3f mesh_local_center;
        ofVec3f mesh_remote_center;
        ofVec2f ghosts_local_center;
        ofVec2f ghosts_remote_center;        
    
        ofMesh attachment;
    
    /* NETWORK SETTINGS */
    int PORT_OSC_CONTROL_SEND = 7001;
    int PORT_OSC_CONTROL_RECEIVE = 7002;
    string IPAD_IP = "192.168.0.101";
    string SERVER_IP = "localhost";
    int LOCAL_SERVER_PORT = 7000;
    int REMOTE_SERVER_PORT = 8000;
    bool isServer = true;
    
    float lastConnectionCheck = 0;
    
    ofxTCPServer tcpServer;
    ofxTCPClient tcpClient;

    bool connected;
    
    /* DMX */
   	ofxDmx dmx;
    string ENTTEC_PORT = "tty.usbserial-EN118363";

    /* lights */
    StairvilleLEDParSpot spotInteraction1, spotInteraction2, spotCloud1, spotCloud2;
    LEDRing ledRingInteraction;
    FogMachine fogMachine;
    
    int dmx_state = 0;
    int render_mode_local = 1;
    int render_mode_remote = 0;
    
    bool renderModeChanged = true;
    bool lightStateChanged = true;
    bool settingsChanged = true;
    bool firstFrame_local = false;
    bool firstFrame_remote = false;
    
    /* SETTINGS */
    bool local_autocenter = true;
    bool remote_autocenter = false;
    float mesh_scale_local = 1500;
    float mesh_scale_remote = 1500;
    float xCorrection_local = 0;
    float xCorrection_remote = 0;
    float yCorrection_local = 0;
    float yCorrection_remote = 0;
    
    int meshResolution_local = 5;
    int meshResolution_remote = 5;
    
    ofColor colorCharacter_local = ofColor(255, 255, 255);
    ofColor colorCharacter_remote = ofColor(255, 255, 255);
    
    bool localMesh = true;
    bool remoteMesh = true;
    float zScaling = 1;
    float depthThreshold = 50;
    
    bool mirror = true;
    bool activate_network = false;
    
    float ghosts_blur_local = 11;
    float ghosts_blur_remote = 11;
    float ghosts_smoothing_local = 10;
    float ghosts_smoothing_remote = 10;
    float ghosts_local_scale = 2;
    float ghosts_remote_scale = 2;
    float skeleton_scale_local = 1.85;
    float skeleton_scale_remote = 1.85;
    
    ofFbo fboLocal;
    ofFbo fboRemote;
    
    int alpha_local = 255;
    int alpha_remote = 255;
    
    bool fadeAlpha_local = true;
    bool fadeAlpha_remote = true;
    
    int numTrackedUsers_remote;
    
    float fadeStartTime_local = 0;
    float fadeStartTime_remote = 0;
    
    bool send_ghost = false;
    
    bool resetUsers = false;
    
    int alphaClouds = 255;
    
    ofxCvContourFinder contourFinder;
    int threshold_far = 60;
    int threshold_near = 0;

    
    ofxCvGrayscaleImage cvImage;
    ofxCvGrayscaleImage cv_threshold1;
    ofxCvGrayscaleImage cv_threshold2;
    
    bool showDebug = false;
    int numContours = 0;
//    
//
//    
//    float interaction_pulse_min = 100;
//    float interaction_pulse_max = 200;
//    float brightness_interaction = 100;
//    float fade_time = 3;
//    
//    float brightness_cloud = 100;
//    float cloud_fadeIn_time = 2;
//    ofColor color_cloud = ofColor(255, 255, 255);
//    
//    float fog_level = 255;
//    bool fog = false;
};
