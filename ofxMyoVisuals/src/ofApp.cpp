#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    //ofSetFullscreen(true);
    ofSetWindowPosition(0,0);
    ofSetWindowShape(1024,768);
    ofSetFullscreen(true);
    
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
    ofxBlur.setup(ofGetWidth(), ofGetHeight(), 16, .2, 1);
    
    //
    // Visual stuff
    //
    for (int q=0 ; q<9 ; q++) {
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
    //collector.print();
    
    currentPose = collector.currentPose;
    if (currentPose != lastPose) {
        ofLog() << "New pose!";
        lastPose = currentPose;
        poseChanged = true;
    }
    
    if (poseChanged) {
        
        if (currentPose == myo::Pose::waveOut) {
            if (activeQuad < quads.size()-1) {
                triggerRight();
                myo->vibrate(myo::Myo::vibrationShort);
            }
        } else if (currentPose == myo::Pose::waveIn) {
            if (activeQuad < quads.size()-1) {
                triggerLeft();
                myo->vibrate(myo::Myo::vibrationShort);
            }
        } else if (currentPose == myo::Pose::fist) {
            sOrientation.x = collector.yaw_f;
            sOrientation.y = collector.pitch_f;
            sOrientation.z = 0;
        }
        
    }
    poseChanged = false;
    
#endif
    
    for (int q=0 ; q<quads.size() ; q++) {
#ifdef USING_MYO
        
        if (currentPose == myo::Pose::fist) {
            
            ofVec3f diff;
            diff.x = collector.yaw_f - sOrientation.x;
            diff.y = collector.pitch_f - sOrientation.y;
            diff.z = 0;
            
            quads[q]->pos.x = (-diff.x/20.0)*ofGetWidth()/3.0;
            quads[q]->pos.y = (diff.y/20.0)*ofGetHeight()/3.0;
            
            if (activeQuad < quads.size()-1) {
                // not video time!!!
                quads[q]->color.r = 255;
                quads[q]->color.g = 255;
                quads[q]->color.b = 255;
            } else {
                int s = 255; int l = 200;
                quads[q]->color.setHue(collector.pitch_f/20.0*255.0);
                quads[q]->color.setSaturation(s);
                quads[q]->color.setBrightness(l);
                
            }
            
            
            //ofxBlur.setScale(2.0);
            
        } else {
            
            int min = 188/360.0*255.0;
            int max = 310/360.0*255.0;
            int s = 125; int l = 125;
            
            if (activeQuad == quads.size()-1) {
                // video time!!!
                quads[q]->color.setHue(collector.yaw_f/20.0*255.0);
                quads[q]->color.setSaturation(s);
                quads[q]->color.setBrightness(l);
            }
            else {
                quads[q]->color.setHue(min);
                quads[q]->color.setSaturation(collector.yaw_f/20.0*255.0-125);
                quads[q]->color.setBrightness(collector.pitch_w/20.0*125.0+125);
            }
            
            //quads[q]->color.r = (int)(collector.pitch_w/20.0*125.0+75);
            //quads[q]->color.g = (int)(collector.roll_w/20.0*125.0+75);
            //quads[q]->color.b = (int)(collector.yaw_w/20.0*125.0+75);
            
            quads[q]->pos.x = 0;
            quads[q]->pos.y = 0;
            quads[q]->pos.z = 0;
            
            //ofxBlur.setScale(0.0);
        }
#endif
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


