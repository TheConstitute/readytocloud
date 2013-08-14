#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofSetFrameRate(25);
    ofLogLevel(OF_LOG_WARNING);
    
    //changeScreenRes(1024, 768);
    
    bool playback = false;
    string playbackFile = "test.oni";
    
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
            else if ( argument == "SEND_GHOST" ) { if(value=="1") send_ghost = true; else send_ghost = false; }
            else if ( argument == "PORT_OSC_CONTROL_SEND" ) { PORT_OSC_CONTROL_SEND = ofToInt(value); }
            else if ( argument == "PORT_OSC_CONTROL_RECEIVE" ) { PORT_OSC_CONTROL_RECEIVE = ofToInt(value); }
            else if ( argument == "IPAD_IP" ) { IPAD_IP = value; }
            else if ( argument == "PLAYBACK" ) { if(value=="1") playback = true; else playback = false; }
            else if ( argument == "PLAYBACK_FILE" ) { playbackFile = value; }
            else if ( argument == "ENTTEC_PORT" ) { ENTTEC_PORT = value; }

            else if ( argument == "MESH_SCALE" ) { mesh_scale_local = ofToFloat(value); mesh_scale_remote = ofToFloat(value); }
            else if ( argument == "MESH_RESOLUTION" ) { meshResolution_local = ofToFloat(value); meshResolution_remote = ofToFloat(value); }
            else if ( argument == "DEPTH_THRESHOLD" ) { depthThreshold = ofToFloat(value); }
            else if ( argument == "MIRROR" ) { if(value=="1") mirror = true; else mirror = false; }
            else if ( argument == "ACTIVATE_NETWORK" ) { if(value=="1") activate_network = true; else activate_network = false; }
            else if ( argument == "SHOW_CLOUD" ) { if(value=="1") b_clouds = true;  else b_clouds = false; }

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
        
    /* SETUP NETWORK */
    if(activate_network){
        if(tcpServer.setup(LOCAL_SERVER_PORT))
            ofLog() << "server is set up on port " << ofToString(LOCAL_SERVER_PORT);
        else "server setup failed";
        
        tcpServer.setMessageDelimiter("\n");
        
        ofLog() << "trying to establish a connection to the remote server: " << ofToString(SERVER_IP) << ofToString(REMOTE_SERVER_PORT);
        connected = tcpClient.setup(SERVER_IP, REMOTE_SERVER_PORT);
        tcpClient.setMessageDelimiter("\n");
        
        if(connected)
            ofLog() << "client is connected to server " << tcpClient.getIP() << ":" << tcpClient.getPort();
        
        lastConnectionCheck = ofGetElapsedTimef();
    }
	
    /* KINECT SETUP */
    // TODO: turn off the stuff we don't need
    kinect.setup();
    
    if(playback) kinect.startPlayer(playbackFile);

    kinect.addImageGenerator();
    kinect.addDepthGenerator();
    kinect.setRegister(true);
    kinect.setMirror(mirror);
    kinect.addUserGenerator();
    kinect.setMaxNumUsers(NUM_MAX_USERS);

    kinect.start();

    kinect.setUseMaskTextureAllUsers(true); // this turns on mask pixels internally AND creates mask textures efficiently
    kinect.setUseMaskPixelsAllUsers(true);
    kinect.setUsePointCloudsAllUsers(true);
    kinect.setPointCloudDrawSizeAllUsers(2); // size of each 'point' in the point cloud
    kinect.setPointCloudResolutionAllUsers(meshResolution_local); // resolution of the mesh created for the point cloud
    kinect.setUseDepthRawPixels(true);  // use the raw pixels otherwise we get strange distortions

    //kinect.startRecording(ofToDataPath("test2.oni"));
    
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
    
    clouds.setPixelFormat(OF_PIXELS_RGBA);
	clouds.loadMovie("cloud.mov");
    clouds.setLoopState(OF_LOOP_NORMAL);
    
    if(b_clouds) clouds.play();
    
    /* OSC SETUP */
    oscReceiver.setup(PORT_OSC_CONTROL_RECEIVE);    
    oscSender.setup(IPAD_IP, PORT_OSC_CONTROL_SEND);
    
    // push all settings to the ipad
    renderModeChanged = true;
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
    ofHideCursor();
    fboLocal.allocate(ofGetWidth(), ofGetHeight());
    fboRemote.allocate(ofGetWidth(), ofGetHeight());
    
    for(int i = 0; i<NUM_MAX_USERS; i++){
        remoteMeshes[i] = ofMesh();
        remoteMeshesTemp[i] = ofMesh();
    }
    
    
    cvImage.allocate(640, 480);
    cv_threshold1.allocate(640, 480);
    cv_threshold2.allocate(640, 480);
    
}


