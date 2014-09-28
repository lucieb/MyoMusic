//
//  Quad.h
//  ofxMyoVisuals
//
//  Created by Lucie Bélanger on 2014-09-27.
//
//

#ifndef __ofxMyoVisuals__Quad__
#define __ofxMyoVisuals__Quad__

#include <ofMain.h>
#include "ofxPlaylist.h"

class Quad {
    
public:
    
    enum QuadState {
        QuadLeft = -1,
        QuadCenter = 0,
        QuadRight = 1
    };
    QuadState getState() { return state; }
    
    Quad();
    ~Quad();
    
    void setup();
    void update();
    void draw();
    
    bool animateLeft();
    bool animateRight();
    bool isAnimated() { return animating; }
    
    void setLeft();
    void setRight();
    
    void setVideoIdx(int idx);
    int getVideoIdx() { return videoIdx; }
    void setImageIdx(int idx);
    int getImageIdx() { return imageIdx; }
    
    
    void onKeyframe(ofxPlaylistEventArgs& args);
    
private:
    
    vector<string> videoFilenames;
    ofVideoPlayer video;
    int videoIdx;
    
    vector<string> imageFilenames;
    ofImage image;
    int imageIdx;
    
    QuadState state;
    bool animating;
    
    ofVec3f rot;
    ofVec3f rotAxis;
    ofVec3f pos;
    float scale;
    ofColor color;

    ofFbo   fbo;
    ofxPlaylist playlist;
    
    string kViewAnimLStarted;
    string kViewAnimLEnded;
    string kViewAnimRStarted;
    string kViewAnimREnded;
    
};
#endif /* defined(__ofxMyoVisuals__Quad__) */
