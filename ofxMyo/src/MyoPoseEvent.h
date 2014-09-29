//
//  MyoPoseEvent.h
//  ofxMyo
//
//  Created by Lucie Bélanger on 2014-09-27.
//
//

#ifndef __ofxMyo__MyoPoseEvent__
#define __ofxMyo__MyoPoseEvent__

#include <iostream>
#include <myo/myo.hpp>
#include "ofMain.h"

class MyoPoseEvent : public ofEventArgs {
    
public:
    
    myo::Pose pose;
    
    MyoPoseEvent() {
        pose = myo::Pose::unknown;
    }
    
    static ofEvent <MyoPoseEvent> events;
};

#endif /* defined(__ofxMyo__MyoPoseEvent__) */
