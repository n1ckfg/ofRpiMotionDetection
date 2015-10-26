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
#define OSC_PORT 8001

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
		void fpsChanged(int & fps);
		void learningTimeChanged(int &learningTime);
		void backgroundThresholdChanged(int &backgroundThreshold);

		//experimenting

		void roiXChanged(float &value);
		void roiYChanged(float &value);
		void roiWChanged(float &value);
		void roiHChanged(float &value);
		void exposureCompensationChanged(int &value);
		void exposureMeteringModeChanged(int &value);
		void exposureModeChanged(int &value);
		void awbModeChanged(int &value);
		void shutterSpeedChanged(int &value);
		// program ID
		string RPiId;
	
    private:
	
#ifdef __arm__
		ofxCvPiCam cam;
#else
		ofVideoGrabber cam;
#endif
	
		// OSC
		ofxOscSender sender;
		ofxOscReceiver receiver;
	
		// Cv
		ofPixels pix;
		int sent_blobs;
		ofImage thresholded;
		Mat frame,frameProcessed;
		ofxCv::RunningBackground background;
		void onCharacterReceived(SSHKeyListenerEventData& e);
		ofxCvContourFinder 	contourFinder;
		ofxCvGrayscaleImage    grayImage;
	    ofImage image;
		ofPixels tosave;
	
		// Settings
		ofxPanel gui, guiXtra;
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
		string filename_save;
	
		//experimenting
		ofRectangle ROI;
		string exposureModes[14];
		string exposureMeteringModes[5];
		string awbModes[11];
        ofParameter<float> roiX;
        ofParameter<float> roiY;
        ofParameter<float> roiW;
        ofParameter<float> roiH;
        ofParameter<int>  exposureCompensation;
        ofParameter<int> exposureMeteringMode;
        ofParameter<int> exposureMode;
        ofParameter<int> awbMode;
        ofParameter<int> shutterSpeed;
		//experimenting end
		ConsoleListener consoleListener;
};

