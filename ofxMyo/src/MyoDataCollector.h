//
//  MyoDataCollector.h
//  ofxMyo
//
//  Created by Lucie Bélanger on 2014-09-27.
//
//

#ifndef __ofxMyo__MyoDataCollector__
#define __ofxMyo__MyoDataCollector__

#include <iostream>
#include <myo/myo.hpp>
#include <ofMain.h>
#include "MyoPoseEvent.h"

#define HOST "localhost"
#define PORT 12345

class ofxOscSender;

// Classes that inherit from myo::DeviceListener can be used to receive events from Myo devices. DeviceListener
// provides several virtual functions for handling different kinds of events. If you do not override an event, the
// default behavior is to do nothing.
class MyoDataCollector : public myo::DeviceListener {
    
private:
    ofxOscSender *sender;
    
    // We store each Myo pointer that we pair with in this list, so that we can keep track of the order we've seen
    // each Myo and give it a unique short identifier (see onPair() and identifyMyo() above).
    std::vector<myo::Myo*> knownMyos;
    
public:
    
    
    ofEvent<MyoPoseEvent> onPoseChange;
    
    // These values are set by onArmRecognized() and onArmLost() above.
    bool onArm;
    myo::Arm whichArm;
    
    // These values are set by onOrientationData() and onPose() above.
    int roll_w, pitch_w, yaw_w;
    myo::Pose currentPose;
    myo::Pose lastPose;
    
    void setup();
    void sendPose(myo::Myo* myo, int fist, int fingersSpread, int waveIn, int waveOut, int thumbToPinky);
    void sendOrientation(myo::Myo* myo, float roll, float pitch, float yaw);
    
    MyoDataCollector();
    
    size_t identifyMyo(myo::Myo* myo);
    
    // Every time Myo Connect successfully pairs with a Myo armband, this function will be called.
    void onPair(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion);
    // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
    void onUnpair(myo::Myo* myo, uint64_t timestamp);
    // onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
    // as a unit quaternion.
    void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat);
    // onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
    // making a fist, or not making a fist anymore.
    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose);
    // onArmRecognized() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
    // arm. This lets Myo know which arm it's on and which way it's facing.
    void onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection);
    
    // onArmLost() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
    // it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
    // when Myo is moved around on the arm.
    void onArmLost(myo::Myo* myo, uint64_t timestamp);
    
    // There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
    // For this example, the functions overridden above are sufficient.
    
    // We define this function to print the current values that were updated by the on...() functions above.
    void print();
};

#endif /* defined(__ofxMyo__MyoDataCollector__) */
