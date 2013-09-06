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
    ofParameter<float> line_width;
    ofParameter<float> amplitude;
    
private:
    void tryCreateFlash(const ofVec3f &start, const ofVec3f &end);
    meshMan* local;
    meshMan* remote;
    vector<meshFlash *> flash_list;
    int max_flashes = 500;
};


#endif /* defined(__readytocloud__meshInteractor__) */
