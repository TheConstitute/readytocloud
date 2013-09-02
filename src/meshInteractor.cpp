//
//  meshInteractor.cpp
//  readytocloud
//
//  Created by Julian Adenauer on 28.08.13.
//
//

#include "meshInteractor.h"


meshInteractor::meshInteractor(){
    distance = 300;
    resolution = 10;
    color = ofColor(0, 255, 0);
    line_width = 1;
}

void meshInteractor::setup(meshMan *local, meshMan* remote){
    this->local = local;
    this->remote = remote;
}

void meshInteractor::update(){    
    for(int l = 0; l < local->getMesh()->getNumVertices()-resolution; l+=resolution){
        for(int r = 0; r < remote->getMesh()->getNumVertices()-resolution; r+=resolution ){
            float d = local->getMesh()->getVertex(l).distance(remote->getMesh()->getVertex(r));
            if (d < distance){
                tryCreateFlash(local->getMesh()->getVertex(l), remote->getMesh()->getVertex(r));
            }
        }
    }
}

void meshInteractor::draw(){
    // draw flashes
    ofPushStyle();
    ofSetLineWidth(line_width);
    for (int i=0; i<flash_list.size(); i++){
        flash_list[i]->draw();
    }
    
    ofPopStyle();

}

//--------------------------------------------------------------
void meshInteractor::tryCreateFlash(const ofVec3f &start, const ofVec3f &end)
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
        flash_list[i]->create(start, end, 300.0f);
        flash_list[i]->setColor(color);
    }
}
