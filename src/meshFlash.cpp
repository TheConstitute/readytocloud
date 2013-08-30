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

void meshFlash::create(ofVec3f start, ofVec3f end, float amplitude)
{
    mesh.clear();
    
    int i;
    
    ofVec3f flashDir = end - start;
    flashDir.normalize();
    
    ofVec3f flashDerangeDir;
    getPerpendicular(flashDir, flashDerangeDir);
    
    
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
            
            float rf = amplitude * ((float)step) / ((float)numVerts) * ofRandomf();
            
            int cvi = v + step / 2;
//            mesh.setVertex(cvi, v1 + delta * 0.5f + flashDerangeDir * rf);
            mesh.setVertex(cvi, v1 + delta * 0.5f + ofVec3f(0,1,0) * rf);
            
//            float f = ((float)v) / (float)numVerts;
//            mesh.setVertex(v, start + (flashDir * f) + ofVec3f(0, amplitude * sin(f * 2 * 3.1415),0));
        }
        step /= 2;
    }
}

void meshFlash::draw(void)
{
    mesh.drawWireframe();
}

void meshFlash::update(float deltaTime)
{
    
}