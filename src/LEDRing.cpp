//
//  LEDRing.cpp
//  Pointcloud_v11
//
//  Created by Julian Adenauer on 02.04.13.
//
//

#include "LEDRing.h"

LEDRing::LEDRing(){
    // initialisieren auf null
    channels[0] = 0;
    channels[1] = 0;
    channels[2] = 0;
}

void LEDRing::setup(ofxDmx* dmx, unsigned char startAddress){
    this->dmx = dmx;
    this->startAddress = startAddress;
    
    // initialisieren auf null
    channels[0] = 0;
    channels[1] = 0;
    channels[2] = 0;

}

void LEDRing::draw(){
    
}

void LEDRing::draw(int x, int y, int w, int h){
    
}

void LEDRing::update(){
    
    if(fading){ updateFade(); }
    if(pulsing){ updatePulse(); }
    
    // TODO: Checken, ob das mit dem sizeof funktioniert
    for(int i=0; i<sizeof(channels); i++) {
        dmx->setLevel(i + startAddress, channels[i]);
    }
}

void LEDRing::setDMXDevice(ofxDmx* dmx){
    this->dmx = dmx;
}

int LEDRing::getStartAddress(){
    return startAddress;
}

void LEDRing::setStartAddress(int startAddress){
    this->startAddress = startAddress;
}

void LEDRing::setColor(unsigned char r, unsigned char g, unsigned char b){
    channels[RED_CHANNEL] = r;
    channels[GREEN_CHANNEL] = g;
    channels[BLUE_CHANNEL] = b;
}

void LEDRing::setColor(ofColor c){
    channels[RED_CHANNEL] = c.r;
    channels[GREEN_CHANNEL] = c.g;
    channels[BLUE_CHANNEL] = c.b;
}

ofColor LEDRing::getColor(){
    ofColor color;
    color.r = channels[RED_CHANNEL];
    color.g = channels[GREEN_CHANNEL];
    color.b = channels[BLUE_CHANNEL];
    return color;
}

void LEDRing::setRed(unsigned char red){
    channels[RED_CHANNEL] = red;
}

unsigned char LEDRing::getRed(){
    return channels[RED_CHANNEL];
}

void LEDRing::setGreen(unsigned char green){
    channels[GREEN_CHANNEL] = green;
}

unsigned char LEDRing::getGreen(){
    return channels[GREEN_CHANNEL];
}

void LEDRing::setBlue(unsigned char blue){
    channels[BLUE_CHANNEL] = blue;
}

unsigned char LEDRing::getBlue(){
    return channels[BLUE_CHANNEL];
}


void LEDRing::setBrightness(unsigned char b){
    // TODO: brightness setzt momentan die farbe auf weiß. hier könnte man noch irgendeinen schönen algorithmus schreiben, der dimmen kann ohne das farbverhältnis zu verändern
    channels[RED_CHANNEL] = b;
    channels[BLUE_CHANNEL] = b;
    channels[GREEN_CHANNEL] = b;
}

unsigned char LEDRing::getBrightness(){
    // TODO: hier wird momentan nur der durchschnittswert aus den farbwerten gebildet. das ist nicht wirklich die helligkeit...
    return (channels[RED_CHANNEL] + channels[GREEN_CHANNEL] + channels[BLUE_CHANNEL]) / 3;
}

unsigned char* LEDRing::getChannels(){
    return channels;
}

int LEDRing::getNumChannels(){
    return sizeof(channels);
}

// advanced functions
void LEDRing::fadeBrightness(unsigned char endValue, float timeSeconds){
    if(!fading)
    {
        if(timeSeconds > 0)
            fadeDelta = (endValue - getBrightness()) / timeSeconds;
        else
            return;
        fadeEndTime = timeSeconds + ofGetElapsedTimef();
        fadeEndValue = endValue;
        fading = true;
    }
}

void LEDRing::fadeIn(){
    fadeBrightness(static_brightness, fadeTime);
}

void LEDRing::fadeOut(){
    fadeBrightness(0, fadeTime);
}

void LEDRing::updateFade(){
    if(ofGetElapsedTimef() >= fadeEndTime){
        setBrightness(fadeEndValue);
        fading = false;
    }
    else{
        float timeLeft = fadeEndTime - ofGetElapsedTimef();
        setBrightness(fadeEndValue - char(timeLeft * fadeDelta));
    }
}

void LEDRing::pulseBrightness(){
    pulsing = true;
}

void LEDRing::pulseBrightness(unsigned char min, unsigned char max, int speed = 3){
//    if(!pulsing)
//    {
//        firstFade = true;
//        fadeBrightness(min, 2);
//    }
    
    pulsing = true;
    pulseSpeed = speed;
    pulseMin = min;
    pulseMax = max;

    // TODO: Der Fade-Geschwindigkeit sollte eigentlich irgendwie mit der Puls-Geschwindigkeit zusammenhängen
    
}

void LEDRing::setPulseMin(unsigned char pulseMin){
    this->pulseMin = pulseMin;
}

unsigned char LEDRing::getPulseMin(){
    return pulseMin;
}

void LEDRing::setPulseMax(unsigned char pulseMax){
    this->pulseMax = pulseMax;
}

unsigned char LEDRing::getPulseMax(){
    return pulseMax;
}

void LEDRing::setFadeTime(float time){
    this->fadeTime = time;
}

float LEDRing::getFadeTime(){
    return this->fadeTime;
}


void LEDRing::updatePulse(){
    // mit pulsen erst beginnen, wenn das faden abgeschlossen ist
//    if(!fading){
//        if(firstFade)
//            pulseStartTime = ofGetElapsedTimef();
        float brightness = ofMap(sin((ofGetElapsedTimef()) * pulseSpeed), -1, 1, pulseMin, pulseMax);
        setBrightness(brightness);
//        firstFade = false;
//    }
}

void LEDRing::stopPulse(){
    pulsing = false;
}