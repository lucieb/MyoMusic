#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(60);
	ofSetVerticalSync(TRUE);
	ofSetBackgroundAuto(false);
	
	ofxKeyframeAnimRegisterEvents(this);
	
	for (int i=0; i<20; i++) {
		animatedRectangles.push_back(new AnimatedRectangle());
		ofxKeyframeAnimRegisterEvents(animatedRectangles[i]);
	}	
	
}

//--------------------------------------------------------------
void testApp::update(){
	vector<AnimatedRectangle *>::iterator it = animatedRectangles.begin();
	
	for (int i=0; i<20; i++) {
		animatedRectangles[i]->playlist.update();	
	}	

}

void testApp::onKeyframe(ofxPlaylistEventArgs& args){

	// this check is only necessary if you want to be absolutely sure that 
	// the onKeyFrame Event was sent by the same object as the receiver.
	if (args.pSender != static_cast<void*>(this)) return;
	
	ofLog(OF_LOG_VERBOSE) << "Keyframe Event received for (" << args.pSender << "): " << args.message << ": " << ofGetFrameNum();

}

//--------------------------------------------------------------
void testApp::draw(){
	

	ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
		ofSetColor(0, 5);
		ofFill();
		ofRect(0,0,ofGetWidth(),ofGetHeight());
	ofPopStyle();

	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	
	for (int i=0; i<animatedRectangles.size(); i++){
		animatedRectangles[i]->draw();
	}
	
	ofPushStyle();
		ofDisableBlendMode();
		ofSetColor(0,255);
		ofRect(ofGetWidth()/2. - 4, ofGetHeight()/2. - 12, 120, 20);
		ofFill();	
		ofSetColor(255);
		ofDrawBitmapString(ofToString(ofGetFrameNum()) + ":" + ofToString(ofGetFrameRate()),ofPoint(ofGetWidth()/2.,ofGetHeight()/2.));
	ofPopStyle();
	

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

	using namespace Playlist;
	int index = int(ofRandom(animatedRectangles.size()));
	

	AnimatedRectangle * pRect = animatedRectangles[index];
	
	pRect->playlist.addKeyFrame(Action::event(pRect,"START1"));
	pRect->playlist.addKeyFrame(Action::tween(100, &pRect->pos.y, ofRandomuf()*ofGetHeight(), TWEEN_QUAD, TWEEN_EASE_OUT));
	pRect->playlist.addToKeyFrame(Action::tween(50,50, &pRect->pos.x, ofRandomuf()*ofGetWidth(), TWEEN_QUAD, TWEEN_EASE_OUT));

	// addToKeyframe will add the Keyframe so that it is executed in paralell with the last added Keyframe
	// note that this tween will be delayed by 50 frames and will then execute for another 50 frames, 
	// thus running as long as the first keyframe in the playlist.
	
	
	pRect->playlist.addKeyFrame(Action::event(this,"END1"));

	// chain syntax example
	pRect->playlist.addKeyFrame(Action::event(this,"START2"))
				   .addKeyFrame(Action::event(this,"START3"))
				   .addKeyFrame(Action::pause(0));

	pRect->playlist.addKeyFrame(Action::event(this,"START4"));

	// let's use funky bezier curves!
	BezierTween * eC = new BezierTween();

	// the values for InSpeed, OutSpeed, InInfluence and OutInfluence map 1:1 to an After Effects Bezier curve.
	// inspect an After Effects timeline to get a visual impression of how they influence the bezier curve.
	
	eC->setInSpeed(120);			// you set the speed in units / second 
	eC->setInInfluence(.10);		// influence is a normalized float (0..1)

	eC->setOutSpeed(8700.0);
	eC->setOutInfluence(0.002);

	pRect->playlist.addKeyFrame(Action::tween(100, &pRect->angle,   ofRandomuf()*360, ofPtr<BezierTween>(eC)));
	pRect->playlist.addKeyFrame(Action::event(this,"END"));

	ofLog(OF_LOG_VERBOSE) << "Duration: " << animatedRectangles[index]->playlist.duration;
	

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}