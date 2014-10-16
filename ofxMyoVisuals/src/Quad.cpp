//
//  Quad.cpp
//  ofxMyoVisuals
//
//  Created by Lucie Bélanger on 2014-09-27.
//
//

#include "Quad.h"

Quad::Quad() :
    color(165,105,180,125),
    pos(0,0,0), rot(0,0,0), rotAxis(0,0,0),
    scale(1.0), state(QuadCenter), animating(false),
    videoIdx(-1), imageIdx(0) {
    
        for (int i=1 ; i<8 ; i++) {
            imageFilenames.push_back(ofToDataPath("presentation/wawTO/Slide"+ofToString(i)+".jpg"));
        }
        
        videoFilenames.push_back(ofToDataPath("videos/glitchMT-HD.mov"));
        videoFilenames.push_back(ofToDataPath("videos/harvest-SD.mp4"));
        videoFilenames.push_back(ofToDataPath("videos/kwirllars-SD.mp4"));
        videoFilenames.push_back(ofToDataPath("videos/shifting-SD.mp4"));
        videoFilenames.push_back(ofToDataPath("videos/subspace-SD.mp4"));
}

Quad::~Quad() {

}

void Quad::setup() {
    
    imageIdx = 0;
    videoIdx = -1;
    
    pos.z = 0;
    rot.y = 0;
    
    kViewAnimLStarted = "animL_started_" + ofToString(ofGetElapsedTimeMillis());
    kViewAnimLEnded = "animL_ended_" + ofToString(ofGetElapsedTimeMillis());
    kViewAnimRStarted = "animR_started_" + ofToString(ofGetElapsedTimeMillis());
    kViewAnimREnded = "animR_ended_" + ofToString(ofGetElapsedTimeMillis());
    
    if (videoIdx >= 0) {
        video.loadMovie(videoFilenames[videoIdx]);
        video.setUseTexture(true);
        video.setLoopState(OF_LOOP_NORMAL);
        video.setVolume(0.0);
    }
    
    if (imageIdx >= 0) {
        image.loadImage(imageFilenames[imageIdx]);
        image.setUseTexture(true);
    }
    
    ofxKeyframeAnimRegisterEvents(this);
    
}

void Quad::update() {
    
    if (videoIdx >= 0 && state == QuadCenter) {
        if (!video.isPaused()) {
            if (!video.isPlaying())
                video.play();
            video.update();
        }
    }
    playlist.update();
}

void Quad::draw() {
    
    color.a = 255-abs(rot.y/75.0*255.0);
    
    ofPushStyle();
    ofPushMatrix();
    {
        ofTranslate(rotAxis);
        
        ofTranslate(pos.x,pos.y,pos.z);
        ofRotate(rot.x,1,0,0);
        ofRotate(rot.y,0,1,0);
        ofRotate(rot.z,0,0,1);
        
        ofTranslate(-rotAxis.x, rotAxis.y, rotAxis.z);
        
        if (state == QuadCenter) {
            ofTranslate(ofGetWidth()/2.0,ofGetHeight()/2.0,0);
            ofScale(scale, scale, 1.0);
            ofTranslate(-ofGetWidth()/2.0, -ofGetHeight()/2.0,0);
        }
        
        int w = ofGetWindowWidth();
        int h = ofGetWindowHeight();
        
        int tw;// = video.getWidth();
        int th;// = video.getHeight();
        
        glColor4f(color.r/255.0, color.g/255.0, color.b/255.0, color.a/255.0);
        
        if (videoIdx >= 0) {
            video.getTextureReference().bind();
            tw = video.getWidth();
            th = video.getHeight();
        } else if (imageIdx >= 0) {
            image.getTextureReference().bind();
            tw = image.getWidth();
            th = image.getHeight();
        } else {
            glColor4f(color.r/255.0, color.g/255.0, color.b/255.0, color.a/255.0);
        }
        
        glBegin(GL_POLYGON);
        {
            glTexCoord2f(0.0, 0.0);
            glVertex2f(0, 0);
            
            glTexCoord2f(tw, 0.0);
            glVertex2f(w, 0);
            
            glTexCoord2f(tw, th);
            glVertex2f(w, h);
            
            glTexCoord2f(0.0, th);
            glVertex2f(0, h);
        }
        glEnd();
        
        if (videoIdx >= 0)
            video.getTextureReference().unbind();
        else if (imageIdx >= 0)
            image.getTextureReference().unbind();
        
    }
    ofPopMatrix();
    ofPopStyle();
}


