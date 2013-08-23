//
//  LEDRing.h
//  Pointcloud_v10
//
//  Created by Julian Adenauer on 02.04.13.
//
//

#ifndef Pointcloud_v10_LEDRing_h
#define Pointcloud_v10_LEDRing_h

#include "ofMain.h"
#include "ofxDmx.h"

// number that has to be added to the start address
#define RED_CHANNEL 0
#define GREEN_CHANNEL 1
#define BLUE_CHANNEL 2

class LEDRing {
    
public:
    LEDRing();
    void setup(ofxDmx* dmx, unsigned char startAddress);
    void draw();
    void draw(int x, int y, int w, int h);
    void update();
    void setDMXDevice(ofxDmx* dmx);
    int getStartAddress();
    void setStartAddress(int startAddress);

    void setColor(unsigned char r, unsigned char g, unsigned char b);
    void setColor(ofColor c);
    ofColor getColor();

    void setRed(unsigned char red);
    unsigned char getRed();
    void setGreen(unsigned char green);
    unsigned char getGreen();
    void setBlue(unsigned char blue);
    unsigned char getBlue();

    void setBrightness(unsigned char b);
    unsigned char getBrightness();

    unsigned char* getChannels();
    int getNumChannels();
    
    void setPulseMin(unsigned char pulseMin);
    unsigned char getPulseMin();
    void setPulseMax(unsigned char pulseMax);
    unsigned char getPulseMax();
    void setPulseSpeed(int pulseSpeed){ this->pulseSpeed = pulseSpeed; }
    int getPulseSpeed(){ return this->pulseSpeed; }
    void setStaticBrightness(unsigned char brightness){ static_brightness = brightness; }
    unsigned char getStaticBrightness(){ return static_brightness; }
    
    void setFadeTime(float fadeTime);
    float getFadeTime();
    
    // advanced functions
    void fadeBrightness(unsigned char endValue, float timeSeconds);
    void fadeIn();
    void fadeOut();
    void pulseBrightness(unsigned char min, unsigned char max, int speed);
    void pulseBrightness();
    void stopPulse();
    
    ofParameter<int> pulseSpeed;
    ofParameter<int> pulseMin, pulseMax;
    ofParameter<int> static_brightness;
    
private:
    ofxDmx* dmx;
    int startAddress = 0;
    unsigned char channels[3];
    bool fading = false;
    bool pulsing = false;
    
    float fadeEndTime;
    float fadeDelta;
    float fadeTime = 2;
    unsigned char fadeEndValue;
    

    bool firstFade = true;
    float pulseStartTime;

    
    void updateFade();
    void updatePulse();
};


#endif
