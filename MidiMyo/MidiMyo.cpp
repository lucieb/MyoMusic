#include <map>

#include "/usr/local/include/RtMidi.h"
#include <myo/myo.hpp>

class MyoData {
public:
    float roll_w = 0;
    float pitch_w = 0;
    float yaw_w = 0;
    
    bool keyOn = false;
    unsigned char notePlayed = 0;
    float magnitude = 0;
};

class MidiMyo : public myo::DeviceListener {
public:
    MidiMyo() {
    }

    void onOrientationData(myo::Myo* myo,
                           uint64_t timestamp,
                           const myo::Quaternion<float>& quat) {
        using std::atan2;
        using std::asin;
        using std::sqrt;
        
        MyoData &data = myoMap[myo];
        
        float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                           1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
        float pitch = asin(2.0f * (quat.w() * quat.y() - quat.z() * quat.x()));
        float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                          1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
        
        data.roll_w = (roll + (float)M_PI)/(M_PI * 2.0f);
        data.pitch_w = (pitch + (float)M_PI/2.0f)/M_PI;
        data.yaw_w = (yaw + (float)M_PI)/(M_PI * 2.0f);
        
        if (!data.keyOn && data.pitch_w > 0.5) {
            data.keyOn = true;
            data.notePlayed = 64 + data.yaw_w * 6;
            noteOn(data.notePlayed, data.magnitude / 1500 * 127);
        } else if (data.keyOn && data.pitch_w < 0.5) {
            data.keyOn = false;
            noteOff(data.notePlayed, 0);
        }
        
    }
    
    void onPose(myo::Myo *myo,
                uint64_t timestamp,
                const myo::Pose pose) {
    }
    
    void onGyroscopeData(myo::Myo *myo,
                         uint64_t timestamp,
                         const myo::Vector3<float> &gyro) {
        MyoData &data = myoMap[myo];
        
        data.magnitude = gyro.magnitude();
        if (data.magnitude > 1500) {
            data.magnitude = 1500;
        }
    }
    
    void init() {
        out->openVirtualPort();
    }
    
    void msg(unsigned char stat,
             unsigned char note,
             unsigned char vel) {
        std::vector<unsigned char> msg;
        msg.push_back(stat);
        msg.push_back(note);
        msg.push_back(vel);
        out->sendMessage(&msg);
    }
    
    void noteOn(unsigned char note,
                unsigned char vel) {
        msg(144, note, vel);
    }
    
    void noteOff(unsigned char note,
                 unsigned char vel) {
        msg(128, note, 0);
    }
    
    void trigger(unsigned char note,
                 unsigned char vel) {
        noteOn(note, vel);
        noteOff(note, vel);
    }
    
    RtMidiOut *out = new RtMidiOut();
    std::map<myo::Myo*, MyoData> myoMap;
};

int main(int argc, char** argv) {
    myo::Hub hub("com.myomusic.MyoMusic");
    hub.waitForMyo(10000);
    MidiMyo m;
    hub.addListener(&m);
    
    m.init();
    
    while (true) {
        hub.run(100);
    }
}
