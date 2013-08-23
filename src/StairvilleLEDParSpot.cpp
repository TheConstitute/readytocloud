//
//  StairvilleLEDParSpot.cpp
//  Pointcloud_v11
//
//  Created by Julian Adenauer on 02.04.13.
//
//

#include "StairvilleLEDParSpot.h"

StairvilleLEDParSpot::StairvilleLEDParSpot(){
    // initialisieren mit weiß aber helligkeit auf null
    channels[0] = 255;
    channels[1] = 255;
    channels[2] = 255;
    channels[3] = 0;
    channels[4] = 0;
    channels[5] = 0;
    channels[6] = 0;
    
    startAddress = 0;
    firstFade = true;
}

void StairvilleLEDParSpot::setup(ofxDmx* dmx, unsigned char startAddress){
    this->dmx = dmx;
    this->startAddress = startAddress;
    
    // initialisieren mit weiß aber helligkeit auf null
    channels[0] = 255;
    channels[1] = 255;
    channels[2] = 255;
    channels[3] = 0;
    channels[4] = 0;
    channels[5] = 0;
    channels[6] = 0;
}

void StairvilleLEDParSpot::draw(){
    
}

void StairvilleLEDParSpot::draw(int x, int y, int w, int h){
    
}

void StairvilleLEDParSpot::update(){
    if(fading){ updateFade(); }
    if(pulsing){ updatePulse(); }
    
    // TODO: Checken, ob das mit dem sizeof funktioniert
    for(int i=0; i<sizeof(channels); i++) {
        dmx->setLevel(i + startAddress, channels[i]);
    }
}

void StairvilleLEDParSpot::setColor(unsigned char r, unsigned char g, unsigned char b){
    channels[RED_CHANNEL] = r;
    channels[GREEN_CHANNEL] = g;
    channels[BLUE_CHANNEL] = b;
}

void StairvilleLEDParSpot::setColor(ofColor c){
    channels[RED_CHANNEL] = c.r;
    channels[GREEN_CHANNEL] = c.g;
    channels[BLUE_CHANNEL] = c.b;
}


void StairvilleLEDParSpot::setRed(unsigned char red){
    channels[RED_CHANNEL] = red;
}

unsigned char StairvilleLEDParSpot::getRed(){
    return channels[RED_CHANNEL];
}

void StairvilleLEDParSpot::setGreen(unsigned char green){
    channels[GREEN_CHANNEL] = green;
}

unsigned char StairvilleLEDParSpot::getGreen(){
    return channels[GREEN_CHANNEL];
}

void StairvilleLEDParSpot::setBlue(unsigned char blue){
    channels[BLUE_CHANNEL] = blue;
}

unsigned char StairvilleLEDParSpot::getBlue(){
    return channels[BLUE_CHANNEL];
}

void StairvilleLEDParSpot::setBrightness(unsigned char b){
    channels[BRIGHTNESS_CHANNEL] = b;
}

unsigned char StairvilleLEDParSpot::getBrightness(){
    return channels[BRIGHTNESS_CHANNEL];
}

unsigned char* StairvilleLEDParSpot::getChannels(){
    return channels;
}

int StairvilleLEDParSpot::getNumChannels(){
    return sizeof(channels);
}

// advanced functions
void StairvilleLEDParSpot::fadeBrightness(unsigned char endValue, float timeSeconds){
//    if(!fading)
//    {
        if(timeSeconds > 0)
            fadeDelta = (endValue - getBrightness()) / timeSeconds;
        else
            return;
        fadeEndTime = timeSeconds + ofGetElapsedTimef();
        fadeEndValue = endValue;
        fading = true;
//    }
}

void StairvilleLEDParSpot::fadeIn(){
    fadeBrightness(static_brightness, fadeTime);
}

void StairvilleLEDParSpot::fadeOut(){
    fadeBrightness(0, fadeTime);
}

void StairvilleLEDParSpot::updateFade(){
    if(ofGetElapsedTimef() >= fadeEndTime){
        channels[BRIGHTNESS_CHANNEL] = fadeEndValue;
        fading = false;
    }
    else{
        float timeLeft = fadeEndTime - ofGetElapsedTimef();
        channels[BRIGHTNESS_CHANNEL] = fadeEndValue - char(timeLeft * fadeDelta);
    }
}

void StairvilleLEDParSpot::pulseBrightness(unsigned char min, unsigned char max, int time = 3){
    // TODO: Fade zum ersten Pulse-Wert
    // fade to min und dann muss man im sin nicht die aktuelle Time nehmen, sondern das delta zur startzeit, wenn der fade vorbei ist

//    if(!pulsing)
//    {
//        firstFade = true;
//        fadeBrightness(min, 2);
//    }
    
    pulsing = true;
    pulseSpeed = time;
    pulseMin = min;
    pulseMax = max;
    // TODO: Der Fade-Geschwindigkeit sollte eigentlich irgendwie mit der Puls-Geschwindigkeit zusammenhängen
}

void StairvilleLEDParSpot::pulseBrightness(){
    pulsing = true;
}

void StairvilleLEDParSpot::updatePulse(){
    // mit pulsen erst beginnen, wenn das faden abgeschlossen ist
//    if(!fading){
//        if(firstFade)
//            pulseStartTime = ofGetElapsedTimef();
        float brightness = ofMap(sin((ofGetElapsedTimef()) * pulseSpeed), -1, 1, pulseMin, pulseMax);
        setBrightness(brightness);
//        firstFade = false;
//    }
}

void StairvilleLEDParSpot::stopPulse(){
    pulsing = false;
}