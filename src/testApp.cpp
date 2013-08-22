#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofSetFrameRate(25);
    ofLogLevel(OF_LOG_WARNING);
    
    //changeScreenRes(1024, 768);
    
    
    /* LOAD SETTINGS */
    ofBuffer buffer = ofBufferFromFile("settings.config");
    if(buffer.size()) {
        while(buffer.isLastLine() == false) {
            
            // move on to the next line
            string line = buffer.getNextLine();
            string argument = line.substr(0, line.find(","));
            string value = line.substr(line.find(",")+1);
            
            if( argument == "SERVER_IP" ){ SERVER_IP = value; }
            else if ( argument == "LOCAL_SERVER_PORT" ) { LOCAL_SERVER_PORT = ofToInt(value);  }
            else if ( argument == "REMOTE_SERVER_PORT" ) { REMOTE_SERVER_PORT = ofToInt(value);  }
            else if ( argument == "ISSERVER" ) { if(value=="1") isServer = true; else isServer = false; }

            else if ( argument == "PORT_OSC_CONTROL_SEND" ) { PORT_OSC_CONTROL_SEND = ofToInt(value); }
            else if ( argument == "PORT_OSC_CONTROL_RECEIVE" ) { PORT_OSC_CONTROL_RECEIVE = ofToInt(value); }
            else if ( argument == "IPAD_IP" ) { IPAD_IP = value; }
            else if ( argument == "ENTTEC_PORT" ) { ENTTEC_PORT = value; }

            else if ( argument == "MESH_SCALE" ) { mesh_scale_local = ofToFloat(value); mesh_scale_remote = ofToFloat(value); }

            else if ( argument == "MIRROR" ) { if(value=="1") mirror = true; else mirror = false; }
            else if ( argument == "ACTIVATE_NETWORK" ) { if(value=="1") activate_network = true; else activate_network = false; }

            else if ( argument == "LED_PULSE_MIN" ) { ledRingInteraction.setPulseMin(ofToInt(value)); }
            else if ( argument == "LED_PULSE_MAX" ) { ledRingInteraction.setPulseMax(ofToInt(value)); }
            else if ( argument == "PULSE_SPEED" ) { ledRingInteraction.setPulseSpeed(ofToInt(value)); spotInteraction1.setPulseSpeed(ofToInt(value)); spotInteraction2.setPulseSpeed(ofToInt(value)); }
            else if ( argument == "LED_STATIC" ) { ledRingInteraction.setStaticBrightness(ofToInt(value)); }
            else if ( argument == "INTERACTION_PULSE_MIN" ) { spotInteraction1.setPulseMin(ofToInt(value)); spotInteraction2.setPulseMin(ofToInt(value)); }
            else if ( argument == "INTERACTION_PULSE_MAX" ) { spotInteraction1.setPulseMax(ofToInt(value)); spotInteraction2.setPulseMax(ofToInt(value)); }
            else if ( argument == "BRIGHTNESS_INTERACTION" ) { spotInteraction1.setStaticBrightness(ofToInt(value)); spotInteraction2.setStaticBrightness(ofToInt(value)); }
            else if ( argument == "FADE_TIME" ) { spotInteraction1.setFadeTime(ofToInt(value)); spotInteraction2.setFadeTime(ofToInt(value)); }
            else if ( argument == "BRIGHTNESS_CLOUD" ) { spotCloud1.setStaticBrightness(ofToInt(value)); spotCloud2.setStaticBrightness(ofToInt(value)); }
            else if ( argument == "CLOUD_FADEIN_TIME" ) { spotCloud1.setFadeTime(ofToFloat(value)); }
        }
    }
    else ofLog() << "settings.config not found!";
        
	
    /* KINECT SETUP */
    kinect.setLed(ofxKinect::LED_OFF);
    kinect.init(false, false);      // disable video image (faster fps)
	kinect.setRegistration(true);   // enable depth->video image calibration
    kinect.setCameraTiltAngle(0);
    
    // TODO: add possibility to playback recorded kinect data
    
    // GUI
    gui.setup(); // most of the time you don't need a name but don't forget to call setup
	gui.add(local_mesh.near_threshold.set("near threshold", 0, 0, 10000));
    gui.add(local_mesh.far_threshold.set("far threshold", 2000, 0, 10000));
    gui.add(local_mesh.depth_threshold.set("depth threshold", 50, 0, 500));
    gui.add(local_mesh.mesh_resolution.set("mesh resolution", 5, 1, 20));
    
    
    /* VIDEO LAYERS */
    overlay_in_local.setPixelFormat(OF_PIXELS_RGBA);
	overlay_in_local.loadMovie("tornado-beam_in.mov");
    overlay_in_local.setLoopState(OF_LOOP_NONE);
    
    overlay_out_local.setPixelFormat(OF_PIXELS_RGBA);
	overlay_out_local.loadMovie("tornado-beam_out.mov");
    overlay_out_local.setLoopState(OF_LOOP_NONE);
    
    overlay_in_remote.setPixelFormat(OF_PIXELS_RGBA);
	overlay_in_remote.loadMovie("tornado-beam_in.mov");
    overlay_in_remote.setLoopState(OF_LOOP_NONE);
    
    overlay_out_remote.setPixelFormat(OF_PIXELS_RGBA);
	overlay_out_remote.loadMovie("tornado-beam_out.mov");
    overlay_out_remote.setLoopState(OF_LOOP_NONE);
    
    /* OSC SETUP */
    oscReceiver.setup(PORT_OSC_CONTROL_RECEIVE);    
    oscSender.setup(IPAD_IP, PORT_OSC_CONTROL_SEND);
    
    // push all settings to the ipad
    lightStateChanged = true;
    oscUpdateAll();
    
    /* DMX SETUP */
    dmx.connect(ENTTEC_PORT, 35);
    spotInteraction1.setup(&dmx, 8);
    spotInteraction2.setup(&dmx, 15);
    spotCloud1.setup(&dmx, 22);
    spotCloud2.setup(&dmx, 29);
    ledRingInteraction.setup(&dmx, 2);
    fogMachine.setup(&dmx, 1);
    
    dmxUpdate();
    
