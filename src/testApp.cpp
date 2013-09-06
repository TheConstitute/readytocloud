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
    
    interaction_parameters.setName("interaction parameters");
    interaction_parameters.add(mesh_interactor.distance.set("distance", 300, 0, 2000));
    interaction_parameters.add(mesh_interactor.resolution.set("resolution", 10, 1, 100));
    interaction_parameters.add(mesh_interactor.color.set("color", ofColor(0, 255, 0)));
    interaction_parameters.add(mesh_interactor.line_width.set("line width", 3.0, 1.0, 10.0));
    interaction_parameters.add(mesh_interactor.amplitude.set("amplitude", 100.0f, 1.0f, 500.0f));
    interaction_parameters.add(local_mesh.beam_flash_skip_vertices.set("beam vertice skip", 5, 0, 20));
    
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
    gui.add(interaction_parameters);
    gui.add(network_parameters);
    gui.add(osc_parameters);
    gui.add(dmx_parameters);
    gui.add(light_parameters);
    gui.loadFromFile("settings.xml");
    
    // gui3d
    gui3d.setup("positioning", "positioning.xml", 800, 30);
    gui3d.add(local_mesh.offset.set("local mesh offset", ofVec3f(0, 0, -3000), ofVec3f(-5000,-5000,-5000), ofVec3f(5000,5000,5000)));
    gui3d.loadFromFile("positioning.xml");
    
    draw_gui3d = false;
    draw_gui = false;
    
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
    
    mesh_transceiver.setup(local_port, server_ip, remote_port);
    remote_mesh.setup(&mesh_transceiver);
    
    local_mesh.setup(&mesh_transceiver, &kinect);
    
    mesh_interactor.setup(&local_mesh, &remote_mesh);
    
    /* OSC SETUP */
    state_sender.setup(server_ip, state_sync_port_remote);
    state_receiver.setup(state_sync_port_local);
    ipadSender.setup(osc_ipad_ip, osc_port_send);
    ipadReceiver.setup(osc_port_receive);
    
    dmx_state.addListener(this, &testApp::stateChanged);
    
    use_easy_cam = false;
    draw_grid = false;
    
    // push all settings to the ipad
    oscUpdateAll();

//    ofHideCursor();
    
    fboLocal.allocate(ofGetWidth(), ofGetHeight());

    kinect.setCameraTiltAngle(kinect_angle);
    
    last_ipad_update = -100; // make sure that the ipad gets updaten on startup
    
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
    
    /* update DMX */
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

        camera.setGlobalPosition(0, camera_offset_y, local_mesh_scale);
        camera.setFov(fov);
        camera.setOrientation(ofVec3f(camera_angle, 0, 0));
        if(use_easy_cam)easyCam.begin();
        else camera.begin();

    if(draw_grid){
        ofDrawGrid(1000);
        ofPushStyle();
        ofSetColor(255, 0, 0);
        ofSetLineWidth(20);
        ofLine(-250, 0, -125, -250, 0, 125);
        ofLine(-500, 0, -125, -500, 0, 125);
        ofLine(-250, 0, -125, -500, 0, -125);
        ofLine(-250, 0, 125, -500, 0, 125);
        ofSetColor(0, 255, 0);
        ofLine(250, 0, -125, 250, 0, 125);
        ofLine(500, 0, -125, 500, 0, 125);
        ofLine(250, 0, -125, 500, 0, -125);
        ofLine(250, 0, 125, 500, 0, 125);
        
        ofPopStyle();
    }
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
    ofLog() << "entering state " << ofToString(dmx_state);
}

//--------------------------------------------------------------
void testApp::stateChanged(int &state){
    // inform the other installation about this change
    ofxOscMessage message;
    message.setAddress("/dmx_state/" + ofToString(state));
    state_sender.sendMessage(message);
        
    // update the scene with the changes
    updateScene();
    
}

