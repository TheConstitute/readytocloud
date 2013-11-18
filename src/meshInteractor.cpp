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
    
    britzel_sound.loadSound("britzelbratzel.mp3");
    britzel_sound.setLoop(true);
}

void meshInteractor::setup(meshMan *local, meshMan* remote){
    this->local = local;
    this->remote = remote;
}

void meshInteractor::update(){
    // update the sound system
    ofSoundUpdate();
    num_active_flashes = 0;
    
    // only draw flashes and play sound when both users are beamed in
    if(!local->isBeamedOut() && !remote->isBeamedOut()){
        for(int l = 0; l < local->getMesh()->getNumVertices()-resolution; l+=resolution){
            for(int r = 0; r < remote->getMesh()->getNumVertices()-resolution; r+=resolution ){
                float d = local->getMesh()->getVertex(l).distance(remote->getMesh()->getVertex(r));
                if (d < distance){
                    tryCreateFlash(local->getMesh()->getVertex(l), remote->getMesh()->getVertex(r));
                }
            }
        }
        
        // update the number of active flashes
        for (int i=0; i<flash_list.size(); i++) {
            if (flash_list[i]->isActive()) num_active_flashes++;
        }
    }
}

void meshInteractor::draw(){
    // draw flashes
    ofPushStyle();
        for (int i=0; i<flash_list.size(); i++){
            flash_list[i]->draw();
        }
    ofPopStyle();

    if (num_active_flashes > 5 && !britzel_sound.getIsPlaying())
        britzel_sound.play();
    else if (num_active_flashes <= 5 && britzel_sound.getIsPlaying())
        britzel_sound.stop();
    


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
        flash_list[i]->create(start, end, amplitude);
        flash_list[i]->setColor(color);
        flash_list[i]->setLineWidth(line_width);
    }
}
