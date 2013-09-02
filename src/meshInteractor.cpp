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
}

void meshInteractor::setup(meshMan *local, meshMan* remote){
    this->local = local;
    this->remote = remote;
}

void meshInteractor::update(){
    intersectors.clear();
    for(int l = 0; l < local->getMesh()->getNumVertices()-resolution; l+=resolution){
        for(int r = 0; r < remote->getMesh()->getNumVertices()-resolution; r+=resolution ){
            float d = local->getMesh()->getVertex(l).distance(remote->getMesh()->getVertex(r));
            if (d < distance){
                intersectors.push_back(local->getMesh()->getVertex(l));
                intersectors.push_back(remote->getMesh()->getVertex(r));
            }
        }
    }
}

void meshInteractor::draw(){
    if(intersectors.size() > 0){
        for(int i = 0; i < intersectors.size()-1; i+=2){
            ofPushStyle();
            ofSetColor(0, 255, 0);
            ofLine(intersectors[i], intersectors[i+1]);
            ofPopStyle();
        }
    }
}