//--------------------------------------------------------------
void testApp::update() {
    // update the network stuff
    if(activate_network){
        // get the data from the clients
        if(tcpServer.getNumClients() > 0) receiveTCP();
        
        // try to setup the connection if it is broken every 5 seconds
        if(!connected && (ofGetElapsedTimef() - lastConnectionCheck) > 5) {
//            // versuch: tcpserver zurücksetzen
//            if(tcpServer. getNumClients()>0) {
//                // disconnect the clients
//                for(int i=0; i<tcpServer.getNumClients(); i++) { tcpServer.disconnectClient(i); ofLog()<< "server disconnected client " << ofToString(i);}
//            }
            connected = tcpClient.setup(SERVER_IP, REMOTE_SERVER_PORT);
            if(connected) ofLog() << "client is connected to server " << tcpClient.getIP() << ":" << tcpClient.getPort();
            lastConnectionCheck = ofGetElapsedTimef();
        }
        else if(!tcpClient.isConnected()){ connected = false; }
    }
    
    if(resetUsers){
        for(int i=0; i<kinect.getNumTrackedUsers(); i++){
            kinect.resetUserTracking(i);
        }
        resetUsers = false;
    }
    
    kinect.update();
    kinect.setPointCloudResolutionAllUsers(meshResolution_local);
    
    oscUpdate();
    
    wireframeUpdate();
    
    if(render_mode_local == RENDER_MODE_GHOST) ghostUpdate();
    
    /* UPDATE VIDEOS */
    overlay_in_local.update();
    overlay_out_local.update();
    overlay_in_remote.update();
    overlay_out_remote.update();
    clouds.update();
    
    /* update DMX */
    updateScene();
    dmxUpdate();
}

