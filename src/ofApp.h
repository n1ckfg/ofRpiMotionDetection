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
#include "ofxOpticalFlowFarneback.h"

#define NUM_MSG_STRINGS 20
#define OSC_PORT 8001

//using namespace ofxCv;
//using namespace cv;


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
	
		// listeners
		void fpsChanged(int &fps);
		void learningTimeChanged(int &learningTime);
		void backgroundThresholdChanged(int &backgroundThreshold);
		void useOpticalFlowChanged(bool &useOpticalFlow);
		void roiXChanged(float &roiX);
		void roiYChanged(float &roiY);
		void roiWChanged(float &roiW);
		void roiHChanged(float &roiH);
		void exposureCompensationChanged(int &exposureCompensation);
		void exposureMeteringModeChanged(int &exposureMeteringMode);
		void exposureModeChanged(int &exposureMode);
		void awbModeChanged(int &awbMode);
		void flickerAvoidChanged(int &flickerAvoid);
		void shutterSpeedChanged(int &shutterSpeed);
	
		// program ID
		string RPiId;

	//Console
		ConsoleListener consoleListener;
		void onCharacterReceived(SSHKeyListenerEventData& e);
	
//    private:
		int W, H;
		int framenr;
	
#ifdef __arm__
		ofxCvPiCam cam;
#endif
	
		// OSC
		ofxOscSender sender;
		ofxOscReceiver receiver;
	
		// Cv
		cv::Mat matInput;
		cv::Mat matToProcess;
		cv::Mat matAccum;

		ofPixels tosave, pixProcessed;
		ofImage thresholded, imageSequence, processed;
		ofxCv::RunningBackground		background;
		ofxCvGrayscaleImage				toContours;
		ofxCvContourFinder				contourFinder;
		int sent_blobs;

	
		// Optical flow
		ofxOpticalFlowFarneback flowFB;
		ofPixels flowPix;
		ofxCvGrayscaleImage flowMat;
	
		ofParameter<float> fbPyrScale, lkQualityLevel, fbPolySigma;
		ofParameter<int> fbLevels, lkWinSize, fbIterations, fbPolyN, fbWinSize, lkMaxLevel, lkMaxFeatures, lkMinDistance;
		ofParameter<bool> boolDraw, fbUseGaussian, usefb, useOpticalFlow;

//	ofxCv::FlowFarneback fb;
//	ofxCv::FlowPyrLK lk;
//	ofxCv::Flow* curFlow;
	
		// Settings
		ofxPanel gui;
		ofParameter<int> cutDown;
		ofParameter<int> fps;
		ofParameter<int> learningTime;
		ofParameter<float> accumFactor;
		ofParameter<int> backgroundThreshold;
		ofParameter<int> erodeFactor;
		ofParameter<int> dilateFactor;
		ofParameter<int> medianBlurFactor;
		ofParameter<int> minContourArea;
		ofParameter<int> maxContourArea;
		ofParameter<int> maxContours;
		ofParameter<bool> useAccum;
		string filename_save;
	
		ofRectangle ROI;
		ofParameter<float> roiX;
        ofParameter<float> roiY;
        ofParameter<float> roiW;
        ofParameter<float> roiH;
        ofParameter<int>  exposureCompensation;
        ofParameter<int> exposureMeteringMode;
        ofParameter<int> exposureMode;
        ofParameter<int> awbMode;
        ofParameter<int> shutterSpeed;
        ofParameter<int> flickerAvoid;
        int exposureMeteringModeInt;
        int exposureModeInt;
		int awbModeInt;
		int flickerAvoidInt;
	
	//		string exposureModes[14];
	//		string exposureMeteringModes[5];
	//		string awbModes[11];
	//		string flickerAvoids[11];

};

