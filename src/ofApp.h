#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#ifdef __arm__
#include "ofxCvPiCam.h"
#endif
#include "ConsoleListener.h"
#include "ofxOsc.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"

#define NUM_MSG_STRINGS 20
#define OSC_PORT 8005

using namespace ofxCv;
using namespace cv;


class ofApp : public ofBaseApp, public SSHKeyListener{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void fpsChanged(int & fps);
		void learningTimeChanged(int &learningTime);
		void backgroundThresholdChanged(int &backgroundThreshold);
		string RPiId;

        int thresh;
    private:
#ifdef __arm__
        ofxCvPiCam cam;
#else
        ofVideoGrabber cam;
#endif
        ofxOscSender sender;
        ofxOscReceiver receiver;

        int current_msg_string;
		string msg_strings[NUM_MSG_STRINGS];
		float timers[NUM_MSG_STRINGS];

        ofImage thresholded;
        Mat frame,frameProcessed;
        ofxCv::RunningBackground background;
        ConsoleListener consoleListener;
        void onCharacterReceived(SSHKeyListenerEventData& e);
        ofxCvContourFinder 	contourFinder;
        ofxCvGrayscaleImage    grayImage;

        string filename_save;
        //parameters
        ofParameter<int> cutDown;
		ofParameter<int> fps;
		ofParameter<int> learningTime;
		ofParameter<int> backgroundThreshold;
		ofParameter<int> erodeFactor;
		ofParameter<int> dilateFactor;
		ofParameter<int> medianBlurFactor;
		ofParameter<int> minContourArea;
		ofParameter<int> maxContourArea;
		ofParameter<int> maxContours;

		ofxPanel gui;

		ofPixels tosave;
};

