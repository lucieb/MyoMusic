//
//  MyoDataCollector.cpp
//  ofxMyo
//
//  Created by Lucie Bélanger on 2014-09-27.
//
//

#include "MyoDataCollector.h"

// Classes that inherit from myo::DeviceListener can be used to receive events from Myo devices. DeviceListener
// provides several virtual functions for handling different kinds of events. If you do not override an event, the
// default behavior is to do nothing.

MyoDataCollector::MyoDataCollector()
    : onArm(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose()
    {
    }
    
    // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
    void MyoDataCollector::onUnpair(myo::Myo* myo, uint64_t timestamp)
    {
        // We've lost a Myo.
        // Let's clean up some leftover state.
        roll_w = 0;
        pitch_w = 0;
        yaw_w = 0;
        onArm = false;
    }
    
    // onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
    // as a unit quaternion.
    void MyoDataCollector::onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
    {
        using std::atan2;
        using std::asin;
        using std::sqrt;
        
        // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
        float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                           1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
        float pitch = asin(2.0f * (quat.w() * quat.y() - quat.z() * quat.x()));
        float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                          1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
        
        // Convert the floating point angles in radians to a scale from 0 to 20.
        roll_w = static_cast<int>((roll + (float)M_PI)/(M_PI * 2.0f) * 18);
        pitch_w = static_cast<int>((pitch + (float)M_PI/2.0f)/M_PI * 18);
        yaw_w = static_cast<int>((yaw + (float)M_PI)/(M_PI * 2.0f) * 18);
    }
    
    // onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
    // making a fist, or not making a fist anymore.
    void MyoDataCollector::onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        currentPose = pose;
        
        // Vibrate the Myo whenever we've detected that the user has made a fist.
        if (pose == myo::Pose::fist) {
            myo->vibrate(myo::Myo::vibrationMedium);
        }
    }
    
    // onArmRecognized() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
    // arm. This lets Myo know which arm it's on and which way it's facing.
    void MyoDataCollector::onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
    {
        onArm = true;
        whichArm = arm;
    }
    
    // onArmLost() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
    // it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
    // when Myo is moved around on the arm.
    void MyoDataCollector::onArmLost(myo::Myo* myo, uint64_t timestamp)
    {
        onArm = false;
    }
    
    // There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
    // For this example, the functions overridden above are sufficient.
    
    // We define this function to print the current values that were updated by the on...() functions above.
    void MyoDataCollector::print()
    {
        // Clear the current line
        std::cout << '\r';
        
        // Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
        std::cout << roll_w << '[' << std::string(roll_w, '*') << std::string(18 - roll_w, ' ') << ']'
        << pitch_w << '[' << std::string(pitch_w, '*') << std::string(18 - pitch_w, ' ') << ']'
        << yaw_w << '[' << std::string(yaw_w, '*') << std::string(18 - yaw_w, ' ') << ']';
        
        if (onArm) {
            // Print out the currently recognized pose and which arm Myo is being worn on.
            
            // Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
            // output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
            // that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
            std::string poseString = currentPose.toString();
            
            std::cout << '[' << (whichArm == myo::armLeft ? "L" : "R") << ']'
            << '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
        } else {
            // Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
            std::cout << "[?]" << '[' << std::string(14, ' ') << ']';
        }
        
        std::cout << std::flush;
    }

