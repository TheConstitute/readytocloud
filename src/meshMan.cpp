//
//  meshMan.cpp
//  readytocloud
//
//  Created by Julian Adenauer on 21.08.13.
//
//

#include "meshMan.h"

meshMan::meshMan(){
    mesh_resolution = 5;
}

void meshMan::updateFromKinect(ofxKinect *kinect){
    // there is a new frame and we are connected
	if(kinect->isFrameNew()) {
        int w = 640;
        int h = 480;
        mesh.clear();
        
        for(int y = 0; y < h; y += mesh_resolution) {
            for(int x = 0; x < w; x += mesh_resolution) {
                float distance = kinect->getDistanceAt(x, y);
                if(distance > near_threshold && distance < far_threshold) {
                    ofVec3f current = kinect->getWorldCoordinateAt(x, y);
                    ofVec3f right = kinect->getWorldCoordinateAt(x + mesh_resolution, y);
                    ofVec3f below = kinect->getWorldCoordinateAt(x, y + mesh_resolution);
                    
                    if(abs(current.distance(right)) < depth_threshold && abs(current.distance(below)) < depth_threshold){
                        mesh.addVertex(current);
                        mesh.addVertex(right);
                        mesh.addVertex(below);
                    }
                }
            }
        }
        
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

void meshMan::draw(){
    // draw local user(s)
    ofPushMatrix();
    ofScale(1, -1, -1);
    
    // apply centering
    ofTranslate(-center.x, -center.y, -center.z);
    
    // draw the local mesh
    mesh.draw();
    ofPopMatrix();
}