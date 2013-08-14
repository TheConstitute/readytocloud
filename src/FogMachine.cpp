//
//  FogMachine.cpp
//  Pointcloud_v11
//
//  Created by Julian Adenauer on 03.04.13.
//
//

#include "FogMachine.h"


FogMachine::FogMachine(){
}

void FogMachine::setup(ofxDmx* dmx, unsigned char startAddress){
    this->dmx = dmx;
    this->startAddress = startAddress;
}

void FogMachine::update(){
    // wenn Timer abgelaufen ist, den Nebel abstellen
    if(timer && ofGetElapsedTimef() > endTime){
        timer = false;
        on = false;
    }
    
    if(on){
        dmx->setLevel(startAddress, level);
    }
    else {
        dmx->setLevel(startAddress, 0);
    }
}

void FogMachine::setLevel(unsigned char level){
    this->level = level;
}

unsigned char FogMachine::getLevel(){
    return this->level;
}

void FogMachine::fogOnTimer(int seconds){
    endTime = ofGetElapsedTimef() + seconds;
    timer = true;
    on = true;
}

void FogMachine::fogOn(){
    on = true;
}

void FogMachine::fogOff(){
    on = false;
}

bool FogMachine::isFogging(){
    return on;
}