//
//  Screensaver.cpp
//  readytocloud
//
//  Created by Julian Adenauer on 16.11.13.
//
//

#include "Screensaver.h"

Screensaver::Screensaver(){
    scanner_line_position = 0;
    scanner_line_speed = 2;
    screensaver_mode = SCREENSAVER_NONE;
}

void Screensaver::setMode(int mode){
    if (mode == (int) SCREENSAVER_SCANNER) screensaver_mode = SCREENSAVER_SCANNER;
    else if (mode == (int) SCREENSAVER_NONE) screensaver_mode = SCREENSAVER_NONE;
    ofLog() << "set screensaver mode to " << screensaver_mode;
}

void Screensaver::update(){

    switch(screensaver_mode){
        case SCREENSAVER_NONE:
            // draw nothing
            break;
        case SCREENSAVER_LINES:
            break;
        case SCREENSAVER_SCANNER:
            scanner_line_position += scanner_line_speed;
            if(scanner_line_position < 0){
                scanner_line_position = 0;
                scanner_line_speed = 2;
            }
            else if (scanner_line_position > ofGetHeight()){
                scanner_line_position = ofGetHeight();
                scanner_line_speed = -2;
            }
            break;
        case SCREENSAVER_CIRCLES:
            break;
    }
}

void Screensaver::draw(){
    switch(screensaver_mode){
        case SCREENSAVER_NONE:
            // draw nothing
            break;
        case SCREENSAVER_LINES:
            break;
        case SCREENSAVER_SCANNER:
            ofPushStyle();
            ofSetLineWidth(3);
            ofSetColor(255, 255, 255);
            ofLine(0, scanner_line_position, ofGetWidth(), scanner_line_position);
            ofPopStyle();
            break;
        case SCREENSAVER_CIRCLES:
            break;
    }
}