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
    mesh_resolution_x = 5;
    mesh_resolution_y = 5;
    connected = false;
    near_threshold = 0;
    far_threshold = 2000;
    depth_threshold_max = 55;
    
    offset = ofVec3f(0, 0, -3000); // we assume that people are standing 3m in front of the kinect
    
    draw_contour = false;
    
    mesh_mode = mesh_mode_lines;
    
    grayImage.allocate(kinect->width, kinect->height);
	grayThreshNear.allocate(kinect->width, kinect->height);
	grayThreshFar.allocate(kinect->width, kinect->height);
    
}

void meshMan::update(){
    frame_new = false;
    
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
    
    // there is a new frame and we are connected
	if(kinect->isFrameNew()) {
        int w = 640;
        int h = 480;
        mesh.clear();

        frame_new = true;

        if(mesh_mode == mesh_mode_triangles){
            mesh.setMode(OF_PRIMITIVE_TRIANGLES); 
            for(int y = 0; y < h - mesh_resolution_y; y += mesh_resolution_y) {
                for(int x = 0; x < w - mesh_resolution_x; x += mesh_resolution_x) {
                    float distance = kinect->getDistanceAt(x, y);
                    if(distance > near_threshold && distance < far_threshold) {
                        ofVec3f current = kinect->getWorldCoordinateAt(x, y);
                        ofVec3f right = kinect->getWorldCoordinateAt(x + mesh_resolution_x, y);
                        ofVec3f below = kinect->getWorldCoordinateAt(x, y + mesh_resolution_y);
    //                    ofVec3f current = ofVec3f(x, y, kinect->getDistanceAt(x, y));
    //                    ofVec3f right = ofVec3f(x + mesh_resolution, y, kinect->getDistanceAt(x + mesh_resolution, y));
    //                    ofVec3f below = ofVec3f(x, y + mesh_resolution, kinect->getDistanceAt(x, y + mesh_resolution));

                        if(abs(current.distance(right)) < depth_threshold_max && abs(current.distance(below)) < depth_threshold_max){
                            // apply the offset and add to the mesh
                            mesh.addVertex(current + offset);
                            mesh.addVertex(right + offset);
                            mesh.addVertex(below + offset);
                        }
                    }
                }
            }
        }
        else if (mesh_mode == mesh_mode_quads){
            mesh.setMode(OF_PRIMITIVE_QUADS);
            for(int y = 0; y < h - mesh_resolution_y; y += mesh_resolution_y) {
                for(int x = 0; x < w - mesh_resolution_x; x += mesh_resolution_x) {
                    float distance = kinect->getDistanceAt(x, y);
                    if(distance > near_threshold && distance < far_threshold) {
                        ofVec3f current = kinect->getWorldCoordinateAt(x, y);
                        ofVec3f right = kinect->getWorldCoordinateAt(x + mesh_resolution_x, y);
                        ofVec3f below = kinect->getWorldCoordinateAt(x, y + mesh_resolution_y);
                        ofVec3f rightbelow = kinect->getWorldCoordinateAt(x + mesh_resolution_x, y + mesh_resolution_y);
                        
                        if(abs(current.distance(right)) < depth_threshold_max && abs(current.distance(below)) < depth_threshold_max && abs(current.distance(rightbelow)) < depth_threshold_max){
                            // apply the offset and add to the mesh
                            mesh.addVertex(current + offset);
                            mesh.addVertex(right + offset);
                            mesh.addVertex(rightbelow + offset);
                            mesh.addVertex(below + offset);
                        }
                    }
                }
            }
        }
        else if (mesh_mode == mesh_mode_lines){
            mesh.setMode(OF_PRIMITIVE_LINES);
            
            // make the horizontal lines
            for(int y = 0; y < h - mesh_resolution_y; y+= mesh_resolution_y) {
                for(int x = 0; x < w - mesh_resolution_x; x+=mesh_resolution_x) {
                    ofVec3f current = kinect->getWorldCoordinateAt(x, y);
                    
                    if(current.z > near_threshold && current.z < far_threshold){
                        ofVec3f right = kinect->getWorldCoordinateAt(x + mesh_resolution_x, y);
                        if(abs(current.z - right.z) < depth_threshold_max) {
                            // apply the offset and add to the mesh
                            mesh.addVertex(current + offset);
                            mesh.addVertex(right + offset);
                        }
                    }
                }
            }
        }
        
        if(transceiver->isConnected())
            transceiver->send(&mesh);
        
        
        
        
        
        
        // TODO: reactivate this autocenter stuff
        //    /* get the center of the pointcloud */
        //    if(autocenter){
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

void meshMan::drawContour(){
    // get the contour
    grayImage.setFromPixels(kinect->getDepthPixels(), kinect->width, kinect->height);
    
    grayThreshNear = grayImage;
    grayThreshFar = grayImage;
    grayThreshNear.threshold(cv_near_threshold, true);
    grayThreshFar.threshold(cv_far_threshold);
    
    cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
    
    int numContours = contourFinder.findContours(grayImage, 10, (kinect->width*kinect->height)/2, 20, true);
    
    // translate the contour into a 3d polyline
    for(int i=0; i < numContours; i++){
        ofMesh lines;
        lines.setMode(OF_PRIMITIVE_LINES);
        
        // add vertices from the current blob into one polyline
        
        for(int point = 0; point < contourFinder.blobs[i].pts.size()-1; point++){
            lines.addVertex(kinect->getWorldCoordinateAt(contourFinder.blobs[i].pts[point].x, contourFinder.blobs[i].pts[point].y) + offset);
            lines.addVertex(kinect->getWorldCoordinateAt(contourFinder.blobs[i].pts[point+1].x, contourFinder.blobs[i].pts[point+1].y) + offset);
        }

        lines.drawWireframe();                          // draw the contour
    }
}

void meshMan::drawDebug(){
    grayImage.draw(0, 0, 320, 240);
    grayThreshFar.draw(330, 0, 320, 240);
    grayThreshNear.draw(0, 250, 320, 240);
    contourFinder.draw(330, 250, 320, 240);
}

void meshMan::updateFromNetwork(){    
    if(transceiver->receive(&temp_mesh)){
        mesh = ofMesh(temp_mesh);
        frame_new = true;
    }


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
    mesh.drawWireframe();
    if(draw_contour) drawContour();
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
