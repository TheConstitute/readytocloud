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
    
    color = ofColor(255, 255, 255);
    
    mesh_mode = mesh_mode_lines;
    
    grayImage.allocate(kinect->width, kinect->height);
	grayThreshNear.allocate(kinect->width, kinect->height);
	grayThreshFar.allocate(kinect->width, kinect->height);
    
    beam_state = beamed_out;
}

//--------------------------------------------------------------
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

//--------------------------------------------------------------
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
    if(kinect->isConnected()){
        kinect->update();
        
        // clear the mesh if the user is beamed out 
        if(beam_state == beamed_out){
            mesh.clear();
        }
        
        // update the mesh if there is a new frame
        else if(kinect->isFrameNew()) {
            int w = 640;
            int h = 480;
            mesh.clear();

            frame_new = true;   // flag that we have made a change to the mesh => this is needed to know that the mesh interactor has to be updated

            if(mesh_mode == mesh_mode_triangles){
                mesh.setMode(OF_PRIMITIVE_TRIANGLES); 
                for(int y = 0; y < h - mesh_resolution_y; y += mesh_resolution_y) {
                    for(int x = 0; x < w - mesh_resolution_x; x += mesh_resolution_x) {
                        float distance = kinect->getDistanceAt(x, y);
                        if(distance > near_threshold && distance < far_threshold) {
                            ofVec3f current = kinect->getWorldCoordinateAt(x, y);
                            ofVec3f right = kinect->getWorldCoordinateAt(x + mesh_resolution_x, y);
                            ofVec3f below = kinect->getWorldCoordinateAt(x, y + mesh_resolution_y);

                            if(abs(current.distance(right)) < depth_threshold_max && abs(current.distance(below)) < depth_threshold_max){
                                // apply offset
                                current += offset;
                                right += offset;
                                below += offset;
                                
                                if(mirror){
                                    current.x *= -1;
                                    right.x *= -1;
                                    below.x *= -1;
                                }
                                
                                // add to the mesh
                                mesh.addVertex(current);
                                mesh.addVertex(right);
                                mesh.addVertex(below);
                            }
                        }
                    }
                }
            }
    //        else if (mesh_mode == mesh_mode_quads){
    //            mesh.setMode(OF_PRIMITIVE_QUADS);
    //            for(int y = 0; y < h - mesh_resolution_y; y += mesh_resolution_y) {
    //                for(int x = 0; x < w - mesh_resolution_x; x += mesh_resolution_x) {
    //                    float distance = kinect->getDistanceAt(x, y);
    //                    if(distance > near_threshold && distance < far_threshold) {
    //                        ofVec3f current = kinect->getWorldCoordinateAt(x, y);
    //                        ofVec3f right = kinect->getWorldCoordinateAt(x + mesh_resolution_x, y);
    //                        ofVec3f below = kinect->getWorldCoordinateAt(x, y + mesh_resolution_y);
    //                        ofVec3f rightbelow = kinect->getWorldCoordinateAt(x + mesh_resolution_x, y + mesh_resolution_y);
    //                        
    //                        if(abs(current.distance(right)) < depth_threshold_max && abs(current.distance(below)) < depth_threshold_max && abs(current.distance(rightbelow)) < depth_threshold_max){
    //                            // apply offset
    //                            current += offset;
    //                            right += offset;
    //                            below += offset;
    //                            rightbelow += offset;
    //                            
    //                            if(mirror){
    //                                current.x *= -1;
    //                                right.x *= -1;
    //                                below.x *= -1;
    //                                rightbelow.x *= -1;
    //                            }
    //                            
    //                            // apply the offset and add to the mesh
    //                            mesh.addVertex(current);
    //                            mesh.addVertex(right);
    //                            mesh.addVertex(rightbelow);
    //                            mesh.addVertex(below);
    //                        }
    //                    }
    //                }
    //            }
    //        }
            else if (mesh_mode == mesh_mode_lines){
                mesh.setMode(OF_PRIMITIVE_LINES);
                
                // make the horizontal lines
                for(int y = 0; y < h - mesh_resolution_y; y+= mesh_resolution_y) {
                    for(int x = 0; x < w - mesh_resolution_x; x+=mesh_resolution_x) {
                        ofVec3f current = kinect->getWorldCoordinateAt(x, y);
                        
                        if(current.z > near_threshold && current.z < far_threshold){
                            ofVec3f right = kinect->getWorldCoordinateAt(x + mesh_resolution_x, y);
                            if(abs(current.z - right.z) < depth_threshold_max) {
                                // apply offset
                                current += offset;
                                right += offset;
                                
                                if(mirror){
                                    current.x *= -1;
                                    right.x *= -1;
                                }
                                
                                // apply the offset and add to the mesh
                                mesh.addVertex(current);
                                mesh.addVertex(right);
                            }
                        }
                    }
                }
            }
            
            transceiver->send(&mesh);
        }
    }
    
    // try to initialize the kinect again when connection was lost
    // unfortunately the program normally crashes when the kinect has been disconnected
    else {
        // clear the mesh while the kinect is not delivering data
        mesh.clear();
        kinect = new ofxKinect();
        if(kinect->isConnected()){
        kinect->setRegistration(true);   // enable depth->video image calibration
        kinect->init();
        kinect->open();
        kinect->setLed(ofxKinect::LED_OFF);}
    }
}

