#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFullscreen(false);
    ofSetWindowPosition(0,0);
    ofSetWindowShape(1280,800);
    //ofSetFullscreen(false);
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofEnableDepthTest();
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    
#ifdef USING_MYO
    //
    // MYO stuff
    //
    collector.setup();
    
    currentPose = myo::Pose::unknown;
    lastPose = myo::Pose::unknown;
    poseChanged = false;
    
    hub = new myo::Hub("com.wearhacks.myo-music");
    
    std::cout << "Attempting to find a Myo..." << std::endl;
    myo = hub->waitForMyo(10000);
    if (!myo) {
        throw std::runtime_error("Unable to find a Myo!");
    }
    
    // We've found a Myo.
    std::cout << "Connected to a Myo armband!" << std::endl << std::endl;
    hub->addListener(&collector);
#endif
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    ofxBlur.setup(ofGetWidth(), ofGetHeight(), 8, .2, 1);
    
    //
    // Visual stuff
    //
    for (int q=0 ; q<7 ; q++) {
        quads.push_back(new Quad());
        quads[q]->setup();
        quads[q]->setImageIdx(q);
    }
    quads[quads.size()-1]->setVideoIdx(0);
    
    // 5 4 3 2 1 0
    activeQuad = 0;
    for (int q=0 ; q<activeQuad ; q++)
        quads[q]->setRight();
    
    for (int q=quads.size()-1 ; q>activeQuad ; q--)
        quads[q]->setLeft();
    
    /*
    quads[0]->setRight();
    quads[1]->setRight();
    quads[3]->setLeft();
    quads[4]->setLeft();
    activeQuad = 2;
    */
    
    ofBackground(0);
}

//--------------------------------------------------------------
void ofApp::update(){
    
#ifdef USING_MYO
    hub->run(1000/20);
    collector.print();
    
    currentPose = collector.currentPose;
    if (currentPose != lastPose) {
        ofLog() << "New pose!";
        lastPose = currentPose;
        poseChanged = true;
    }
    
    if (poseChanged) {
        
        if (currentPose == myo::Pose::waveOut) {
            triggerRight();
            myo->vibrate(myo::Myo::vibrationShort);
        } else if (currentPose == myo::Pose::waveIn) {
            triggerLeft();
            myo->vibrate(myo::Myo::vibrationShort);
        }
        
    }
    poseChanged = false;
    
#endif
    
    for (int q=0 ; q<quads.size() ; q++) {
        quads[q]->update();
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofxBlur.begin();
    ofClear(0,0,0,255);
    
#ifdef USING_MYO
    /*
    ofSetColor((int)collector.roll_w/18.0*255.0,
               (int)collector.pitch_w/18.0*255.0,
               (int)collector.yaw_w/18.0*255.0);
    //ofSetColor(255,125,125);
    ofFill();
    ofRect(0,0,1024,768);
    */
#endif
    
    for (int q=0 ; q<quads.size() ; q++) {
        quads[q]->draw();
    }
    
    ofxBlur.end();
    
    ofxBlur.draw();
}

void ofApp::triggerLeft() {
    ofLogVerbose() << "activeQuad = " << activeQuad;
    if (activeQuad > 0) {
        
        if (quads[activeQuad]->isAnimated() || quads[activeQuad-1]->isAnimated())
            return;
        
        ofLogVerbose() << "\t animating LEFT quads " << activeQuad << " and " << activeQuad-1;
        ofLogVerbose() << "\t\t quad " << activeQuad-1 << " has state " << quads[activeQuad-1]->getState();
        ofLogVerbose() << "\t\t quad " << activeQuad << " has state " << quads[activeQuad]->getState();
        
        bool result = true;
        result &= quads[activeQuad]->animateLeft();
        result &= quads[activeQuad-1]->animateLeft();
        
        if (result)
            activeQuad--;
    }
}

void ofApp::triggerRight() {
    ofLogVerbose() << "activeQuad = " << activeQuad;
    if (activeQuad < quads.size()-1) {
        
        if (quads[activeQuad]->isAnimated() || quads[activeQuad+1]->isAnimated())
            return;
        
        ofLogVerbose() << "\t animating RIGHT quads " << activeQuad << " and " << activeQuad+1;
        ofLogVerbose() << "\t\t quad " << activeQuad << " has state " << quads[activeQuad]->getState();
        ofLogVerbose() << "\t\t quad " << activeQuad+1 << " has state " << quads[activeQuad+1]->getState();
        
        bool result = true;
        result &= quads[activeQuad]->animateRight();
        result &= quads[activeQuad+1]->animateRight();
        
        if (result);
            activeQuad++;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    switch(key) {
        // 4 3 2 1 0
        case 'l':
            triggerLeft();
            break;

        case 'r':
            triggerRight();
            break;
    }
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


