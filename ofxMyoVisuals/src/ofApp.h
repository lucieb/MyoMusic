#pragma once

#define USING_MYO

#ifdef USING_MYO
#include <myo/myo.hpp>
#endif
#include "ofMain.h"
#include "MyoDataCollector.h"
#include "Quad.h"
#include "ofFbo.h"
#include "ofxBlur.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void poseChangedFor(myo::Pose newPose);
    void triggerLeft();
    void triggerRight();
    
    
private:
    
    ofFbo fbo;
    ofxBlur ofxBlur;
    
    vector<Quad*> quads;
    int activeQuad;
    
#ifdef USING_MYO
    myo::Myo* myo;
    MyoDataCollector collector;
    myo::Hub* hub;
    myo::Pose currentPose;
    myo::Pose lastPose;
    int lastPitch;
    bool poseChanged;
#endif
    
		
};