//--------------------------------------------------------------
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
            ofPoint current = kinect->getWorldCoordinateAt(contourFinder.blobs[i].pts[point].x, contourFinder.blobs[i].pts[point].y) + offset;
            ofPoint next = kinect->getWorldCoordinateAt(contourFinder.blobs[i].pts[point+1].x, contourFinder.blobs[i].pts[point+1].y) + offset;
            
            if(mirror){
                current.x *= -1;
                next.x *= -1;
            }                                                           
            
            lines.addVertex(current);
            lines.addVertex(next);
        }

        lines.drawWireframe();                          // draw the contour
    }
}

//--------------------------------------------------------------
void meshMan::drawDebug(){
    grayImage.draw(0, 0, 320, 240);
    grayThreshFar.draw(330, 0, 320, 240);
    grayThreshNear.draw(0, 250, 320, 240);
    contourFinder.draw(330, 250, 320, 240);
}

//--------------------------------------------------------------
void meshMan::updateFromNetwork(){
    if(transceiver->receive(&temp_mesh)){
        mesh = ofMesh(temp_mesh);
        frame_new = true; // flag that we have made a change to the mesh => this is needed to know that the mesh interactor has to be updated
    }

    // clear the mesh again if the user is beamed out. the reason for generating the mesh in this state is that the network buffer does not overflow.
    if(beam_state == beamed_out)
        mesh.clear();
}

//--------------------------------------------------------------
void meshMan::draw(){
    ofPushStyle();
    ofSetColor(color);
    
    switch(beam_state){
        case beaming_in:
            drawBeamInOut(fader);
            fader += 0.01f;
            if (fader >= 1.0f) beam_state = beamed_in;
            break;
        case beaming_out:
            drawBeamInOut(fader);
            fader -= 0.01f;
            if (fader <= 0) beam_state = beamed_out;
            break;
        case beamed_in:
            mesh.drawWireframe();
            if(draw_contour) drawContour();
            break;
        case beamed_out:
            // draw nothing
            break;
    }
    
    ofPopStyle();

    // draw flashes
    ofPushStyle();
    for (int i=0; i<flash_list.size(); i++)
        flash_list[i]->draw();
    ofPopStyle();
}

//--------------------------------------------------------------
bool meshMan::isBeamedIn(){
    if(beam_state == beamed_in) return true;
    else return false;
}

//--------------------------------------------------------------
bool meshMan::isBeamedOut(){
    if(beam_state == beamed_out) return true;
    else return false;
}

