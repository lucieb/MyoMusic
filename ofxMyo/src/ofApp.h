#pragma once

#include <myo/myo.hpp>
#include "ofMain.h"
#include "MyoDataCollector.h"
#include "MyoPoseEvent.h"


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
    
private:
    myo::Myo* myo;
    MyoDataCollector collector;
    myo::Hub* hub;
    
    void poseChanged(MyoPoseEvent &args);
		
};
