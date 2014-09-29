#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <thread>

#include <myo/myo.hpp>
#include "include/RtWvOut.h"
#include "include/SineWave.h"
#include "include/BlitSquare.h"
#include "include/BlitSaw.h"
#include "include/SingWave.h"
#include "include/Chorus.h"
#include "include/Filter.h"
#include "include/Fir.h"
#include "include/Rhodey.h"
#include "include/FMVoices.h"
#include "include/TubeBell.h"

using namespace stk;

class DataCollector : public myo::DeviceListener {
public:
    DataCollector()
    : onArm(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose() {
    }
    
    void onUnpair(myo::Myo* myo, uint64_t timestamp) {
        roll_w = 0;
        pitch_w = 0;
        yaw_w = 0;
        onArm = false;
    }

    void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat) {
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
        roll_w = (roll + (float)M_PI)/(M_PI * 2.0f);
        pitch_w = (pitch + (float)M_PI/2.0f)/M_PI;
        yaw_w = (yaw + (float)M_PI)/(M_PI * 2.0f);
    }

    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose) {
        currentPose = pose;
    }
    
    void onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection) {
        onArm = true;
        whichArm = arm;
    }

    void onArmLost(myo::Myo* myo, uint64_t timestamp) {
        onArm = false;
    }
    
    void initStk() {
        Stk::setSampleRate(44100.0);
        Stk::showWarnings(true);
        try {
            dac = new RtWvOut(1);
        } catch (StkError &) {
            exit(1);
        }
        sine.setFrequency(441.0);
    }
    
    void play() {
        if (yaw_w < last_yaw - 0.01 || yaw_w > last_yaw + 0.01) {
            sine.setFrequency(500.0 - yaw_w * 400);
            last_yaw = yaw_w;
        }
        
        if (pitch_w > 0.5 && !notePlaying) {
            notePlaying = true;
        } else if (pitch_w < 0.5 && notePlaying) {
            notePlaying = false;
        }
    }

    bool onArm;
    myo::Arm whichArm;

    float roll_w;
    float pitch_w;
    float yaw_w;
    float last_roll = 0;
    float last_pitch = 0;
    float last_yaw = 0;
    myo::Pose currentPose;
    
    BlitSaw sine;
    Fir filter;
    RtWvOut *dac = 0;
    
    bool notePlaying = false;
};

/*int main(int argc, char** argv) {
    try {
        myo::Hub hub("com.myomusic.MyoMusic");
        myo::Myo* myo = hub.waitForMyo(10000);

        if (!myo) {
            throw std::runtime_error("Unable to find a Myo!");
        }

        std::cout << "Connected to a Myo armband!" << std::endl;

        DataCollector collector;
        hub.addListener(&collector);
        
        collector.initStk();
        
        while (true) {
            hub.run(1000 / 20);
            collector.play();
            StkFrames frames(2300, 1);
            if (collector.notePlaying) {
                collector.dac->tick(collector.sine.tick(frames));
            } else {
                collector.dac->tick(frames);
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
        return 1;
    }
}*/