//    ofHideCursor();
    
    fboLocal.allocate(ofGetWidth(), ofGetHeight());
    fboRemote.allocate(ofGetWidth(), ofGetHeight());

    
    
}


//--------------------------------------------------------------
void testApp::update() {

    // update kinect and the mesh data
    kinect.update();
    local_mesh.updateFromKinect(&kinect);

    remote_mesh.updateFromNetwork(); // TODO: this function is not implemented yet
    
    oscUpdate();
    
    /* UPDATE VIDEOS */
    overlay_in_local.update();
    overlay_out_local.update();
    overlay_in_remote.update();
    overlay_out_remote.update();
    
    /* update DMX */
    updateScene();
    dmxUpdate();
}

//--------------------------------------------------------------
void testApp::draw() {
    ofNoFill();
    ofPushMatrix();
    
    ofBackground(0, 0, 0);
	
    fboLocal.begin();
        ofPushStyle();
            ofClear(0,0,0, 0);
            ofSetColor(colorCharacter_local, alpha_local);
            camera.setGlobalPosition(0, 0, mesh_scale_local);
            camera.begin();
    
            local_mesh.draw();
    
            camera.end();
    
            // set alpha back to 255 when drawing the beam
            ofSetColor(colorCharacter_local, 255);
            if(b_overlay_in_local){
                ofEnableAlphaBlending();
                overlay_in_local.draw(0,0, ofGetWidth(), ofGetHeight());
                ofDisableAlphaBlending();
                firstFrame_local = false;
            }
            if(b_overlay_out_local){
                ofEnableAlphaBlending();
                overlay_out_local.draw(0,0, ofGetWidth(), ofGetHeight());
                ofDisableAlphaBlending();
            }
        ofPopStyle();
    fboLocal.end();
    
    if(!fadeAlpha_local && ofGetElapsedTimef() - fadeStartTime_local > 2)
    {
        if(alpha_local > 0)
            alpha_local-=7;
        else alpha_local = 0;
    }
    else if (ofGetElapsedTimef() - fadeStartTime_local > 3)
    {
        if(alpha_local < 255)
            alpha_local+=7;
        else alpha_local=255;
    }
    
    // eigentlich ist der server relevant, aber der scheint nicht wirklich zu wissen, wie viele clients mit ihm verbunden sind (oder es wird nur langsam geupdated). deshalb gehe ich den Weg Ÿber den Client. Wenn der nicht verbunden ist, ist der Server auch nicht verbunden.
    if(remote_mesh.isConnected()){
        fboRemote.begin();
            ofPushStyle();
                ofClear(0,0,0, 0);
                ofSetColor(colorCharacter_remote, alpha_remote);

                camera.setGlobalPosition(0, 0, mesh_scale_remote);
                camera.begin();
        
                remote_mesh.draw();
        
                camera.end();
        
                // set alpha back to 255 when drawing the beam
                ofSetColor(colorCharacter_remote, 255);
                if(b_overlay_in_remote){
                    ofEnableAlphaBlending();
                    overlay_in_remote.draw(0,0, ofGetWidth(), ofGetHeight());
                    ofDisableAlphaBlending();
                    firstFrame_remote = false;
                }
                if(b_overlay_out_remote){
                    ofEnableAlphaBlending();
                    overlay_out_remote.draw(0,0, ofGetWidth(), ofGetHeight());
                    ofDisableAlphaBlending();
                }
            ofPopStyle();
        fboRemote.end();
    }

    
    if(!fadeAlpha_remote && ofGetElapsedTimef() - fadeStartTime_remote > 2)
    {
        if(alpha_remote > 0)
            alpha_remote-=7;
        else alpha_remote = 0;
    }
    else if (ofGetElapsedTimef() - fadeStartTime_remote > 2)
    {
        if(alpha_remote < 255)
            alpha_remote+=7;
        else alpha_remote = 255;
    }
    
    
    ofEnableAlphaBlending();
        fboLocal.draw(xCorrection_local, -yCorrection_local);
        if(remote_mesh.isConnected()) fboRemote.draw(xCorrection_remote, -yCorrection_remote);
    ofDisableAlphaBlending();

    ofPopMatrix();
    
    gui.draw();
}

