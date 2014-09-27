#include "ofApp.h"
#include "ofxOsc.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    sender = new ofxOscSender();
    sender->setup(HOST, PORT);
    
    // First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
    // publishing your application. The Hub provides access to one or more Myos.
    hub = new myo::Hub("com.wearhacks.myo-music");
    
    std::cout << "Attempting to find a Myo..." << std::endl;
    
    // Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
    // immediately.
    // waitForAnyMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
    // if that fails, the function will return a null pointer.
    myo = hub->waitForMyo(10000);
    
    // If waitForAnyMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
    if (!myo) {
        throw std::runtime_error("Unable to find a Myo!");
    }
    
    // We've found a Myo.
    std::cout << "Connected to a Myo armband!" << std::endl << std::endl;
    
    // Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
    //DataCollector collector;
    
    // Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
    // Hub::run() to send events to all registered device listeners.
    hub->addListener(&collector);
}

//--------------------------------------------------------------
void ofApp::update(){
    //myo::Hub hub("com.wearhacks.myo-music");
    // In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
    // In this case, we wish to update our display 20 times a second, so we run for 1000/20 milliseconds.
    hub->run(1000/20);
    // After processing events, we call the print() member function we defined above to print out the values we've
    // obtained from any events that have occurred.
    collector.print();
    
    {
        ofxOscMessage m;
        m.setAddress("/myo-music/position/roll");
        m.addFloatArg(collector.roll_w/18.0*100.0);
        sender->sendMessage(m);
    }
    {
        ofxOscMessage m;
        m.setAddress("/myo-music/position/pitch");
        m.addFloatArg(collector.pitch_w/18.0*100.0);
        sender->sendMessage(m);
    }
    {
        ofxOscMessage m;
        m.setAddress("/myo-music/position/yaw");
        m.addFloatArg(collector.yaw_w/18.0*100.0);
        sender->sendMessage(m);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    //collector.roll_w
    
    ofSetColor((int)collector.roll_w/18.0*255.0,
               (int)collector.pitch_w/18.0*255.0,
               (int)collector.yaw_w/18.0*255.0);
    //ofSetColor(255,125,125);
    ofFill();
    ofRect(0,0,1024,768);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