//--------------------------------------------------------------
void meshMan::beamIn(){
    if(beam_state == beamed_out){
        beam_state = beaming_in;
        fader = 0;
    }
}

//--------------------------------------------------------------
void meshMan::beamOut(){
    if(beam_state == beamed_in){
        beam_state = beaming_out;
        fader = 1;
    }
}

//--------------------------------------------------------------
void meshMan::tryCreateFlash(const ofVec3f &start, const ofVec3f &end)
{
    int i;
    // find inactive
    for (i=0; i<flash_list.size(); i++) {
        if (!flash_list[i]->isActive()) break;
    }
    
    
    if (i >= flash_list.size()) {
        i = -1;
        if (flash_list.size() < max_flashes) {
            flash_list.push_back(new meshFlash());
            i = flash_list.size()-1;
        }
    }
    
    if (i>=0) {
        flash_list[i]->create(start, end, flash_amplitude);
        flash_list[i]->setColor(beam_color);
        flash_list[i]->setLineWidth(beam_line_width);
    }    
}

//--------------------------------------------------------------
void meshMan::beamFlash(const ofVec3f &vertex)
{
    tryCreateFlash(ofVec3f(0,-1500,0), vertex);
}

//--------------------------------------------------------------
void meshMan::drawBeamInOut(float fader)
{
    int numVerts = mesh.getNumVertices();
    
    ofVec3f *vertices = mesh.getVerticesPointer();
    
    int i;
    
    float minY = 100000.0f, maxY = -100000.0f;
    
    for (i=0; i<numVerts; i++)
    {
        if (vertices[i].y < minY) minY = vertices[i].y;
        else if (vertices[i].y > maxY) maxY = vertices[i].y;
    }
    
    float drawTreshold = maxY - (maxY-minY) * fader;
    float flashTreshold = drawTreshold - beam_flash_vertex_range;

    for (i=0; i < numVerts; i += (1 + beam_flash_skip_vertices))
    {
        if (vertices[i].y > flashTreshold && vertices[i].y < drawTreshold)
        {
            beamFlash(vertices[i]);
        }
    }
   
    
    i=0;
    
    // draw the mesh "by hand"
    while (i < numVerts)
    {
        ofNoFill();
        switch(mesh.getMode())
        {
            case OF_PRIMITIVE_TRIANGLES:
                if (vertices[i].y > drawTreshold && vertices[i+1].y > drawTreshold && vertices[i+2].y > drawTreshold) {
                    ofTriangle(vertices[i].x, vertices[i].y, vertices[i].z,   vertices[i+1].x, vertices[i+1].y, vertices[i+1].z,   vertices[i+2].x, vertices[i+2].y, vertices[i+2].z);
                }
                i+=3;
                break;
//            case OF_PRIMITIVE_QUADS:
//                {
//                    ofPoint p1(vertices[i]);
//                    ofPoint p2(vertices[i]);
//                    ofPoint p3(vertices[i]);
//                    ofPoint p4(vertices[i]);
//                    
//                    if (p1.y > drawTreshold && p2.y > drawTreshold && p3.y > drawTreshold && p4.y > drawTreshold) {
//                        ofLine(p1, p2);
//                        ofLine(p2, p3);
//                        ofLine(p3, p4);
//                        ofLine(p4, p1);
//                    }
//                    i+=4;
//                }
//                break;
                
            case OF_PRIMITIVE_LINES:
                if (vertices[i].y > drawTreshold && vertices[i+1].y > drawTreshold) {
                    ofLine(vertices[i].x, vertices[i].y, vertices[i].z, vertices[i+1].x, vertices[i+1].y, vertices[i+1].z);
                }
                i+=2;
                break;
        }
    }
    
}

//--------------------------------------------------------------
void meshMan::addMeshVertex(float x, float y, float z){
    ofVec3f v;
    v.x = x;
    v.y = y;
    v.z = z;
}
