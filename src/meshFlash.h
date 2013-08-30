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

    void create(ofVec3f start, ofVec3f end, float amplitude);
    
    void update(float deltaTime);
    
    void draw();

    
private:

    void getPerpendicular(const ofVec3f &vec, ofVec3f &pv);
    
    int iterations = 6;
    
    ofMesh mesh;

};
#endif
