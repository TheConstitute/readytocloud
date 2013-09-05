//
//  FogMachine.h
//  Pointcloud_v11
//
//  Created by Julian Adenauer on 03.04.13.
//
//

#ifndef __Pointcloud_v11__FogMachine__
#define __Pointcloud_v11__FogMachine__

#include "ofMain.h"
#include "ofxDmx.h"

class FogMachine {

public:
    FogMachine();
    void setup(ofxDmx* dmx, unsigned char startAddress);
    void update();
    void fogOnTimer(int seconds);
    void setLevel(unsigned char level);
    unsigned char getLevel();
    void fogOn();
    void fogOff();
    bool isFogging();
    
    
private:
    bool on = false;
    bool timer = false;
    unsigned char level = 120;
    unsigned char startAddress = 1;
    float endTime;
    ofxDmx* dmx;
};



#endif /* defined(__Pointcloud_v11__FogMachine__) */
