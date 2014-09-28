#ifndef GUARD_ANIMATED_RECTANGLE
#define	GUARD_ANIMATED_RECTANGLE

/*
 *  animated_rectangle.h
 *  
 *
 *  Created by Timothy Gfrerer on 02/03/2011.
 *  Copyright 2011. All rights reserved.
 *
 */

#include "ofxPlaylist.h"


class AnimatedRectangle {
public:
	ofxPlaylist playlist;
	
	AnimatedRectangle(){		
		pos=ofVec3f((ofGetWidth()-120)*ofRandomuf(),(ofGetHeight()-80)*ofRandomuf() ,0);
		size=ofVec2f(120,80);
		color=ofFloatColor(ofRandomuf() * 255.,ofRandomuf() * 255., ofRandomuf() * 255., ofRandomuf() * 255.);
		angle = ofRandomuf() * 360.;
	};
	~AnimatedRectangle(){
		// be nice and clean up after yourself.
		playlist.clear();
	};
	
	void draw(){
		glPushMatrix();{

			glTranslatef(pos.x,pos.y,pos.z);
			glRotatef((angle+360), 0, 0, 1);
			glTranslatef(size.x/2.f, size.y/2.f, 0);
			ofPushStyle();{
				ofFill();
				glColor4f(color.r/255., color.g/255., color.b/255., color.a/255.);
				ofRect(ofVec3f(0),size.x,size.y);
				
			}
			ofPopStyle();
		}
		glPopMatrix();
	}

	void onKeyframe(ofxPlaylistEventArgs& args){
		
		// this check is only necessary if you want to be absolutely sure that 
		// the onKeyFrame Event was sent by the same object as the receiver.
		if (args.pSender != static_cast<void*>(this)) return;
		
		ofLog(OF_LOG_VERBOSE) << "Animated Rectangle: Keyframe Event received for (" << args.pSender << "): " << args.message << ": " << ofGetFrameNum();
		
	}
	
	float angle;
	ofFloatColor color;
	ofVec3f pos;
	ofVec2f	size;
};




#endif