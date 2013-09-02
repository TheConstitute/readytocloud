#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
//	ofSetLogLevel(OF_LOG_VERBOSE);
    
//    ofSetFrameRate(25);
	
    /* KINECT SETUP */
	kinect.setRegistration(true);   // enable depth->video image calibration
    kinect.init();
    kinect.open();
    kinect.setCameraTiltAngle(0);
    kinect.setLed(ofxKinect::LED_OFF);
    
    // TODO: add possibility to playback recorded kinect data
    
    
    // GUI
    local_mesh_parameters.setName("local mesh parameters");
    local_mesh_parameters.add(local_mesh.near_threshold.set("near threshold", 0, 0, 10000));
    local_mesh_parameters.add(local_mesh.far_threshold.set("far threshold", 2000, 0, 10000));
    local_mesh_parameters.add(local_mesh.depth_threshold_max.set("depth threshold max", 50, 0, 500));
    local_mesh_parameters.add(local_mesh.mesh_resolution_x.set("mesh resolution x", 5, 1, 20));
    local_mesh_parameters.add(local_mesh.mesh_resolution_y.set("mesh resolution y", 5, 1, 20));
    local_mesh_parameters.add(local_mesh_scale.set("mesh scale local", 1700, -5000, 5000));
    local_mesh_parameters.add(local_mesh.cv_near_threshold.set("cv near threshold", 0, 0, 255));
    local_mesh_parameters.add(local_mesh.cv_far_threshold.set("cv far threshold", 255, 0, 255));
    local_mesh_parameters.add(local_mesh.draw_contour.set("draw contour", false));
    local_mesh_parameters.add(line_width.set("line width", 1, 1, 10));
    local_mesh_parameters.add(local_mesh.mesh_mode.set("mesh mode", 2, 0, 2));
    local_mesh_parameters.add(fov.set("camera FOV", 60, 1, 180));
    local_mesh_parameters.add(camera_offset_y.set("camer offset y", -500, -1000, 1000));
    local_mesh_parameters.add(camera_angle.set("camera angle", 0, -90, 90));
    local_mesh_parameters.add(kinect_angle.set("kinect angle", 0, -30, 30));
    local_mesh_parameters.add(x_correction_local.set("x correction local", 0, -1000, 1000));
    local_mesh_parameters.add(y_correction_local.set("y correction local", 0, -1000, 1000));
    local_mesh_parameters.add(local_mesh.mirror.set("mirror", false));

    network_parameters.setName("network parameters");
    network_parameters.add(hide_remote.set("hide remote", false));
    network_parameters.add(activate_network.set("activate network", true));
    network_parameters.add(local_port.set("local port", 7000));
    network_parameters.add(server_ip.set("server ip", "localhost"));
    network_parameters.add(remote_port.set("remote port", 8000));
    network_parameters.add(state_sync_port_local.set("state_sync_port_local", 6666, 0, 10000));
    network_parameters.add(state_sync_port_remote.set("state_sync_port_remote", 6667, 0, 10000));
    
    osc_parameters.setName("osc parameters");
    osc_parameters.add(osc_port_send.set("osc port send", 7001));
    osc_parameters.add(osc_port_receive.set("osc port receive", 7002));
    osc_parameters.add(osc_ipad_ip.set("osc ipad ip", "192.168.0.101"));
    
    dmx_parameters.setName("dmx parameters");
    dmx_parameters.add(dmx_entec_port.set("dmx entec port", "tty.usbserial-EN118363"));
    dmx_parameters.add(dmx_state.set("dmx state", 0, 0, 6));
    dmx_parameters.add(dmx_state_remote.set("dmx state remote", 0, 0, 6));
    
    light_parameters.setName("light parameters");
    light_parameters.add(ledRingInteraction.pulseMin.set("ring pulse min", 8, 0, 255));
    light_parameters.add(ledRingInteraction.pulseMax.set("ring pulse max", 255, 0, 255));
    light_parameters.add(ledRingInteraction.static_brightness.set("ring brightness", 255, 0, 255));
    light_parameters.add(ledRingInteraction.pulseSpeed.set("pulse speed", 4, 0, 255));
    light_parameters.add(spotInteraction1.pulseMin.set("spot pulse min", 20, 0, 255));
    light_parameters.add(spotInteraction1.pulseMax.set("spot pulse max", 160, 0, 255));
    light_parameters.add(spotInteraction1.static_brightness.set("spot brightness", 32, 0, 255)); 
    light_parameters.add(spotInteraction1.fadeTime.set("fade time", 2, 0, 10));
    light_parameters.add(spotCloud1.static_brightness.set("spot cloud brightness", 255, 0, 255));
    light_parameters.add(spotCloud1.fadeTime.set("cloud fade time", 3, 0, 10));
    
    gui.setup(); 
    gui.add(local_mesh_parameters);
    gui.add(network_parameters);
    gui.add(osc_parameters);
    gui.add(dmx_parameters);
    gui.add(light_parameters);
    gui.loadFromFile("settings.xml");
    
    
    /* DMX SETUP */
    dmx.connect(dmx_entec_port, 35);
    spotInteraction1.setup(&dmx, 8);
    spotInteraction2.setup(&dmx, 15);
    spotCloud1.setup(&dmx, 22);
    spotCloud2.setup(&dmx, 29);
    ledRingInteraction.setup(&dmx, 2);
    fogMachine.setup(&dmx, 1);
    spotCloud1.setColor(255, 0, 0);
    spotCloud1.setBrightness(255);
    dmxUpdate();
    
    // gui3d
    gui3d.setup("positioning", "positioning.xml", 800, 30);
    gui3d.add(local_mesh.offset.set("local mesh offset", ofVec3f(0, 0, -3000), ofVec3f(-5000,-5000,-5000), ofVec3f(5000,5000,5000)));
    gui3d.loadFromFile("positioning.xml");
    
    draw_gui3d = false;
    draw_gui = false;
    
    if(activate_network){
        mesh_transceiver.setup(local_port, server_ip, remote_port);
        remote_mesh.setup(&mesh_transceiver);
    }
    
    local_mesh.setup(&mesh_transceiver, &kinect);
    
    if(activate_network) mesh_interactor.setup(&local_mesh, &remote_mesh);
    
    /* OSC SETUP */
    state_sender.setup(server_ip, state_sync_port_remote);
    state_receiver.setup(state_sync_port_local);
    ipadSender.setup(osc_ipad_ip, osc_port_send);
    ipadReceiver.setup(osc_port_receive);
    
    dmx_state.addListener(this, &testApp::stateChanged);
    
    use_easy_cam = false;
    draw_grid = false;
    
    
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
    

    
    // push all settings to the ipad
    lightStateChanged = true;
    oscUpdateAll();

