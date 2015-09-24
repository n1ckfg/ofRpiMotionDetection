#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(127,127,127);
//	ofSetFrameRate(100);

	video.initGrabber(320, 240, true);
    video.setDeviceID(0);
}

//--------------------------------------------------------------
void testApp::update(){

    video.update();
    if (video.isFrameNew())
        video.grabFrame();

    printf("frame %d, fps %.2f\n", ofGetFrameNum(), ofGetFrameRate());
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(0xffffff);

	video.draw(20, 20);

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){
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
void testApp::mouseReleased(){

}
