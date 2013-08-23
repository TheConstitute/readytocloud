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
    depth_threshold = 55;
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

void meshMan::setupNetwork(int local_port, string remote_ip, int remote_port){
    if(tcp_server.setup(local_port))
        ofLog() << "server is set up on port " << ofToString(local_port);
    else "server setup failed";
    
    tcp_server.setMessageDelimiter("\n");
    
    ofLog() << "trying to establish a connection to the remote server: " << ofToString(remote_ip) << ofToString(remote_port);
    connected = tcp_client.setup(remote_ip, remote_port);
    tcp_client.setMessageDelimiter("\n");
    
    if(connected)
        ofLog() << "client is connected to server " << tcp_client.getIP() << ":" << tcp_client.getPort();
    
    mode = mode_network;
}

void meshMan::setupKinect(ofxKinect* kinect){
    this->kinect = kinect;
    mode = mode_kinect;
    
}

//--------------------------------------------------------------
void meshMan::updateFromKinect(){
    kinect->update();
    
    // there is a new frame and we are connected
	if(kinect->isFrameNew()) {
        int w = 640;
        int h = 480;
        mesh.clear();
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        
        for(int y = 0; y < h; y += mesh_resolution) {
            for(int x = 0; x < w; x += mesh_resolution) {
                float distance = kinect->getDistanceAt(x, y);
                if(distance > near_threshold && distance < far_threshold) {
//                    ofVec3f current = kinect->getWorldCoordinateAt(x, y);
//                    ofVec3f right = kinect->getWorldCoordinateAt(x + mesh_resolution, y);
//                    ofVec3f below = kinect->getWorldCoordinateAt(x, y + mesh_resolution);
                    ofVec3f current = ofVec3f(x, y, kinect->getDistanceAt(x, y));
                    ofVec3f right = ofVec3f(x + mesh_resolution, y, kinect->getDistanceAt(x + mesh_resolution, y));
                    ofVec3f below = ofVec3f(x, y + mesh_resolution, kinect->getDistanceAt(x, y + mesh_resolution));

                    if(abs(current.distance(right)) < depth_threshold && abs(current.distance(below)) < depth_threshold){
                        mesh.addVertex(current);
                        mesh.addVertex(right);
                        mesh.addVertex(below);
                    }
                }
            }
        }
        
        ofLog() << mesh.getNumVertices();
        
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
void meshMan::sendMeshTCP(){
    // ich schicke jetzt ein controll-float mit. ein byte würde eigentlich auch reichen, aber ich habe gerade keine lust, mit memcpy rumzuspielen

        float data[4];
        for(int i = 0; i < mesh.getNumVertices(); i++)
        {
            data[0] = 0; // TODO: I used to send the ID of the user here. But now with ofxKinect, this is not possible.
            ofVec3f vertex = mesh.getVertex(i);
            data[1] = vertex.x;
            data[2] = vertex.y;
            data[3] = vertex.z;
            // just send the raw data. not compatible with other operatingsystems, though. but that doesn't really matter in this case.
            if (connected && tcp_client.isConnected()) {
                try { tcp_client.sendRawBytes((const char*)data, 4 * sizeof(float));}
                catch (int e) { ofLog() << "caught an exception in sendMeshTCP: " << e; }
            }
        }
        
        // send clear message
        data[0] = 0;//CLEAR_MESH_USER0 + user; // TODO: review this
        data[1] = 0;//kinect.getNumTrackedUsers(); // TODO: review this part!
        data[2] = 0;
        data[3] = 0;
        if (connected && tcp_client.isConnected()) {
            try { tcp_client.sendRawBytes((const char*)data, 4 * sizeof(float)); }
            catch (int e) { ofLog() << "caught an exception in sendMeshTCP: " << e; }
        }
}

//--------------------------------------------------------------
void meshMan::receiveTCP(){
    for(int client = 0; client < tcp_server.getNumClients(); client++){
        float buf[4];
        try{
            // TODO: reimplement this!
            while(tcp_server.receiveRawBytes(client, (char*) buf, 4 * sizeof(float)) == 4 * sizeof(float)){
//                if(buf[0] == CLEAR_MESH_USER0){ refreshRemoteMesh(0); numTrackedUsers_remote = buf[1]; }
//                else if (buf[0] == CLEAR_MESH_USER1){ refreshRemoteMesh(1); numTrackedUsers_remote = buf[1]; }
//                else if (buf[0] == CLEAR_MESH_USER2){ refreshRemoteMesh(2); numTrackedUsers_remote = buf[1]; }
//                else if (buf[0] == CLEAR_MESH_USER3){ refreshRemoteMesh(3); numTrackedUsers_remote = buf[1]; }
//                else if (buf[0] == CLEAR_MESH_USER4){ refreshRemoteMesh(4); numTrackedUsers_remote = buf[1]; }
//                else if (buf[0] == ADD_MESH_VERTEX_USER0) addMeshVertex(buf[1], buf[2], buf[3], 0);
//                else if (buf[0] == ADD_MESH_VERTEX_USER1) addMeshVertex(buf[1], buf[2], buf[3], 1);
//                else if (buf[0] == ADD_MESH_VERTEX_USER2) addMeshVertex(buf[1], buf[2], buf[3], 2);
//                else if (buf[0] == ADD_MESH_VERTEX_USER3) addMeshVertex(buf[1], buf[2], buf[3], 3);
//                else if (buf[0] == ADD_MESH_VERTEX_USER4) addMeshVertex(buf[1], buf[2], buf[3], 4);
            }
        }
        catch (int e) { ofLog() << "caught an error while receiving data over TCP: " << e; }
    }
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
