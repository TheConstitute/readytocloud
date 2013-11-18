//
//  Screensaver.h
//  readytocloud
//
//  Created by Julian Adenauer on 16.11.13.
//
//

#ifndef __readytocloud__Screensaver__
#define __readytocloud__Screensaver__

#include "ofMain.h"

class Screensaver {

public:
    Screensaver();
    
    void update();
    void draw();
    
    void setMode(int mode);
    
    enum screensaver_modes { SCREENSAVER_NONE, SCREENSAVER_SCANNER, SCREENSAVER_LINES, SCREENSAVER_CIRCLES } screensaver_mode;
    
private:
    int scanner_line_position;
    int scanner_line_speed;
    
    
};



#endif /* defined(__readytocloud__Screensaver__) */