//--------------------------------------------------------------
void testApp::updateScene(){        
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
            local_mesh.beamOut();
            remote_mesh.beamOut();
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
            local_mesh.beamOut();
            remote_mesh.beamOut();
            break;
        case 2:
            ledRingInteraction.pulseBrightness();
            spotInteraction1.pulseBrightness();
            spotInteraction2.pulseBrightness();
            spotCloud1.stopPulse();
            spotCloud2.stopPulse();
            spotCloud1.fadeOut();
            spotCloud2.fadeOut();
            local_mesh.beamOut();
            remote_mesh.beamOut();
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
            local_mesh.beamOut();
            remote_mesh.beamOut();
            break;
        case 4: // beam in local
//            fogMachine.fogOff();
            local_mesh.beamIn();
            
            // shut down cloud and interaction spots
            spotInteraction1.fadeOut();
            spotInteraction2.fadeOut();
            spotCloud1.fadeOut();
            spotCloud2.fadeOut();
            break;
        case 5: // beam in remote
            remote_mesh.beamIn();
            break;
        case 6: // beam out
            remote_mesh.beamOut();
            local_mesh.beamOut();
            break;
    }
    
    ofLog() << "switched light state to " << dmx_state;
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
void testApp::keyPressed (int key) {
	switch (key) {
        case 'i':
            local_mesh.beamIn();
			break;
        
        case 'o':
            local_mesh.beamOut();
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
    // see if there are state updates from the other installation
    while(state_receiver.hasWaitingMessages()){
        ofxOscMessage m;
        state_receiver.getNextMessage(&m);
        
        if      (m.getAddress() == "/dmx_state_remote/0") { dmx_state_remote = 0; }
        else if (m.getAddress() == "/dmx_state_remote/1") { dmx_state_remote = 1; }
        else if (m.getAddress() == "/dmx_state_remote/2") { dmx_state_remote = 2; }
        else if (m.getAddress() == "/dmx_state_remote/3") { dmx_state_remote = 3; }
        else if (m.getAddress() == "/dmx_state_remote/4") { dmx_state_remote = 4; }
        else if (m.getAddress() == "/dmx_state_remote/5") { dmx_state_remote = 5; }
        else if (m.getAddress() == "/dmx_state_remote/6") { dmx_state_remote = 6; }
    }
    
    // now work through the messages from the ipad and update the parameters
    while(ipadReceiver.hasWaitingMessages()){
        ofxOscMessage m;
        ipadReceiver.getNextMessage(&m);
                
        // light state
        if      (m.getAddress() == "/dmx_state/0") { dmx_state = 0; }
        else if (m.getAddress() == "/dmx_state/1") { dmx_state = 1; }
        else if (m.getAddress() == "/dmx_state/2") { dmx_state = 2; }
        else if (m.getAddress() == "/dmx_state/3") { dmx_state = 3; }
        else if (m.getAddress() == "/dmx_state/4") { dmx_state = 4; }
        else if (m.getAddress() == "/dmx_state/5") { dmx_state = 5; }
        else if (m.getAddress() == "/dmx_state/6") { dmx_state = 6; }
        
        // mesh settings
        else if (m.getAddress() == "/mesh/local/scale") { local_mesh_scale = m.getArgAsFloat(0); }
        
        // centering
        else if (m.getAddress() == "/centering/local/correction") { y_correction_local = m.getArgAsFloat(0); x_correction_local = m.getArgAsFloat(1); }
        
        // fog settings
        else if (m.getAddress() == "/fog/level") { fogMachine.setLevel(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/fog/state") { 
            if(m.getArgAsFloat(0) >= 0.9 && m.getArgAsFloat(0) <= 1.1){
                fogMachine.fogOn();
            }
            else { fogMachine.fogOff(); }
        }
        
        // character color settings
        else if (m.getAddress() == "/color/characters/white") { local_mesh.color = colors[0]; }
        else if (m.getAddress() == "/color/characters/red") { local_mesh.color = colors[1]; }
        else if (m.getAddress() == "/color/characters/green") { local_mesh.color = colors[2]; }
        else if (m.getAddress() == "/color/characters/blue") { local_mesh.color = colors[3]; }
        else if (m.getAddress() == "/color/characters/yellow") { local_mesh.color = colors[4]; }
        else if (m.getAddress() == "/color/characters/pink") { local_mesh.color = colors[5]; }
        else if (m.getAddress() == "/color/characters/violet") { local_mesh.color = colors[6]; }
        
        // britzel color settings
        else if (m.getAddress() == "/color/britzel/white") { local_mesh.beam_color = colors[0]; remote_mesh.beam_color = colors[0]; mesh_interactor.color = colors[0]; }
        else if (m.getAddress() == "/color/britzel/red") { local_mesh.beam_color = colors[1]; remote_mesh.beam_color = colors[1]; mesh_interactor.color = colors[1]; }
        else if (m.getAddress() == "/color/britzel/green") { local_mesh.beam_color = colors[2]; remote_mesh.beam_color = colors[2]; mesh_interactor.color = colors[2]; }
        else if (m.getAddress() == "/color/britzel/blue") { local_mesh.beam_color = colors[3]; remote_mesh.beam_color = colors[3]; mesh_interactor.color = colors[3]; }
        else if (m.getAddress() == "/color/britzel/yellow") { local_mesh.beam_color = colors[4]; remote_mesh.beam_color = colors[4]; mesh_interactor.color = colors[4]; }
        else if (m.getAddress() == "/color/britzel/pink") { local_mesh.beam_color = colors[5]; remote_mesh.beam_color = colors[5]; mesh_interactor.color = colors[5]; }
        else if (m.getAddress() == "/color/britzel/violet") { local_mesh.beam_color = colors[6]; remote_mesh.beam_color = colors[6]; mesh_interactor.color = colors[6]; }
        
        // cloud color settings
        else if (m.getAddress() == "/spots/cloud/color/white") { spotCloud1.setColor(colors[0]); spotCloud2.setColor(colors[0]); }
        else if (m.getAddress() == "/spots/cloud/color/red") { spotCloud1.setColor(colors[1]); spotCloud2.setColor(colors[1]); }
        else if (m.getAddress() == "/spots/cloud/color/green") { spotCloud1.setColor(colors[2]); spotCloud2.setColor(colors[2]); }
        else if (m.getAddress() == "/spots/cloud/color/blue") { spotCloud1.setColor(colors[3]); spotCloud2.setColor(colors[3]); }
        else if (m.getAddress() == "/spots/cloud/color/yellow") { spotCloud1.setColor(colors[4]); spotCloud2.setColor(colors[4]); }
        else if (m.getAddress() == "/spots/cloud/color/pink") { spotCloud1.setColor(colors[5]); spotCloud2.setColor(colors[5]); }
        else if (m.getAddress() == "/spots/cloud/color/violet") { spotCloud1.setColor(colors[6]); spotCloud2.setColor(colors[6]); }
        
        // mesh resolution
        else if (m.getAddress() == "/mesh_resolution/x") { local_mesh.mesh_resolution_x = m.getArgAsInt32(0); }
        else if (m.getAddress() == "/mesh_resolution/y") { local_mesh.mesh_resolution_y = m.getArgAsInt32(0); }
        else if (m.getAddress() == "/linewidth") { line_width = m.getArgAsFloat(0); }
        
        // camera settings
        else if (m.getAddress() == "/camera/offset") { camera_offset_y = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/camera/angle") { camera_angle = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/camera/fov") { fov = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/kinect/angle") { if(kinect_angle != m.getArgAsFloat(0)) {kinect_angle = m.getArgAsFloat(0); kinect.setCameraTiltAngle(kinect_angle);}}
        
        // britzel settings
        else if (m.getAddress() == "/britzel/distance") { mesh_interactor.distance = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/britzel/resolution") { mesh_interactor.resolution = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/flash/linewidth") { mesh_interactor.line_width = m.getArgAsFloat(0); local_mesh.beam_line_width = m.getArgAsFloat(0); remote_mesh.beam_line_width = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/britzel/amplitude") { mesh_interactor.amplitude = m.getArgAsFloat(0); }
        
        // mirror
        else if (m.getAddress() == "/mirror") { local_mesh.mirror = bool(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/grid") { draw_grid = bool(m.getArgAsFloat(0)); }
        
        // thresholds
        else if (m.getAddress() == "/threshold/near") { local_mesh.near_threshold = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/threshold/far") { local_mesh.far_threshold = m.getArgAsFloat(0); }
        
        // debug camera
        else if (m.getAddress() == "/debug_camera") { debug_camera = bool(m.getArgAsFloat(0)); }
        
        // x/y/z Position correction
        else if (m.getAddress() == "/centering/3d/x") { ofVec3f current = local_mesh.offset; current.x = m.getArgAsFloat(0); local_mesh.offset = current; }
        else if (m.getAddress() == "/centering/3d/y") { ofVec3f current = local_mesh.offset; current.y = m.getArgAsFloat(0); local_mesh.offset = current; }
        else if (m.getAddress() == "/centering/3d/z") { ofVec3f current = local_mesh.offset; current.z = m.getArgAsFloat(0); local_mesh.offset = current; }
        
        // load/save settings
        else if (m.getAddress() == "/settings/save") { gui.saveToFile("settings.xml"); gui3d.saveToFile("positioning.xml"); }
        else if (m.getAddress() == "/settings/load") { gui.loadFromFile("settings.xml"); gui3d.loadFromFile("positioning.xml"); }
        
        // mesh mode
        else if (m.getAddress() == "/mesh_mode/triangles") { local_mesh.mesh_mode = local_mesh.mesh_mode_triangles; }
        else if (m.getAddress() == "/mesh_mode/quads") { local_mesh.mesh_mode = local_mesh.mesh_mode_quads; }
        else if (m.getAddress() == "/mesh_mode/lines") { local_mesh.mesh_mode = local_mesh.mesh_mode_lines; }
        
        // light settings
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
    }
    
    
    // TODO: make this a bit more elegant!
    // do not push changes to the ipad too often!
    if(ofGetElapsedTimef() - last_ipad_update > 0.04){ // only 25 times per second
        last_ipad_update = ofGetElapsedTimef();
        oscUpdateAll();
    }

}

void testApp::oscUpdateAll(){
    /* update the interface on the ipad */
    ofxOscMessage updater;
    
    // update local states
    for(int i= 0; i < 7; i++)
    {
        updater.clear();
        updater.setAddress("/dmx_state/" + ofToString(i));
        if(dmx_state == i)
            updater.addFloatArg(1);
        else
            updater.addFloatArg(0);
        
        ipadSender.sendMessage(updater);
    }

    // update remote states
    for(int i= 0; i < 7; i++)
    {
        updater.clear();
        updater.setAddress("/dmx_state_remote/" + ofToString(i));
        if(dmx_state_remote == i)
            updater.addFloatArg(1);
        else
            updater.addFloatArg(0);
        
        ipadSender.sendMessage(updater);
    }

    
    // scale
    updater.clear();
    updater.setAddress("/mesh/local/scale"); updater.addFloatArg(local_mesh_scale);
    ipadSender.sendMessage(updater);
        
    // x/y correction
    updater.clear();
    updater.setAddress("/centering/local/correction"); updater.addFloatArg(y_correction_local); updater.addFloatArg(x_correction_local);
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
    
    // character color settings
    for(int i=0; i< 7; i++){
        updater.clear();
        updater.setAddress("/color/characters/" + color_names[i]);
        if(local_mesh.color == colors[i])
            updater.addFloatArg(1);
        else
            updater.addFloatArg(0);
        ipadSender.sendMessage(updater);
    }
    
    // britzel color settings
    for(int i=0; i< 7; i++){
        updater.clear();
        updater.setAddress("/color/britzel/" + color_names[i]);
        if(local_mesh.beam_color == colors[i])
            updater.addFloatArg(1);
        else
            updater.addFloatArg(0);
        ipadSender.sendMessage(updater);
    }
    
    // cloud color settings
    for(int i=0; i< 7; i++){
        updater.clear();
        updater.setAddress("/spots/cloud/color/" + color_names[i]);
        if(spotCloud1.getColor() == colors[i])
            updater.addFloatArg(1);
        else
            updater.addFloatArg(0);
        ipadSender.sendMessage(updater);
    }
    
    // connection led
    updater.clear();
    updater.setAddress("/connected");
    if(mesh_transceiver.isConnected())
        updater.addFloatArg(1);
    else
        updater.addFloatArg(0);
    ipadSender.sendMessage(updater);
    
    
    // mesh resolution
    updater.clear();
    updater.setAddress("/mesh_resolution/x"); updater.addIntArg(local_mesh.mesh_resolution_x);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/mesh_resolution/y"); updater.addIntArg(local_mesh.mesh_resolution_y);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/linewidth"); updater.addFloatArg(line_width);
    ipadSender.sendMessage(updater);
    
    
    // camera settings
    updater.clear();
    updater.setAddress("/camera/offset"); updater.addFloatArg(camera_offset_y);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/camera/angle"); updater.addFloatArg(camera_angle);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/camera/fov"); updater.addFloatArg(fov);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/kinect/angle"); updater.addFloatArg(kinect_angle);
    ipadSender.sendMessage(updater);
    
    
    // britzel settings
    updater.clear();
    updater.setAddress("/britzel/distance"); updater.addFloatArg(mesh_interactor.distance);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/britzel/resolution"); updater.addFloatArg(mesh_interactor.resolution);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/flash/linewidth"); updater.addFloatArg(mesh_interactor.line_width);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/britzel/amplitude"); updater.addFloatArg(mesh_interactor.amplitude);
    ipadSender.sendMessage(updater);

    
    // mirror
    updater.clear();
    updater.setAddress("/mirror");
    if(local_mesh.mirror)
        updater.addFloatArg(1);
    else
        updater.addFloatArg(0);
    ipadSender.sendMessage(updater);
    
    // grid
    updater.clear();
    updater.setAddress("/grid");
    if(draw_grid)
        updater.addFloatArg(1);
    else
        updater.addFloatArg(0);
    ipadSender.sendMessage(updater);
    
    // thresholds
    updater.clear();
    updater.setAddress("/threshold/near"); updater.addFloatArg(local_mesh.near_threshold);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/threshold/far"); updater.addFloatArg(local_mesh.far_threshold);
    ipadSender.sendMessage(updater);
    
    // debug camera
    updater.clear();
    updater.setAddress("/debug_camera");
    if(debug_camera)
        updater.addFloatArg(1);
    else
        updater.addFloatArg(0);
    ipadSender.sendMessage(updater);
    
    // x/y/z Position correction
    ofVec3f current_offset = local_mesh.offset;
    
    updater.clear();
    updater.setAddress("/centering/3d/x"); updater.addFloatArg(current_offset.x);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/centering/3d/y"); updater.addFloatArg(current_offset.y);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/centering/3d/z"); updater.addFloatArg(current_offset.z);
    ipadSender.sendMessage(updater);
    
    // mesh mode
    updater.clear();
    updater.setAddress("/mesh_mode/triangles");
    if(local_mesh.mesh_mode == local_mesh.mesh_mode_triangles)
        updater.addFloatArg(1);
    else
        updater.addFloatArg(0);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/mesh_mode/quads");
    if(local_mesh.mesh_mode == local_mesh.mesh_mode_quads)
        updater.addFloatArg(1);
    else
        updater.addFloatArg(0);
    ipadSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/mesh_mode/lines");
    if(local_mesh.mesh_mode == local_mesh.mesh_mode_lines)
        updater.addFloatArg(1);
    else
        updater.addFloatArg(0);
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


