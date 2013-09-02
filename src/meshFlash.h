//
//  meshFlash.h
//  readytocloud
//
//  Created by The Constitute on 29.08.13.
//
//

#ifndef readytocloud_meshFlash_h
#define readytocloud_meshFlash_h

#include "ofMain.h"

class meshFlash
{

public:

    meshFlash();

    void setColor(ofColor color);
    
    void create(ofVec3f start, ofVec3f end, float amplitude);
    
    //void update(float deltaTime);
    
    void draw();

    inline bool isActive()
    {
        return is_active;
    }
    
private:

    void getPerpendicular(const ofVec3f &vec, ofVec3f &pv);
    
    const int iterations = 6;
    
    float creationTime;
    
    const float max_live_time = 0.5f;
    const float min_live_time = 0.15f;
    
    float live_time;
    
    bool is_active;
    
    ofMesh mesh;

    ofColor flash_color;
};
#endif