//    ofHideCursor();
    
    fboLocal.allocate(ofGetWidth(), ofGetHeight());

    kinect.setCameraTiltAngle(kinect_angle);
    
}


//--------------------------------------------------------------
void testApp::update() {

    // update the mesh data
    local_mesh.update();
    if(activate_network) remote_mesh.update();
    
    // only update if one of the meshes has received new data
    if(local_mesh.isFrameNew() || remote_mesh.isFrameNew())
        mesh_interactor.update();
    
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
    ofBackground(0, 0, 0); // paint the background black
    ofPushMatrix();
	ofSetLineWidth(line_width);
    fboLocal.begin();
        ofPushStyle();
        ofClear(0, 0, 0, 0);
        ofSetColor(colorCharacter_local);

        camera.setGlobalPosition(0, camera_offset_y, local_mesh_scale);
        camera.setFov(fov);
        camera.setOrientation(ofVec3f(camera_angle, 0, 0));
        if(use_easy_cam)easyCam.begin();
        else camera.begin();

        if(draw_grid) ofDrawGrid(1000);
        ofScale(1, -1, -1);

        local_mesh.draw();
        if(mesh_transceiver.isConnected() && !hide_remote) remote_mesh.draw();
        mesh_interactor.draw();

        if(use_easy_cam)easyCam.end();
        else camera.end();
        ofPopStyle();
    fboLocal.end();
    
    fboLocal.draw(x_correction_local, -y_correction_local);

    ofPopMatrix();
    
    // synchronize the settings of the light pairs
    spotInteraction1.pulseSpeed = ledRingInteraction.pulseSpeed;
    spotInteraction2.pulseSpeed = ledRingInteraction.pulseSpeed;
    spotInteraction2.pulseMin = spotInteraction1.pulseMin;
    spotInteraction2.pulseMax = spotInteraction1.pulseMax;
    spotInteraction2.static_brightness = spotInteraction1.static_brightness;
    spotCloud2.fadeTime = spotCloud1.fadeTime;
    spotCloud2.static_brightness = spotCloud1.static_brightness;
    spotCloud2.fadeTime = spotCloud1.fadeTime;
    
    if(draw_debug)
        local_mesh.drawDebug();
    if(draw_gui){
        gui.draw();
        ofDrawBitmapString("r: \t" + ofToString(mesh_transceiver.getNumBytesReceived()), ofGetWidth() - 200, ofGetHeight() - 45);
        ofDrawBitmapString("s: \t" + ofToString(mesh_transceiver.getNumBytesSent()), ofGetWidth() - 200, ofGetHeight() - 30);
        ofDrawBitmapString("is connected: \t" + ofToString(mesh_transceiver.isConnected()), ofGetWidth() - 200, ofGetHeight() - 15);
    }
    if(draw_gui3d)
        gui3d.draw();
    
}

