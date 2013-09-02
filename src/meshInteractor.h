//
//  meshInteractor.h
//  readytocloud
//
//  Created by Julian Adenauer on 28.08.13.
//
//

#ifndef __readytocloud__meshInteractor__
#define __readytocloud__meshInteractor__

#include "ofMain.h"
#include "meshMan.h"

class meshInteractor {
    
public:
    meshInteractor();
    void setup(meshMan *local, meshMan* remote);
    void update();
    void draw();
    
    ofParameter<int> resolution;
    ofParameter<ofColor> color;
    ofParameter<float> distance;
    
private:
    meshMan* local;
    meshMan* remote;
    vector<ofVec3f> intersectors;
};


#endif /* defined(__readytocloud__meshInteractor__) */
