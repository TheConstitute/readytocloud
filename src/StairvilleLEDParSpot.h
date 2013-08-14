//
//  StairvilleLEDParSpot.h
//  Pointcloud_v11
//
//  Created by Julian Adenauer on 02.04.13.
//
//

#ifndef __Pointcloud_v10__StairvilleLEDParSpot__
#define __Pointcloud_v10__StairvilleLEDParSpot__

#include "ofMain.h"
#include "ofxDmx.h"

// number that has to be added to the start address 
#define RED_CHANNEL 0
#define GREEN_CHANNEL 1
#define BLUE_CHANNEL 2
#define MACRO_CHANNEL 3
#define STROBE_CHANNEL 4
#define MODUS_CHANNEL 5
#define BRIGHTNESS_CHANNEL 6

class StairvilleLEDParSpot {
    
public:
    StairvilleLEDParSpot();
    void setup(ofxDmx* dmx, unsigned char startAddress);
    void draw();
    void draw(int x, int y, int w, int h);
    void update();

    void setColor(unsigned char r, unsigned char g, unsigned char b);
    ofColor getColor(){ return ofColor(channels[RED_CHANNEL], channels[GREEN_CHANNEL], channels[BLUE_CHANNEL]); }
    void setColor(ofColor c);
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
    
    // getters & setters
    void setDMXDevice(ofxDmx* dmx){ this->dmx = dmx; }
    int getStartAddress(){ return startAddress; }
    void setStartAddress(int startAddress){ this->startAddress = startAddress; }
    
    void setFadeTime(float time){ this->fadeTime = time; }
    float getFadeTime(){ return fadeTime; }
    void setPulseMin(unsigned char pulseMin){ this->pulseMin = pulseMin; }
    unsigned char getPulseMin(){ return pulseMin; }
    void setPulseMax(unsigned char pulseMax) { this->pulseMax = pulseMax; }
    unsigned char getPulseMax() { return pulseMax; }
    void setPulseSpeed(int pulseSpeed){ this->pulseSpeed = pulseSpeed; }
    int getPulseSpeed(){ return this->pulseSpeed; }
    
    void setStaticBrightness(unsigned char brightness){ static_brightness = brightness; }
    unsigned char getStaticBrightness(){ return static_brightness; }
    
    // advanced functions
    void fadeBrightness(unsigned char endValue, float timeSeconds);
    void fadeIn();
    void fadeOut();
    void pulseBrightness(unsigned char min, unsigned char max, int speed);
    void pulseBrightness();
    void stopPulse();

    
private:
    ofxDmx* dmx;
    int startAddress = 0;
    unsigned char channels[7];
    bool fading = false;
    bool pulsing = false;
    unsigned char fadeEndValue;
    
    float fadeEndTime;
    float fadeDelta;
    float fadeTime;
    unsigned char static_brightness;

    int pulseSpeed;
    bool firstFade = true;
    float pulseStartTime;
    int pulseMin, pulseMax;
    
    void updateFade();
    void updatePulse();
};

#endif /* defined(__Pointcloud_v10__StairvilleLEDParSpot__) */