//--------------------------------------------------------------
void testApp::nextState(){
    dmx_state++;
    if(dmx_state>5) dmx_state = 1;
    lightStateChanged = true;
    ofLog() << "entering state " << ofToString(dmx_state);
}

//--------------------------------------------------------------
void testApp::stateChanged(int &state){
    ofxOscMessage message;
    message.setAddress("/dmx_state/" + ofToString(state));
    state_sender.sendMessage(message);
    if(state == 1)
        local_mesh.beamOut();
    else if (state == 2)
        local_mesh.beamIn();
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
            case 5:
                beamIn_remote();                
            case 6: // beam out
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
    mesh_transceiver.disconnect();
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
            x_correction_local++;
            break;
                        
        case 'd':
            draw_debug = !draw_debug;
            break;
        case 'h':
            draw_gui = !draw_gui;
            break;
        case 'c':
            use_easy_cam = !use_easy_cam;
            break;
        case 'g':
            draw_grid = !draw_grid;
            break;
        case 'p':
            draw_gui3d = !draw_gui3d;
            break;
        case OF_KEY_UP:
			kinect_angle++;
			if(kinect_angle>30) kinect_angle=30;
            kinect.setCameraTiltAngle(kinect_angle);
			break;
			
		case OF_KEY_DOWN:
			kinect_angle--;
			if(kinect_angle<-30) kinect_angle=-30;
            kinect.setCameraTiltAngle(kinect_angle);
			break;

    }
}

