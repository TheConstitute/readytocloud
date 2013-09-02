//
//  meshFlash.cpp
//  readytocloud
//
//  Created by The Constitute on 29.08.13.
//
//

#include "meshFlash.h"

meshFlash::meshFlash()
{
    flash_color.set(220, 220, 255);
    is_active = false;
}

void meshFlash::getPerpendicular(const ofVec3f &vec, ofVec3f &pv)
{
    // find index of smallest component
    int bi = 0;
    if (vec[bi] > vec[bi+1])
    {
        bi=1;
        if (vec[bi] > vec[bi+1]) bi=2;
    }
    else if (vec[bi] > vec[bi+2]) bi=2;
    
    pv[bi] = 1;
    
    ofVec3f temp = vec.crossed(pv);
    pv.set(vec.crossed(temp));
    pv.normalize();
}

void meshFlash::setColor(ofColor color)
{
    flash_color.set(color);
}

void meshFlash::create(ofVec3f start, ofVec3f end, float amplitude)
{
    mesh.clear();
    
    int i;
    
    ofVec3f flashDir = end - start;
    flashDir.normalize();
    
    ofVec3f flashDerangeDir1;
    ofVec3f flashDerangeDir2;
    
    getPerpendicular(flashDir, flashDerangeDir1);
    flashDerangeDir2.set(flashDir.crossed(flashDerangeDir1));
    
    int numVerts = (1 << iterations) +1;
    
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);


    
    /* debug test sine wave
    for (i=0; i<numVerts; i++)
    {
        float f = ((float)i) / (float)numVerts;
        mesh.addVertex(start + (flashDir * f) + ofVec3f(0, amplitude * sin(f * 2 * 3.1415),0));
    }
    */
    
    mesh.addVertex(start);
    for (i=1; i<numVerts-1; i++)
    {
        mesh.addVertex(ofVec3f(0,0,0));
    }
    mesh.addVertex(end);
    
    int step = numVerts -1;
    for (i=0; i<iterations; i++)
    {
        for (int v = 0; v < numVerts-1; v += step)
        {
            ofVec3f v1 = mesh.getVertex(v);
            ofVec3f v2 = mesh.getVertex(v + step);
            
            ofVec3f delta = v2 - v1;
            
            float sf = ((float)step) / ((float)numVerts);
            float rf1 = sf * amplitude * ofRandomf();
            float rf2 = sf * amplitude * ofRandomf();
            
            int cvi = v + step / 2;
            mesh.setVertex(cvi, v1 + delta * 0.5f + flashDerangeDir1 * rf1 + flashDerangeDir2 * rf2);
//            mesh.setVertex(cvi, v1 + delta * 0.5f + ofVec3f(0,1,0) * rf);

        }
        step /= 2;
    }
    creationTime = ofGetElapsedTimef();
    live_time = min_live_time + ofRandomf() * (max_live_time - min_live_time);
    
    is_active = true;
}

void meshFlash::draw(void)
{
    if (!is_active || mesh.getNumVertices() == 0) return;
    
    float timeToLive = live_time - (ofGetElapsedTimef() - creationTime);
    if (timeToLive < 0){
        timeToLive = 0;
        is_active = false;
    }
    
    ofPushStyle();
        ofSetColor(flash_color, (int)(timeToLive * 255.0f / live_time));
        mesh.drawWireframe();
    ofPopStyle();    
}

/*
void meshFlash::update(float deltaTime)
{
}
*/