void Quad::setVideoIdx(int idx) {
    videoIdx = idx%videoFilenames.size();
    imageIdx = -1;
    
    if (videoIdx >= 0) {
        video.loadMovie(videoFilenames[videoIdx]);
        video.setUseTexture(true);
        video.setLoopState(OF_LOOP_NORMAL);
        video.setVolume(0.0);
        video.play();
        video.update();
        if (state != QuadCenter)
            video.setPaused(true);
    }
}

void Quad::setImageIdx(int idx) {
    imageIdx = idx%imageFilenames.size();
    videoIdx = -1;
    
    if (imageIdx >= 0) {
        image.loadImage(imageFilenames[imageIdx]);
        image.setUseTexture(true);
    }
}

void Quad::setLeft() {
    rot.y = 75;
    state = QuadLeft;
    rotAxis.x = 0;
    video.setPaused(true);
}

void Quad::setRight() {
    rot.y = -75;
    state = QuadRight;
    rotAxis.x = ofGetWidth();
    video.setPaused(true);
}

#pragma mark -
#pragma mark ofxPlaylist callback listener
//--------------------------------------------------------------
void Quad::onKeyframe(ofxPlaylistEventArgs& args)
{
    
    // this check is only necessary if you want to be absolutely sure that
    // the onKeyFrame Event was sent by the same object as the receiver.
	if (args.pSender != static_cast<void*>(this)) return;
    
    if(args.message == kViewAnimLStarted) {
        animating = true;
    }
    else if(args.message == kViewAnimLEnded) {
        animating = false;
        
        if (state == QuadRight) {
            state = QuadCenter;
            if (videoIdx >= 0)
                video.setPaused(false);
            
        } else if (state == QuadCenter) {
            state = QuadLeft;
            if (videoIdx >= 0)
                video.setPaused(true);
        }
    }
    else if(args.message == kViewAnimRStarted) {
        animating = true;
    }
    else if(args.message == kViewAnimREnded) {
        animating = false;
        
        if (state == QuadLeft) {
            state = QuadCenter;
            if (videoIdx >= 0)
                video.setPaused(false);
            
        } else if (state == QuadCenter) {
            state = QuadRight;
            if (videoIdx >= 0)
                video.setPaused(true);
        }
    }
}

bool Quad::animateLeft() {
    
    if (animating) return false;
    
    using namespace Playlist;
    playlist.addKeyFrame(Action::event(this,kViewAnimLStarted));
    
    if (state == QuadCenter) {
        rotAxis.x = 0;
        playlist.addKeyFrame(Action::tween(1000.0f, &rot.y, 75.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &pos.x, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &pos.y, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &pos.z, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &scale, 0.5));
    }
    else if (state == QuadRight) {
        rotAxis.x = ofGetWidth();
        playlist.addKeyFrame(Action::pause(400.0f));
        playlist.addKeyFrame(Action::tween(1000.0f, &rot.y, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &pos.x, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &pos.y, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &pos.z, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &scale, 1.0));
    }
    
    playlist.addKeyFrame(Action::event(this,kViewAnimLEnded));
    
    return true;
}

bool Quad::animateRight() {
    
    if (animating) return false;
    
    using namespace Playlist;
    playlist.addKeyFrame(Action::event(this,kViewAnimRStarted));
    
    if (state == QuadCenter) {
        rotAxis.x = ofGetWidth();
        playlist.addKeyFrame(Action::tween(1000.0f, &rot.y, -75.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &pos.x, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &pos.y, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &pos.z, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &scale, 0.5));
    }
    else if (state == QuadLeft) {
        rotAxis.x = 0;
        playlist.addKeyFrame(Action::pause(400.0f));
        playlist.addKeyFrame(Action::tween(1000.0f, &rot.y, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &pos.x, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &pos.y, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &pos.z, 0.0f, TWEEN_QUAD, TWEEN_EASE_OUT));
        playlist.addToKeyFrame(Action::tween(1000.0f, &scale, 1.0));
    }
    
    playlist.addKeyFrame(Action::event(this,kViewAnimREnded));
    
    return true;
}