//
//  MyoDataCollector.cpp
//  ofxMyo
//
//  Created by Lucie Bélanger on 2014-09-27.
//
//

#include "MyoDataCollector.h"
#include "ofxOsc.h"

// Classes that inherit from myo::DeviceListener can be used to receive events from Myo devices. DeviceListener
// provides several virtual functions for handling different kinds of events. If you do not override an event, the
// default behavior is to do nothing.

MyoDataCollector::MyoDataCollector()
    : onArm(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose(), lastPose()
    {
    }

void MyoDataCollector::setup() {
    
    sender = new ofxOscSender();
    sender->setup(HOST, PORT);
}

// This is a utility function implemented for this sample that maps a myo::Myo* to a unique ID starting at 1.
// It does so by looking for the Myo pointer in knownMyos, which onPair() adds each Myo into as it is paired.
size_t MyoDataCollector::identifyMyo(myo::Myo* myo) {
    // Walk through the list of Myo devices that we've seen pairing events for.
    for (size_t i = 0; i < knownMyos.size(); ++i) {
        // If two Myo pointers compare equal, they refer to the same Myo device.
        if (knownMyos[i] == myo) {
            return i + 1;
        }
    }
    
    return 0;
}

    void MyoDataCollector::onPair(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion)
    {
        // Print out the MAC address of the armband we paired with.
        
        // The pointer address we get for a Myo is unique - in other words, it's safe to compare two Myo pointers to
        // see if they're referring to the same Myo.
        
        // Add the Myo pointer to our list of known Myo devices. This list is used to implement identifyMyo() below so
        // that we can give each Myo a nice short identifier.
        knownMyos.push_back(myo);
        
        // Now that we've added it to our list, get our short ID for it and print it out.
        std::cout << "Paired with " << identifyMyo(myo) << "." << std::endl;
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
        
        sendOrientation(myo, roll_w/18.0*100.0, pitch_w/18.0*100.0, yaw_w/18.0*100.0);
    }
    
    // onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
    // making a fist, or not making a fist anymore.
    void MyoDataCollector::onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        currentPose = pose;
        
        {
            if (pose == myo::Pose::fist) {
                
                sendPose(myo, 1, 0, 0, 0, 0);
                
            } else if (pose == myo::Pose::fingersSpread) {
                
                sendPose(myo, 0, 1, 0, 0, 0);
                
            } else if (pose == myo::Pose::waveIn) {
                
                sendPose(myo, 0, 0, 1, 0, 0);
                
            } else if (pose == myo::Pose::waveOut) {
                
                sendPose(myo, 0, 0, 0, 1, 0);
                
            } else if (pose == myo::Pose::thumbToPinky) {
                
                sendPose(myo, 0, 0, 0, 0, 1);
                
            } else {
                
                sendPose(myo, 0, 0, 0, 0, 0);
            }
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


void MyoDataCollector::sendPose(myo::Myo* myo, int fist, int fingersSpread, int waveIn, int waveOut, int thumbToPinky) {
    
    char address[100];
    unsigned int id = identifyMyo(myo);
    
    ofxOscMessage m;
    
    sprintf(address, "/myo-music/%u/pose/new", id);
    m.setAddress(address);
    m.addStringArg("bang");
    sender->sendMessage(m);
    
    m.clear();
    sprintf(address, "/myo-music/%u/pose/fist", id);
    m.setAddress(address);
    m.addIntArg(fist);
    sender->sendMessage(m);
    
    m.clear();
    sprintf(address, "/myo-music/%u/pose/fingerSpread", id);
    m.setAddress(address);
    m.addIntArg(fingersSpread);
    sender->sendMessage(m);
    
    m.clear();
    sprintf(address, "/myo-music/%u/pose/waveIn", id);
    m.setAddress(address);
    m.addIntArg(waveIn);
    sender->sendMessage(m);
    
    m.clear();
    sprintf(address, "/myo-music/%u/pose/waveOut", id);
    m.setAddress(address);
    m.addIntArg(waveOut);
    sender->sendMessage(m);
    
    m.clear();
    sprintf(address, "/myo-music/%u/pose/thumbToPinky", id);
    m.setAddress(address);
    m.addIntArg(thumbToPinky);
    sender->sendMessage(m);
    
}

void MyoDataCollector::sendOrientation(myo::Myo* myo, float roll, float pitch, float yaw) {
    char message[100];
    unsigned int id = identifyMyo(myo);
    
    {
        sprintf(message, "/myo-music/%u/position/roll", id);
        
        ofxOscMessage m;
        m.setAddress(message);
        m.addFloatArg(roll);
        sender->sendMessage(m);
    }
    {
        sprintf(message, "/myo-music/%u/position/pitch", id);
        
        ofxOscMessage m;
        m.setAddress(message);
        m.addFloatArg(pitch);
        sender->sendMessage(m);
    }
    {
        sprintf(message, "/myo-music/%u/position/yaw", id);
        
        ofxOscMessage m;
        m.setAddress(message);
        m.addFloatArg(yaw);
        sender->sendMessage(m);
    }
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

