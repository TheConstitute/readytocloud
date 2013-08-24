//
//  meshMan.cpp
//  readytocloud
//
//  Created by Julian Adenauer on 21.08.13.
//

#include "meshMan.h"

//--------------------------------------------------------------
meshMan::meshMan(){
    // initialize the variables
    mesh_resolution = 5;
    connected = false;
    near_threshold = 0;
    far_threshold = 2000;
    depth_threshold_max = 55;
    depth_threshold_min = 10;
    
}

void meshMan::update(){
    switch(mode){
        case mode_kinect:
            updateFromKinect();
            break;
        case mode_network:
            updateFromNetwork();
            break;
    }
}

void meshMan::setup(meshTransceiver* transceiver, ofxKinect* kinect){
    this->transceiver = transceiver;
    
    if(kinect == NULL)
        mode = mode_network;
    else {
        this->kinect = kinect;
        mode = mode_kinect;
    }
}

//--------------------------------------------------------------
void meshMan::updateFromKinect(){
    kinect->update();
    
    bool render_quads = true;
    

    // there is a new frame and we are connected
	if(kinect->isFrameNew()) {
        int w = 640;
        int h = 480;
        mesh.clear();


        if(!render_quads){
            mesh.setMode(OF_PRIMITIVE_TRIANGLES);
            for(int y = 0; y < h; y += mesh_resolution) {
                for(int x = 0; x < w; x += mesh_resolution) {
                    float distance = kinect->getDistanceAt(x, y);
                    if(distance > near_threshold && distance < far_threshold) {
                        ofVec3f current = kinect->getWorldCoordinateAt(x, y);
                        ofVec3f right = kinect->getWorldCoordinateAt(x + mesh_resolution, y);
                        ofVec3f below = kinect->getWorldCoordinateAt(x, y + mesh_resolution);
    //                    ofVec3f current = ofVec3f(x, y, kinect->getDistanceAt(x, y));
    //                    ofVec3f right = ofVec3f(x + mesh_resolution, y, kinect->getDistanceAt(x + mesh_resolution, y));
    //                    ofVec3f below = ofVec3f(x, y + mesh_resolution, kinect->getDistanceAt(x, y + mesh_resolution));

                        if(abs(current.distance(right)) < depth_threshold_max && abs(current.distance(below)) < depth_threshold_max){
                            mesh.addVertex(current);
                            mesh.addVertex(right);
                            mesh.addVertex(below);
                        }
                    }
                }
            }
        }
        else {
            mesh.setMode(OF_PRIMITIVE_QUADS);
            for(int y = 0; y < h; y += mesh_resolution) {
                for(int x = 0; x < w; x += mesh_resolution) {
                    float distance = kinect->getDistanceAt(x, y);
                    if(distance > near_threshold && distance < far_threshold) {
                        ofVec3f current = kinect->getWorldCoordinateAt(x, y);
                        ofVec3f right = kinect->getWorldCoordinateAt(x + mesh_resolution, y);
                        ofVec3f below = kinect->getWorldCoordinateAt(x, y + mesh_resolution);
                        ofVec3f rightbelow = kinect->getWorldCoordinateAt(x + mesh_resolution, y + mesh_resolution);
                        
                        if(abs(current.distance(right)) < depth_threshold_max && abs(current.distance(below)) < depth_threshold_max && abs(current.distance(rightbelow)) < depth_threshold_max){
                            mesh.addVertex(current);
                            mesh.addVertex(right);
                            mesh.addVertex(rightbelow);
                            mesh.addVertex(below);
                        }
                    }
                }
            }
        }
        
        transceiver->send(&mesh);
        
        // TODO: reactivate this autocenter stuff
        //    /* get the center of the pointcloud */
        //    if(local_autocenter){
        //        ofVec3f sumOfAllPoints(0,0,0);
        //        vector<ofVec3f> vertices = allUserPoints.getVertices();
        //        for(int i = 0; i < vertices.size(); i++){
        //            sumOfAllPoints += vertices[i];
        //        }
        //        if(vertices.size() > 0)
        //            mesh_local_center = sumOfAllPoints / vertices.size();
        //    }
        //
        //    // calculate the center of the remote mesh
        //    if(remote_autocenter){
        //        ofMesh allUserPoints;
        //
        //        // put all vertices in one mesh
        //        for(int i = 0; i< NUM_MAX_USERS; i++){ allUserPoints.addVertices(remoteMeshes[i].getVertices()); }
        //
        //        // get the center
        //        ofVec3f sumOfAllPoints(0,0,0);
        //        vector<ofVec3f> vertices = allUserPoints.getVertices();
        //        for(int i = 0; i < vertices.size(); i++){
        //            sumOfAllPoints += vertices[i];
        //        }
        //        if(vertices.size() > 0)
        //            mesh_remote_center = sumOfAllPoints / vertices.size();
        //    }
    }
}

void meshMan::updateFromNetwork(){
    transceiver->receive(&mesh);

    // TODO: redo this
//    // get the data from the clients
//    if(tcpServer.getNumClients() > 0) receiveTCP();
//    
//    // try to setup the connection if it is broken every 5 seconds
//    if(!connected && (ofGetElapsedTimef() - lastConnectionCheck) > 5.0) {
//        // versuch: tcpserver zurücksetzen
//        // if(tcpServer. getNumClients()>0) {
//        // disconnect the clients
//        // for(int i=0; i<tcpServer.getNumClients(); i++) {
//        //tcpServer.disconnectClient(i);
//        // ofLog()<< "server disconnected client " << ofToString(i);
//        //}
//        //}
//        connected = tcpClient.setup(SERVER_IP, REMOTE_SERVER_PORT);
//        if(connected) ofLog() << "client is connected to server " << tcpClient.getIP() << ":" << tcpClient.getPort();
//        lastConnectionCheck = ofGetElapsedTimef();
//    }
//    else if(!tcpClient.isConnected()){ connected = false; }
}

//--------------------------------------------------------------
void meshMan::draw(){
    ofPushMatrix();
    
    ofScale(1, -1, -1);
    
    // apply centering
    ofTranslate(-center.x, -center.y, -center.z);
    
    // draw the local mesh
    mesh.drawWireframe();
    
    ofPopMatrix();
}

//--------------------------------------------------------------
void meshMan::addMeshVertex(float x, float y, float z){
    ofVec3f v;
    v.x = x;
    v.y = y;
    v.z = z;
}

//--------------------------------------------------------------
void meshMan::refreshRemoteMesh(){
//    remoteMeshes[user] = ofMesh(remoteMeshesTemp[user]);
//    remoteMeshesTemp[user].clear();
}