//--------------------------------------------------------------
void testApp::draw() {
    ofPushMatrix();
    
    ofBackground(0, 0, 0);
	
    fboLocal.begin();
        ofPushStyle();
            ofClear(0,0,0, 0);
            ofSetColor(colorCharacter_local, alpha_local);
            camera.setGlobalPosition(0, 0, mesh_scale_local);
            camera.begin();
    
            if(!firstFrame_local){
                if(render_mode_local == RENDER_MODE_WIREFRAME){ drawWireframeLocal(); }
            }
    
            camera.end();
            if(render_mode_local == RENDER_MODE_SKELETON) drawSkeletonLocal();
            if(render_mode_local == RENDER_MODE_GHOST){ drawGhostLocal(); }
    
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
    
    // eigentlich ist der server relevant, aber der scheint nicht wirklich zu wissen, wie viele clients mit ihm verbunden sind (oder es wird nur langsam geupdated). deshalb gehe ich den Weg über den Client. Wenn der nicht verbunden ist, ist der Server auch nicht verbunden.
    if(tcpClient.isConnected()){
        fboRemote.begin();
            ofPushStyle();
                ofClear(0,0,0, 0);
                ofSetColor(colorCharacter_remote, alpha_remote);

                camera.setGlobalPosition(0, 0, mesh_scale_remote);
                camera.begin();
        
                if(!firstFrame_remote){
                    if(render_mode_remote == RENDER_MODE_WIREFRAME){ drawWireframeRemote(); }
                }
        
                camera.end();
                if(render_mode_remote == RENDER_MODE_SKELETON) drawSkeletonRemote();
                if(render_mode_remote == RENDER_MODE_GHOST){ drawGhostRemote(); }
        
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
        if(tcpClient.isConnected()) fboRemote.draw(xCorrection_remote, -yCorrection_remote);
    ofDisableAlphaBlending();

//    if(!firstFrame){
//
//        if(render_mode_remote == RENDER_MODE_GHOST){ drawGhostRemote(); }
//    }
//
//
//if(render_mode_remote == RENDER_MODE_WIREFRAME){ drawWireframeRemote(); }


    /* DRAW VIDEO LAYERS */
    
    if(b_clouds)
    {
        ofPushStyle();
            ofSetColor(spotCloud1.getColor(), alphaClouds);
            ofEnableAlphaBlending();
            clouds.draw(0,0, ofGetWidth(), ofGetHeight());
            ofDisableAlphaBlending();
        ofPopStyle();
    }
    
    ofPopMatrix();

    if(showDebug)
    {
        cvImage.draw(0, 0, 320, 240);
        cv_threshold1.draw(330, 0, 320, 240);
        cv_threshold2.draw(660, 0, 320, 240);
        contourFinder.draw();
    }
}

//--------------------------------------------------------------
void testApp::wireframeUpdate(){
    
    userMeshes.clear();
    
    ofMesh allUserPoints;
    
    // go through all users and add their wireframes to list
    for (int u = 0; u < kinect.getNumTrackedUsers(); u++) {
        ofxOpenNIUser & user = kinect.getTrackedUser(u);
        ofMesh userPoints = user.getPointCloud();
        ofMesh userWireframe;
        getTriangleMesh(&userPoints, &userWireframe, depthThreshold);
        allUserPoints.addVertices(userWireframe.getVertices());
        
        //        ofPoint headWorldPos = user.getJoint(JOINT_HEAD).getWorldPosition();
        //        headWorldPos.y += 100;
        //        userWireframe.addVertex(ofVec3f(headWorldPos.x, headWorldPos.y, headWorldPos.z));
        //        userWireframe.addVertex(ofVec3f(headWorldPos.x, headWorldPos.y+100, headWorldPos.z));
        //        userWireframe.addVertex(ofVec3f(headWorldPos.x, headWorldPos.y, headWorldPos.z+10));
        
        userMeshes.push_back(userWireframe);
        
        // send this thing to the remote computer
        if(activate_network) sendMeshTCP(&userWireframe, u);
    }
    
    /* get the center of the pointcloud */
    if(local_autocenter){
        ofVec3f sumOfAllPoints(0,0,0);
        vector<ofVec3f> vertices = allUserPoints.getVertices();
        for(int i = 0; i < vertices.size(); i++){
            sumOfAllPoints += vertices[i];
        }
        if(vertices.size() > 0)
            mesh_local_center = sumOfAllPoints / vertices.size();
    }
    
    // calculate the center of the remote mesh
    if(render_mode_remote == RENDER_MODE_WIREFRAME && remote_autocenter){
        ofMesh allUserPoints;
        
        // put all vertices in one mesh
        for(int i = 0; i< NUM_MAX_USERS; i++){ allUserPoints.addVertices(remoteMeshes[i].getVertices()); }
        
        // get the center
        ofVec3f sumOfAllPoints(0,0,0);
        vector<ofVec3f> vertices = allUserPoints.getVertices();
        for(int i = 0; i < vertices.size(); i++){
            sumOfAllPoints += vertices[i];
        }
        if(vertices.size() > 0)
            mesh_remote_center = sumOfAllPoints / vertices.size();
    }

}

//--------------------------------------------------------------
void testApp::drawWireframeLocal(){
    // draw local user(s)
    ofPushMatrix();
    
        // wenn man auf den fbo malt, muss y auch gespiegelt werden
        ofScale(1, -1, -1);
        
        // apply z-scaling (to increase depth-effect
        ofScale(1, 1, zScaling);
        
        // apply centering
        ofTranslate(-mesh_local_center.x, -mesh_local_center.y, -mesh_local_center.z);
        
        ofPushStyle();
    
        // draw the local meshes
        if(localMesh) {
            for(int i=0; i < userMeshes.size(); i++){
                if(userMeshes[i].getNumVertices() > 10){ userMeshes[i].drawWireframe(); }
            }
        }
        
        ofPopStyle();
    
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::drawWireframeRemote(){
    ofPushMatrix();
    
    // wenn man auf den fbo malt, muss y auch gespiegelt werden
    ofScale(1, -1, -1);
    
    ofTranslate(-mesh_remote_center.x, -mesh_remote_center.y, -mesh_remote_center.z);
    for(int i=0; i < numTrackedUsers_remote; i++){
        if(remoteMeshes[i].getNumVertices() > 10)
            remoteMeshes[i].drawWireframe();
    }
    ofPopMatrix();

}

//--------------------------------------------------------------
void testApp::drawGhostLocal(){
    if(numContours >0){
        // local ghosts
        ofPushMatrix();
        
        // scale the ghosts
        ofScale(ghosts_local_scale, ghosts_local_scale);

    //    if(_("mirror")) ofScale(-1, 1);
        // draw all local ghosts
    //    for(int i = 0; i<kinect.getNumTrackedUsers(); i++)
    //    {
            ghosts_local[0].setColor(ofColor(colorCharacter_local.r, colorCharacter_local.g, colorCharacter_local.b, alpha_local));
            
            // prevent division by zero and only draw, when there is enough to draw
            if(ghosts_local_scale != 0 && ghosts_local[0].getTessellation().getNumVertices() > 30){
                ghosts_local[0].draw(
                                 ((ofGetWidth()/2)/ghosts_local_scale) - ghosts_local_center.x,
                                 ((ofGetHeight()/2)/ghosts_local_scale) - ghosts_local_center.y);
            }
    //    }

        
        ofPopMatrix();
    }
}

//--------------------------------------------------------------
void testApp::drawGhostRemote(){
    ofPushMatrix();
    
    // scale the remote ghosts
    ofScale(ghosts_remote_scale, ghosts_remote_scale);

    // draw all remote ghosts
    for(int i = 0; i<NUM_MAX_USERS; i++)
    {
        ghosts_remote[i].setColor(colorCharacter_remote);
        
        // prevent division by zero and only draw, when there is enough to draw
        if(ghosts_remote_scale != 0 && ghosts_local[i].getTessellation().getNumVertices() > 30){
            ghosts_remote[i].draw(
                              ((ofGetWidth()/2)/ghosts_remote_scale) - ghosts_remote_center.x,
                              ((ofGetHeight()/2)/ghosts_remote_scale) - ghosts_remote_center.y);
        }
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::drawSkeletonLocal(){
    ofPushMatrix();
    
    ofPoint center = ofPoint(0,0);
    
    // get the center of all the ghosts
    for (int u = 0; u < kinect.getNumTrackedUsers(); u++) {
        ofxOpenNIUser & user = kinect.getTrackedUser(u);
        if(user.isSkeleton()){
            center += kinect.worldToProjective(user.getCenter());
        }
    }
    
    center/=kinect.getNumTrackedUsers();
//    ofLog() << ofToString(center);
    
    for (int u = 0; u < kinect.getNumTrackedUsers(); u++) {
        ofxOpenNIUser & user = kinect.getTrackedUser(u);
        ofScale(skeleton_scale_local, skeleton_scale_local);
//        if(_("mirror")) ofScale(-1, 1);
        if(skeleton_scale_local != 0){
            ofTranslate(
                    (ofGetWidth()/2)/skeleton_scale_local - center.x,
                     (ofGetHeight()/2)/skeleton_scale_local - center.y);
        }
        user.drawSkeleton();
        
        ofPoint head = user.getJoint(JOINT_HEAD).getProjectivePosition();
        ofCircle(head.x, head.y, 20);
    }
    
    ofPopMatrix();
}

void testApp::drawSkeletonRemote(){
    // TODO: Skeleton Remote
}

//--------------------------------------------------------------
void testApp::ghostUpdate()
{
        ofPixels& p = kinect.getDepthPixels();
        
        if(p.size() > 30) {
            ofImage img;
            img.setFromPixels(p.getPixels(), 640, 480, OF_IMAGE_COLOR_ALPHA);
            img.setImageType(OF_IMAGE_GRAYSCALE);
            
            
            cvImage.setFromPixels(img.getPixels(), 640, 480);
            
            
//            cvImage.blur(ghosts_blur_local);
            cv_threshold1.setFromPixels(img.getPixels(), 640, 480);
            cv_threshold2.setFromPixels(img.getPixels(), 640, 480);
            
            cv_threshold1.invert();
            cv_threshold1.threshold(threshold_far);
            
//            cvImage.invert();
//            cvImage.threshold(threshold);

            cv_threshold2.threshold(threshold_near);
            
            cvImage.absDiff(cv_threshold1, cv_threshold2);
            
            cvImage.invert();
            
            numContours = contourFinder.findContours(cvImage, (100*100), (640*480), 10, true);
            
            ofPolyline polyline;
            
//            for(int i=0; i < contourFinder.size(); i++){
                        for(int i=0; i < numContours; i++){
                // add vertices from all found countours into one polyline
                if(!contourFinder.blobs[i].hole)
                    polyline.addVertices(contourFinder.blobs[i].pts);
                
            }
            
            if(polyline.size() > 0){
                // die ungeglättete polyline verschicken
//                if(activate_network && send_ghost) sendGhostTCP(&polyline, u);

                // polyline glätten und in den lokalen ofPath des aktuellen users schreiben
                polylineToPath(&ghosts_local[0], &polyline, int(ghosts_smoothing_local));
            }
        }
        else {
            ghosts_local[0].clear();
        }

    ofVec3f sumOfAllPoints(0,0,0);
    
    if(local_autocenter){
        // update center of local ghosts
        // das ist eine ziemlich aufwendige variante, kann man mit Sicherheit auch wesentlich geschickter lösen
        ofMesh allGhosts;
        for(int i=0; i<NUM_MAX_USERS; i++){
            allGhosts.addVertices(ghosts_local[i].getTessellation().getVertices());
        }
        
        vector<ofVec3f> vertices = allGhosts.getVertices();
        for(int i = 0; i < vertices.size(); i++){
            sumOfAllPoints += vertices[i];
        }
        if(vertices.size() > 0)
            ghosts_local_center = sumOfAllPoints / vertices.size();
    }
    
    if(remote_autocenter){
        // updade center of remote ghosts
        ofMesh allRemoteGhosts;
        
        for(int i=0; i<numTrackedUsers_remote; i++){
            allRemoteGhosts.addVertices(ghosts_remote[i].getTessellation().getVertices());
        }
        
        sumOfAllPoints = ofVec3f(0,0,0);
        vector<ofVec3f> remote_vertices = allRemoteGhosts.getVertices();
        for(int i = 0; i < remote_vertices.size(); i++){
            sumOfAllPoints += remote_vertices[i];
        }
        if(remote_vertices.size() > 0)
            ghosts_remote_center = sumOfAllPoints / remote_vertices.size();
    }
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
void testApp::getTriangleMesh(ofMesh* userMesh, ofMesh* newMesh, int depthThreshold){
    
    newMesh->setMode(OF_PRIMITIVE_TRIANGLES);
    
    if(userMesh->getNumVertices() > 100)
    {
        // initialize the vectors
        vector<vector<float> > vectors;
        vectors.resize(640);
        for(int y= 0; y<640; y++){vectors[y].resize(480);}
        
        float res = meshResolution_local;

        ofShortPixels& depth = kinect.getDepthRawPixels();
        for(int i=0; i<userMesh->getNumVertices(); i++){
            ofVec3f v = userMesh->getVertex(i);
            vectors[v.x][v.y] = v.z;
        }
        
        for(int x=0; x<640 - res; x+=res){
            for(int y=0; y<480 - res; y+=res){
                if(vectors[x][y] != 0 && vectors[x+res][y] != 0 && vectors[x][y+res] != 0 && vectors[x+res][y+res] != 0){
                    if(abs(vectors[x][y] - vectors[x+res][y]) < depthThreshold && abs(vectors[x][y] - vectors[x][y+res]) < depthThreshold){
                        newMesh->addVertex(kinect.projectiveToWorld(ofPoint(x, y, vectors[x][y])));
                        newMesh->addVertex(kinect.projectiveToWorld(ofPoint(x+res, y, vectors[x+res][y])));
                        newMesh->addVertex(kinect.projectiveToWorld(ofPoint(x, y+res, vectors[x][y+res])));
                    }
                }
            }
        }
    }
}


//--------------------------------------------------------------
void testApp::exit() {
    tcpServer.close();
    tcpClient.close();
    ofLog() << "server & client stopped";
    kinect.stop();
    ofLog() << "stopped tracking";

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
            
        case 'c':
            clouds.play();
            b_clouds = !b_clouds;
			break;
            
        case '+':
            nextState();
            break;
            
        case 'a':
            render_mode_local++;
            if(render_mode_local > RENDER_MODE_SKELETON) render_mode_local = RENDER_MODE_WIREFRAME;
            renderModeChanged = true;
            break;
            
        case 'x':
            xCorrection_local++;
            break;
        
        case '1':
            render_mode_local = 0;
            break;
        
        case '2':
            render_mode_local = 1;
            break;
        
        case '3':
            render_mode_local = 2;
            break;
        
        case 'u':
            resetUsers = true;
            break;
        case 'n':
            alphaClouds -= 5;
            if (alphaClouds < 0) alphaClouds = 0;
            break;
        case 'm':
            alphaClouds += 5;
            if (alphaClouds > 255) alphaClouds = 255;
            break;
            
        case 't':
            threshold_near--;
            ofLog() << "Threshold near: " << threshold_near;
            break;
            
        case 'z':
            threshold_near++;
            ofLog() << "Threshold near: " << threshold_near;
            break;
            
        case 'g':
            threshold_far--;
            ofLog() << "Threshold far: " << threshold_far;
            break;
            
        case 'h':
            threshold_far++;
            ofLog() << "Threshold far: " << threshold_far;
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
        
        // render mode
        if (m.getAddress() == "/renderMode/local/0") { render_mode_local = 0; renderModeChanged = true;}
        else if (m.getAddress() == "/renderMode/local/1") { render_mode_local = 1; renderModeChanged = true; }
        else if (m.getAddress() == "/renderMode/local/2") { render_mode_local = 2; renderModeChanged = true; }
        else if (m.getAddress() == "/renderMode/remote/0") { render_mode_remote = 0; renderModeChanged = true;}
        else if (m.getAddress() == "/renderMode/remote/1") { render_mode_remote = 1; renderModeChanged = true; }
        else if (m.getAddress() == "/renderMode/remote/2") { render_mode_remote = 2; renderModeChanged = true; }
        
        // light state
        else if (m.getAddress() == "/lightState/0") { dmx_state = 0; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/1") { dmx_state = 1; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/2") { dmx_state = 2; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/3") { dmx_state = 3; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/4") { dmx_state = 4; lightStateChanged = true; }
        else if (m.getAddress() == "/lightState/5") { dmx_state = 5; lightStateChanged = true; }
        
        // mesh settings
        else if (m.getAddress() == "/mesh/local/scale") { mesh_scale_local = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/mesh/local/resolution") { meshResolution_local = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/mesh/remote/scale") { mesh_scale_remote = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/mesh/remote/resolution") { meshResolution_remote = m.getArgAsFloat(0); }
        
        // centering
        else if (m.getAddress() == "/centering/local/correction") { yCorrection_local = m.getArgAsFloat(0); xCorrection_local = m.getArgAsFloat(1); }
        else if (m.getAddress() == "/centering/remote/correction") { yCorrection_remote = m.getArgAsFloat(0); xCorrection_remote = m.getArgAsFloat(1); }
        else if (m.getAddress() == "/centering/autolocal") { local_autocenter =  bool(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/centering/autoremote") { remote_autocenter = bool(m.getArgAsFloat(0)); }
        else if (m.getAddress() == "/mirror") { mirror = bool(m.getArgAsFloat(0)); }
        
        // ghost settings
        else if (m.getAddress() == "/ghost/local/scale") { ghosts_local_scale = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/ghost/local/blur") { ghosts_blur_local = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/ghost/local/smoothing") { ghosts_smoothing_local = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/ghost/remote/scale") { ghosts_remote_scale = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/ghost/remote/blur") { ghosts_blur_remote = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/ghost/remote/smoothing") { ghosts_smoothing_remote = m.getArgAsFloat(0); }
        
        // skeleton settings
        else if (m.getAddress() == "/skeleton/local/scale") { skeleton_scale_local = m.getArgAsFloat(0); }
        else if (m.getAddress() == "/skeleton/remote/scale") { skeleton_scale_remote = m.getArgAsFloat(0); }
        
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
    
    if(renderModeChanged){
        renderModeChanged = false;
        
        // render modes
        if(render_mode_local == 0) {
            updater.clear();
            updater.setAddress("/renderMode/local/0"); updater.addFloatArg(1);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/renderMode/local/1"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/renderMode/local/2"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
        }
        else if(render_mode_local == 1) {
            updater.clear();
            updater.setAddress("/renderMode/local/0"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/renderMode/local/1"); updater.addFloatArg(1);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/renderMode/local/2"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
        }
        else if(render_mode_local == 2) {
            updater.clear();
            updater.setAddress("/renderMode/local/0"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/renderMode/local/1"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/renderMode/local/2"); updater.addFloatArg(1);
            oscSender.sendMessage(updater);
        }
        
        // remote render modes
        if(render_mode_remote == 0) {
            updater.clear();
            updater.setAddress("/renderMode/remote/0"); updater.addFloatArg(1);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/renderMode/remote/1"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/renderMode/remote/2"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
        }
        else if(render_mode_remote == 1) {
            updater.clear();
            updater.setAddress("/renderMode/remote/0"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/renderMode/remote/1"); updater.addFloatArg(1);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/renderMode/remote/2"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
        }
        else if(render_mode_remote == 2) {
            updater.clear();
            updater.setAddress("/renderMode/remote/0"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/renderMode/remote/1"); updater.addFloatArg(0);
            oscSender.sendMessage(updater);
            
            updater.clear();
            updater.setAddress("/renderMode/remote/2"); updater.addFloatArg(1);
            oscSender.sendMessage(updater);
        }
    }
    
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
    
    updater.clear();
    updater.setAddress("/mesh/local/resolution"); updater.addFloatArg(meshResolution_local);
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
    
    // ghost settings
    updater.clear();
    updater.setAddress("/ghost/local/scale"); updater.addFloatArg(ghosts_local_scale);
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/ghost/local/blur"); updater.addFloatArg(ghosts_blur_local);
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/ghost/local/smoothing"); updater.addFloatArg(ghosts_smoothing_local);
    oscSender.sendMessage(updater);
    
    
    updater.clear();
    updater.setAddress("/ghost/remote/scale"); updater.addFloatArg(ghosts_remote_scale);
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/ghost/remote/blur"); updater.addFloatArg(ghosts_blur_remote);
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/ghost/remote/smoothing"); updater.addFloatArg(ghosts_smoothing_remote);
    oscSender.sendMessage(updater);
    
    // skeleton settings
    updater.clear();
    updater.setAddress("/skeleton/local/scale"); updater.addFloatArg(skeleton_scale_local);
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/skeleton/remote/scale"); updater.addFloatArg(skeleton_scale_remote);
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
    
    // num users
    updater.clear();
    updater.setAddress("/numUsers/local"); updater.addStringArg(ofToString(kinect.getNumTrackedUsers()));
    oscSender.sendMessage(updater);
    
    updater.clear();
    updater.setAddress("/numUsers/remote"); updater.addStringArg(ofToString(numTrackedUsers_remote)); 
    oscSender.sendMessage(updater);
    
    //connection
    updater.clear();
    updater.setAddress("/connection"); if(tcpClient.isConnected()) updater.addFloatArg(1); else updater.addFloatArg(0);
    oscSender.sendMessage(updater);
}


//--------------------------------------------------------------
void testApp::sendMeshTCP(const ofMesh* mesh, int user){
    // ich schicke jetzt ein controll-float mit. ein byte würde eigentlich auch reichen, aber ich habe gerade keine lust, mit memcpy rumzuspielen
    
    if(mesh != NULL){
        float data[4];
        for(int i = 0; i < mesh->getNumVertices(); i++)
        {
            data[0] = ADD_MESH_VERTEX_USER0 + user;
            ofVec3f vertex = mesh->getVertex(i);
            data[1] = vertex.x;
            data[2] = vertex.y;
            data[3] = vertex.z;
            // just send the raw data. not compatible with other operatingsystems, though. but that doesn't really matter in this case.
            if (connected && tcpClient.isConnected()) {
                try { tcpClient.sendRawBytes((const char*)data, 4 * sizeof(float));}
                catch (int e) { ofLog() << "caught an exception in sendMeshTCP: " << e; }
            }
        }
        
        // send clear message
        data[0] = CLEAR_MESH_USER0 + user;
        data[1] = kinect.getNumTrackedUsers();
        data[2] = 0;
        data[3] = 0;
        if (connected && tcpClient.isConnected()) {
            try { tcpClient.sendRawBytes((const char*)data, 4 * sizeof(float)); }
            catch (int e) { ofLog() << "caught an exception in sendMeshTCP: " << e; }
        }
    }
}

//--------------------------------------------------------------
void testApp::sendGhostTCP(ofPolyline* ghost_local_polyline, int user){
    float data[4];
    if(ghost_local_polyline != NULL){
        for(int i = 0; i < ghost_local_polyline->getVertices().size(); i++)
        {
            data[0] = ADD_GHOST_VERTEX_USER0 + user;
            ofVec3f vertex = ghost_local_polyline->getVertices()[i];
            data[1] = vertex.x;
            data[2] = vertex.y;
            data[3] = 0;    // der ghost ist 2D
            // just send the raw data. not compatible with other operatingsystems, though. but that doesn't really matter in this case.
            if (connected && tcpClient.isConnected()){
                try { tcpClient.sendRawBytes((const char*)data, 4 * sizeof(float)); }
                catch (int e) { ofLog() << "caught an exception in sendGhostTCP: " << e; }
            }
        }
        
        // send clear message
        data[0] = CLEAR_GHOST_USER0 + user;
        data[1] = kinect.getNumTrackedUsers();
        data[2] = 0;
        data[3] = 0;
        if (connected && tcpClient.isConnected()){
            try { tcpClient.sendRawBytes((const char*)data, 4 * sizeof(float)); }
            catch (int e) { ofLog() << "caught an exception in sendGhostTCP: " << e; }
        }
    }
}

//--------------------------------------------------------------
void testApp::receiveTCP(){
    for(int client = 0; client < tcpServer.getNumClients(); client++){
        float buf[4];
        try{
            while(tcpServer.receiveRawBytes(client, (char*) buf, 4 * sizeof(float)) == 4 * sizeof(float)){
//                clientTimeout = ofGetElapsedTimef();
                
                if(buf[0] == CLEAR_MESH_USER0){ refreshRemoteMesh(0); numTrackedUsers_remote = buf[1]; }
                else if (buf[0] == CLEAR_MESH_USER1){ refreshRemoteMesh(1); numTrackedUsers_remote = buf[1]; }
                else if (buf[0] == CLEAR_MESH_USER2){ refreshRemoteMesh(2); numTrackedUsers_remote = buf[1]; }
                else if (buf[0] == CLEAR_MESH_USER3){ refreshRemoteMesh(3); numTrackedUsers_remote = buf[1]; }
                else if (buf[0] == CLEAR_MESH_USER4){ refreshRemoteMesh(4); numTrackedUsers_remote = buf[1]; }
                else if (buf[0] == ADD_MESH_VERTEX_USER0) addMeshVertex(buf[1], buf[2], buf[3], 0);
                else if (buf[0] == ADD_MESH_VERTEX_USER1) addMeshVertex(buf[1], buf[2], buf[3], 1);
                else if (buf[0] == ADD_MESH_VERTEX_USER2) addMeshVertex(buf[1], buf[2], buf[3], 2);
                else if (buf[0] == ADD_MESH_VERTEX_USER3) addMeshVertex(buf[1], buf[2], buf[3], 3);
                else if (buf[0] == ADD_MESH_VERTEX_USER4) addMeshVertex(buf[1], buf[2], buf[3], 4);
//                else if (buf[0] == CLEAR_GHOSTS) { for(int i=0; i<NUM_MAX_USERS; i++) { ghosts_remote[i].clear(); ghosts_remote_polyline[i].clear(); }}
                else if (buf[0] == ADD_GHOST_VERTEX_USER0) addGhostVertex(buf[1], buf[2], 0);
                else if (buf[0] == ADD_GHOST_VERTEX_USER1) addGhostVertex(buf[1], buf[2], 1);
                else if (buf[0] == ADD_GHOST_VERTEX_USER2) addGhostVertex(buf[1], buf[2], 2);
                else if (buf[0] == ADD_GHOST_VERTEX_USER3) addGhostVertex(buf[1], buf[2], 3);
                else if (buf[0] == ADD_GHOST_VERTEX_USER4) addGhostVertex(buf[1], buf[2], 4);
                else if(buf[0] == CLEAR_GHOST_USER0){ refreshRemoteGhost(0); numTrackedUsers_remote = buf[1]; }
                else if (buf[0] == CLEAR_GHOST_USER1){ refreshRemoteGhost(1); numTrackedUsers_remote = buf[1];}
                else if (buf[0] == CLEAR_GHOST_USER2){ refreshRemoteGhost(2); numTrackedUsers_remote = buf[1];}
                else if (buf[0] == CLEAR_GHOST_USER3){ refreshRemoteGhost(3); numTrackedUsers_remote = buf[1];}
                else if (buf[0] == CLEAR_GHOST_USER4){ refreshRemoteGhost(4); numTrackedUsers_remote = buf[1];}
            }
        }
        catch (int e) { ofLog() << "caught an error while receiving data over TCP: " << e; }
    }
}

//--------------------------------------------------------------
void testApp::addMeshVertex(float x, float y, float z, int user){
    ofVec3f v;
    v.x = x;
    v.y = y;
    v.z = z;
    
    remoteMeshesTemp[user].setMode(OF_PRIMITIVE_TRIANGLES);
    remoteMeshesTemp[user].addVertex(v);
}

//--------------------------------------------------------------
void testApp::refreshRemoteMesh(int user){
    remoteMeshes[user] = ofMesh(remoteMeshesTemp[user]);
    remoteMeshesTemp[user].clear();
}

void testApp::refreshRemoteGhost(int user){
    // make a nice pathe out of the polyline
    for(int i=0; i<numTrackedUsers_remote; i++){
        polylineToPath(&ghosts_remote[i], &ghosts_remote_polyline[i], int(ghosts_smoothing_remote));
    }
}

//--------------------------------------------------------------
void testApp::addGhostVertex(float x, float y, int user){    
    ghosts_remote_polyline[user].addVertex(x, y);
}

//--------------------------------------------------------------
void testApp::polylineToPath(ofPath* path, ofPolyline* polyline, int smoothing){
    if(path != NULL && polyline != NULL){
        // clear the path before we assign new stuff to it
        path->clear();
        
        // if we have points on the polyline, convert them into the ofPath
        if(polyline->size()>0)
        {
            ofPolyline polyline_smooth = polyline->getSmoothed(ghosts_smoothing_local);
            
            for( int i = 0; i < polyline_smooth.getVertices().size(); i++) {
                if(i == 0) {
                    path->newSubPath();
                    path->moveTo(polyline_smooth.getVertices()[i] );
                } else {
                    path->lineTo(polyline_smooth.getVertices()[i] );
                }
            }
//            path->close();
//            path->simplify();
        }
    }
}

// könnte noch hilfreich sein!
void testApp::changeScreenRes(int h, int v){
    CGRect screenFrame = CGDisplayBounds(kCGDirectMainDisplay);
    CGSize screenSize  = screenFrame.size;
    printf("current resolution is %f %f\n", screenSize.width, screenSize.height);
    
    if(h != screenSize.width || v != screenSize.height){
        
        CGDirectDisplayID display = CGMainDisplayID(); // ID of main display
        CFDictionaryRef mode = CGDisplayBestModeForParameters(display, 32, h, v, NULL); // mode to switch to
        
        CGDisplayConfigRef config;
        
        if (CGBeginDisplayConfiguration(&config) == kCGErrorSuccess) {
            CGConfigureDisplayMode(config, display, mode);
            CGCompleteDisplayConfiguration(config, kCGConfigureForSession );
        }else{
//            printf(logStr, "Error changing resolution to %i, %i ",h,v);
            
        }
    } else{
        
//        printf(logStr, "screen resolution did not need to be changed. it is %i, %i ",h,v);
        
    }
    
}