//--------------------------------------------------------------
void testApp::nextState(){
    dmx_state++;
    if(dmx_state>5) dmx_state = 1;
    lightStateChanged = true;
    ofLog() << "entering state " << ofToString(dmx_state);
}

//--------------------------------------------------------------
void testApp::updateScene(){
    if(lightStateChanged){
        lightStateChanged = false;
        
        switch(dmx_state){
            // blackout mode
            case 0:
                spotInteraction1.stopPulse();
                spotInteraction2.stopPulse();
                spotInteraction1.fadeOut();
                spotInteraction2.fadeOut();
                ledRingInteraction.stopPulse(); 
                ledRingInteraction.fadeOut();
                spotCloud1.stopPulse();
                spotCloud2.stopPulse();
                spotCloud1.fadeOut();
                spotCloud2.fadeOut();
                break;
            case 1:
                spotInteraction1.stopPulse();
                spotInteraction2.stopPulse();
                spotInteraction1.fadeOut();
                spotInteraction2.fadeOut();
                spotCloud1.stopPulse();
                spotCloud2.stopPulse();
                spotCloud1.fadeOut();
                spotCloud2.fadeOut();
                ledRingInteraction.pulseBrightness();
                break;
            case 2:
                ledRingInteraction.pulseBrightness();
                spotInteraction1.pulseBrightness();
                spotInteraction2.pulseBrightness();
                spotCloud1.stopPulse();
                spotCloud2.stopPulse();
                spotCloud1.fadeOut();
                spotCloud2.fadeOut();
                break;
            case 3: // fog start
                ledRingInteraction.stopPulse();
                ledRingInteraction.fadeIn();
                spotInteraction1.stopPulse();
                spotInteraction2.stopPulse();
                spotInteraction1.fadeIn();
                spotInteraction2.fadeIn();
                fogMachine.fogOn();
                spotCloud1.fadeIn();
                spotCloud2.fadeIn();
                break;
            case 4: // beam in
                fogMachine.fogOff();
                beamIn_local();
                
                // shut down cloud and interaction spots
                spotInteraction1.fadeOut();
                spotInteraction2.fadeOut();
                spotCloud1.fadeOut();
                spotCloud2.fadeOut();
                break;
            case 5: // beam out
                beamOut_local();
                spotInteraction1.fadeIn();
                spotInteraction2.fadeIn();
                break;
        }
        
        ofLog() << "switched light state to " << dmx_state;
    }
}