//--------------------------------------------------------------
void testApp::oscUpdate()
{
    
    while(state_receiver.hasWaitingMessages()){
        ofxOscMessage m;
        state_receiver.getNextMessage(&m);
        
        if (m.getAddress() == "/dmx_state/0") { dmx_state = 0; }
        else if (m.getAddress() == "/dmx_state/1") { dmx_state_remote= 1;  }
        else if (m.getAddress() == "/dmx_state/2") { dmx_state_remote = 2; }
        else if (m.getAddress() == "/dmx_state/3") { dmx_state_remote = 3; }
        else if (m.getAddress() == "/dmx_state/4") { dmx_state_remote = 4; }
        else if (m.getAddress() == "/dmx_state/5") { dmx_state_remote = 5; }
    }
    
    
    // it might be a bit too heavy to send all the settings on every frame to the ipad
    
    while(ipadReceiver.hasWaitingMessages()){
        ofxOscMessage m;
        ipadReceiver.getNextMessage(&m);
                
        // light state
        if (m.getAddress() == "/lightState/0") { dmx_state = 0; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/1") { dmx_state = 1; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/2") { dmx_state = 2; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/3") { dmx_state = 3; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/4") { dmx_state = 4; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/5") { dmx_state = 5; lightStateChanged = true; }
        
        // mesh settings
        else if (m.getAddress() == "/mesh/local/scale") { local_mesh_scale = m.getArgAsFloat(0); }

        
        // centering
        else if (m.getAddress() == "/centering/local/correction") { y_correction_local = m.getArgAsFloat(0); x_correction_local = m.getArgAsFloat(1); }
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
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/1"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/2"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/3"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/4"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/5"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
        }
        
        if(dmx_state == 1) {
            updater.clear();
            updater.setAddress("/lightState/0"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/1"); updater.addFloatArg(1);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/2"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/3"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/4"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/5"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
        }
        else if(dmx_state == 2) {
            updater.clear();
            updater.setAddress("/lightState/0"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/1"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/2"); updater.addFloatArg(1);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/3"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/4"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/5"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
        }
        else if(dmx_state == 3) {
            updater.clear();
            updater.setAddress("/lightState/0"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/1"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/2"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/3"); updater.addFloatArg(1);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/4"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/5"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
        }
        else if(dmx_state == 4) {
            updater.clear();
            updater.setAddress("/lightState/0"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/1"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/2"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/3"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/4"); updater.addFloatArg(1);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/5"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
        }
        else if(dmx_state == 5) {
            updater.clear();
            updater.setAddress("/lightState/0"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/1"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/2"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/3"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/4"); updater.addFloatArg(0);
            ipadSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/lightState/5"); updater.addFloatArg(1);
            ipadSender.sendMessage(updater);
        }
    }
    
    
    // mesh settings
    updater.clear();
    updater.setAddress("/mesh/local/scale"); updater.addFloatArg(local_mesh_scale);
    ipadSender.sendMessage(updater);
        
    // centering
    updater.clear();
    updater.setAddress("/centering/local/correction"); updater.addFloatArg(y_correction_local); updater.addFloatArg(x_correction_local);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/centering/autolocal"); updater.addFloatArg(local_autocenter);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/centering/autoremote"); updater.addFloatArg(remote_autocenter);
    ipadSender.sendMessage(updater);
    
    // fog settings
    updater.clear();
    updater.setAddress("/fog/level"); updater.addFloatArg(fogMachine.getLevel());
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/fog/state");
    if(fogMachine.isFogging())
        updater.addFloatArg(1);
    else
        updater.addFloatArg(0);
    ipadSender.sendMessage(updater);
    
    // color settings
    updater.clear();
    updater.setAddress("/color/local/3"); updater.addFloatArg(colorCharacter_local.r);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/color/local/2"); updater.addFloatArg(colorCharacter_local.g);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/color/local/1"); updater.addFloatArg(colorCharacter_local.b);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/color/remote/3"); updater.addFloatArg(colorCharacter_remote.r);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/color/remote/2"); updater.addFloatArg(colorCharacter_remote.g);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/color/remote/1"); updater.addFloatArg(colorCharacter_remote.b);
    ipadSender.sendMessage(updater);
    
    
    // lighting settings
    updater.clear();
    updater.setAddress("/spots/cloud/color/3"); updater.addFloatArg(spotCloud1.getRed());
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/spots/cloud/color/2"); updater.addFloatArg(spotCloud1.getGreen());
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/spots/cloud/color/1"); updater.addFloatArg(spotCloud1.getBlue());
    ipadSender.sendMessage(updater);
    
    
    updater.clear();
    updater.setAddress("/spots/cloud/brightness"); updater.addFloatArg(spotCloud1.getStaticBrightness());
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/spots/cloud/fadetime"); updater.addFloatArg(spotCloud1.getFadeTime());
    ipadSender.sendMessage(updater);
    
    
    updater.clear();
    updater.setAddress("/led/interaction/pulse_bright_min"); updater.addFloatArg(ledRingInteraction.getPulseMin());
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/led/interaction/pulse_bright_max"); updater.addFloatArg(ledRingInteraction.getPulseMax());
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/led/interaction/pulse_speed"); updater.addFloatArg(ledRingInteraction.getPulseSpeed());
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/led/interaction/static_brightness"); updater.addFloatArg(ledRingInteraction.getStaticBrightness());
    ipadSender.sendMessage(updater);
    
    
    updater.clear();
    updater.setAddress("/spots/interaction/pulse/brightness/min"); updater.addFloatArg(spotInteraction1.getPulseMin());
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/spots/interaction/pulse/brightness/max"); updater.addFloatArg(spotInteraction1.getPulseMax());
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/spots/interaction/brightness"); updater.addFloatArg(spotInteraction1.getStaticBrightness());
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/spots/interaction/fadetime"); updater.addFloatArg(spotInteraction1.getFadeTime());
    ipadSender.sendMessage(updater);
}