//--------------------------------------------------------------
void testApp::dmxUpdate(){
    
    // update the devices
    spotInteraction1.update();
    spotInteraction2.update();
    spotCloud1.update();
    spotCloud2.update();
    ledRingInteraction.update();
    fogMachine.update();
    
    // push new data to the enttec device
    dmx.update();
}

//--------------------------------------------------------------
void testApp::exit() {
//    tcpServer.close();
//    tcpClient.close();
    ofLog() << "server & client stopped";
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void testApp::beamIn_local(){
    if(!b_overlay_in_local){
        overlay_in_local.play();
        b_overlay_in_local = true;
        b_overlay_out_local = false;
        firstFrame_local = true;
        fadeAlpha_local = true;
        fadeStartTime_local = ofGetElapsedTimef();
    }
}

//--------------------------------------------------------------
void testApp::beamIn_remote(){
    if(!b_overlay_in_remote){
        overlay_in_remote.play();
        b_overlay_in_remote = true;
        b_overlay_out_remote = false;
        firstFrame_remote = true;
        fadeAlpha_remote = true;
        fadeStartTime_remote = ofGetElapsedTimef();
    }
}

//--------------------------------------------------------------
void testApp::beamOut_local(){
    if(b_overlay_in_local)
    {
        overlay_out_local.play();
        b_overlay_in_local = false;
        b_overlay_out_local = true;
        fadeAlpha_local = false;
        fadeStartTime_local = ofGetElapsedTimef();
    }
}

//--------------------------------------------------------------
void testApp::beamOut_remote(){
    if(b_overlay_in_remote)
    {
        overlay_out_remote.play();
        b_overlay_in_remote = false;
        b_overlay_out_remote = true;
        fadeAlpha_remote = false;
        fadeStartTime_remote = ofGetElapsedTimef();
    }
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch (key) {
        case 'i':
            beamIn_local();
			break;
        
        case 'o':
            beamOut_local();
            break;
            
        case '+':
            nextState();
            break;
                        
        case 'x':
            xCorrection_local++;
            break;
                        
        case 'd':
            showDebug = !showDebug;
            break;
    }
}

//--------------------------------------------------------------
void testApp::oscUpdate()
{
    // it might be a bit too heavy to send all the settings on every frame to the ipad
    
    while(oscReceiver.hasWaitingMessages()){
        ofxOscMessage m;
        oscReceiver.getNextMessage(&m);
                
        // light state
        if (m.getAddress() == "/lightState/0") { dmx_state = 0; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/1") { dmx_state = 1; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/2") { dmx_state = 2; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/3") { dmx_state = 3; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/4") { dmx_state = 4; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/5") { dmx_state = 5; lightStateChanged = true; }
        
        // mesh settings
        else if (m.getAddress() == "/mesh/local/scale") { mesh_scale_local = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/mesh/remote/scale") { mesh_scale_remote = m.getArgAsFloat(0); }

        
        // centering
        else if (m.getAddress() == "/centering/local/correction") { yCorrection_local = m.getArgAsFloat(0); xCorrection_local = m.getArgAsFloat(1); }
        else if (m.getAddress() == "/centering/remote/correction") { yCorrection_remote = m.getArgAsFloat(0); xCorrection_remote = m.getArgAsFloat(1); }
        else if (m.getAddress() == "/centering/autolocal") { local_autocenter =  bool(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/centering/autoremote") { remote_autocenter = bool(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/mirror") { mirror = bool(m.getArgAsFloat(0)); }
        
        // fog settings
        else if (m.getAddress() == "/fog/level") { fogMachine.setLevel(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/fog/state") { 
            if(m.getArgAsFloat(0) >= 0.9 && m.getArgAsFloat(0) <= 1.1){
                fogMachine.fogOn();
            }
            else if
                (m.getArgAsFloat(0) >= 0 && m.getArgAsFloat(0) <= 0.1){
                    fogMachine.fogOff();
                }
        }
        
        // color settings
        else if (m.getAddress() == "/color/local/3") { colorCharacter_local.r = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/color/local/2") { colorCharacter_local.g = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/color/local/1") { colorCharacter_local.b = m.getArgAsFloat(0); }
        
        else if (m.getAddress() == "/color/remote/3") { colorCharacter_remote.r = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/color/remote/2") { colorCharacter_remote.g = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/color/remote/1") { colorCharacter_remote.b = m.getArgAsFloat(0); }
        
        // lighting settings
        else if (m.getAddress() == "/spots/cloud/color/3") { spotCloud1.setRed(m.getArgAsFloat(0)); spotCloud2.setRed(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/spots/cloud/color/2") { spotCloud1.setGreen(m.getArgAsFloat(0)); spotCloud2.setGreen(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/spots/cloud/color/1") { spotCloud1.setBlue(m.getArgAsFloat(0)); spotCloud2.setBlue(m.getArgAsFloat(0)); }
        
        else if (m.getAddress() == "/spots/cloud/brightness") { spotCloud1.setStaticBrightness(m.getArgAsFloat(0)); spotCloud2.setStaticBrightness(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/spots/cloud/fadetime") { spotCloud1.setFadeTime(m.getArgAsFloat(0)); spotCloud2.setFadeTime(m.getArgAsFloat(0)); }

        else if (m.getAddress() == "/led/interaction/pulse_bright_min") { ledRingInteraction.setPulseMin(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/led/interaction/pulse_bright_max") { ledRingInteraction.setPulseMax(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/led/interaction/pulse_speed") { ledRingInteraction.setPulseSpeed(m.getArgAsFloat(0)); spotInteraction1.setPulseSpeed(m.getArgAsFloat(0)); spotInteraction2.setPulseSpeed(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/led/interaction/static_brightness") { ledRingInteraction.setStaticBrightness(m.getArgAsFloat(0)); }
        
        else if (m.getAddress() == "/spots/interaction/pulse/brightness/min") { spotInteraction1.setPulseMin(m.getArgAsFloat(0)); spotInteraction2.setPulseMin(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/spots/interaction/pulse/brightness/max") { spotInteraction1.setPulseMax(m.getArgAsFloat(0)); spotInteraction2.setPulseMax(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/spots/interaction/brightness") { spotInteraction1.setStaticBrightness(m.getArgAsFloat(0)); spotInteraction2.setStaticBrightness(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/spots/interaction/fadetime") { spotInteraction1.setFadeTime(m.getArgAsFloat(0)); }
        
        // settings
        else if (m.getAddress() == "/settings/save") { /* TODO: save settings */ }
        else if (m.getAddress() == "/settings/load") { /* TODO: load settings */ }
        else if (m.getAddress() == "/settings/updateall") { oscUpdateAll(); }
        
        else if (m.getAddress() == "/remote/in") { beamIn_remote(); }
        else if (m.getAddress() == "/remote/out") { beamOut_remote(); }
        
    }
    
    // TODO: make this a bit more elegant!
    oscUpdateAll();
}

void testApp::oscUpdateAll(){
    /* update the interface on the ipad */
    ofxOscMessage updater;
    
    if(lightStateChanged){
    // light states
    //lightStateChanged = false;
        if(dmx_state == 0) {
            updater.clear();
            updater.setAddress("/lightState/0"); updater.addFloatArg(1);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/1"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/2"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/3"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/4"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/5"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
        }
        
        if(dmx_state == 1) {
            updater.clear();
            updater.setAddress("/lightState/0"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/1"); updater.addFloatArg(1);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/2"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/3"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/4"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/5"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
        }
        else if(dmx_state == 2) {
            updater.clear();
            updater.setAddress("/lightState/0"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/1"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/2"); updater.addFloatArg(1);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/3"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/4"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/5"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
        }
        else if(dmx_state == 3) {
            updater.clear();
            updater.setAddress("/lightState/0"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/1"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/2"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/3"); updater.addFloatArg(1);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/4"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/5"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
        }
        else if(dmx_state == 4) {
            updater.clear();
            updater.setAddress("/lightState/0"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/1"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/2"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/3"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/4"); updater.addFloatArg(1);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/5"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
        }
        else if(dmx_state == 5) {
            updater.clear();
            updater.setAddress("/lightState/0"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/1"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/2"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/3"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/4"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/5"); updater.addFloatArg(1);
            oscSender.sendMessage(updater);
        }
    }
    
    
    // mesh settings
    updater.clear();
    updater.setAddress("/mesh/local/scale"); updater.addFloatArg(mesh_scale_local);
    oscSender.sendMessage(updater);
        
    // centering
    updater.clear();
    updater.setAddress("/centering/local/correction"); updater.addFloatArg(yCorrection_local); updater.addFloatArg(xCorrection_local);
    oscSender.sendMessage(updater);

    updater.clear();
    updater.setAddress("/centering/remote/correction"); updater.addFloatArg(yCorrection_remote); updater.addFloatArg(xCorrection_remote);
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/centering/autolocal"); updater.addFloatArg(local_autocenter);
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/centering/autoremote"); updater.addFloatArg(remote_autocenter);
    oscSender.sendMessage(updater);
    
    // fog settings
    updater.clear();
    updater.setAddress("/fog/level"); updater.addFloatArg(fogMachine.getLevel());
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/fog/state");
    if(fogMachine.isFogging())
        updater.addFloatArg(1);
    else
        updater.addFloatArg(0);
    oscSender.sendMessage(updater);
    
    // color settings
    updater.clear();
    updater.setAddress("/color/local/3"); updater.addFloatArg(colorCharacter_local.r);
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/color/local/2"); updater.addFloatArg(colorCharacter_local.g);
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/color/local/1"); updater.addFloatArg(colorCharacter_local.b);
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/color/remote/3"); updater.addFloatArg(colorCharacter_remote.r);
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/color/remote/2"); updater.addFloatArg(colorCharacter_remote.g);
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/color/remote/1"); updater.addFloatArg(colorCharacter_remote.b);
    oscSender.sendMessage(updater);
    
    
    // lighting settings
    updater.clear();
    updater.setAddress("/spots/cloud/color/3"); updater.addFloatArg(spotCloud1.getRed());
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/spots/cloud/color/2"); updater.addFloatArg(spotCloud1.getGreen());
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/spots/cloud/color/1"); updater.addFloatArg(spotCloud1.getBlue());
    oscSender.sendMessage(updater);
    
    
    updater.clear();
    updater.setAddress("/spots/cloud/brightness"); updater.addFloatArg(spotCloud1.getStaticBrightness());
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/spots/cloud/fadetime"); updater.addFloatArg(spotCloud1.getFadeTime());
    oscSender.sendMessage(updater);
    
    
    updater.clear();
    updater.setAddress("/led/interaction/pulse_bright_min"); updater.addFloatArg(ledRingInteraction.getPulseMin());
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/led/interaction/pulse_bright_max"); updater.addFloatArg(ledRingInteraction.getPulseMax());
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/led/interaction/pulse_speed"); updater.addFloatArg(ledRingInteraction.getPulseSpeed());
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/led/interaction/static_brightness"); updater.addFloatArg(ledRingInteraction.getStaticBrightness());
    oscSender.sendMessage(updater);
    
    
    updater.clear();
    updater.setAddress("/spots/interaction/pulse/brightness/min"); updater.addFloatArg(spotInteraction1.getPulseMin());
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/spots/interaction/pulse/brightness/max"); updater.addFloatArg(spotInteraction1.getPulseMax());
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/spots/interaction/brightness"); updater.addFloatArg(spotInteraction1.getStaticBrightness());
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/spots/interaction/fadetime"); updater.addFloatArg(spotInteraction1.getFadeTime());
    oscSender.sendMessage(updater);
}


