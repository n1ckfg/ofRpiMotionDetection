#include "ofApp.h"
#include <opencv2/opencv.hpp>


using namespace ofxCv;
using namespace cv;

void ofApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
	keyPressed((int)e.character);
}
//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(fps);
    ofLog() << "RPid " << RPiId;
	framenr = 1;
	
	gui.setup("panel");
    gui.setPosition(650,10);
	
	// openCV params
    gui.add(cutDown.set( "cutDown", 110, 1, 255 ));
    gui.add(fps.set("fps", 15, 1, 30));
    gui.add(learningTime.set("learningTime",100,1,1000));
	gui.add(backgroundThreshold.set("backgroundThreshold",10,1,300));
    gui.add(erodeFactor.set("erodeFactor",1,0,3));
    gui.add(dilateFactor.set("dilateFactor",2,0,3));
    gui.add(medianBlurFactor.set("medianBlur",3,0,5));
    gui.add(minContourArea.set("minContourArea",20, 5, 40));
    gui.add(maxContourArea.set("maxContourArea", 300, 40, 160*120)); // one third of the screen.
    gui.add(maxContours.set("maxContours", 5, 1, 10));
    gui.add(accumFactor.set("accumFactor",0.3,0.,1.));
	gui.add(useAccum.set("useAccum", true));
	
	// Optical Flow
	gui.add(useOpticalFlow.set("Use Optical Flow", false));
	gui.add(lkMaxLevel.set("lkMaxLevel", 3, 0, 8));
	gui.add(lkMaxFeatures.set("lkMaxFeatures", 200, 1, 1000));
	gui.add(lkQualityLevel.set("lkQualityLevel", 0.01, 0.001, .02));
	gui.add(lkMinDistance.set("lkMinDistance", 4, 1, 16));
	gui.add(lkWinSize.set("lkWinSize", 8, 4, 64));
	gui.add(usefb.set("Use Farneback", true));
	gui.add(fbPyrScale.set("fbPyrScale", .5, 0, .99));
	gui.add(fbLevels.set("fbLevels", 4, 1, 8));
	gui.add(fbIterations.set("fbIterations", 2, 1, 8));
	gui.add(fbPolyN.set("fbPolyN", 7, 5, 10));
	gui.add(fbPolySigma.set("fbPolySigma", 1.5, 1.1, 2));
	gui.add(fbUseGaussian.set("fbUseGaussian", false));
	gui.add(fbWinSize.set("winSize", 32, 4, 64));
	curFlow = &fb;

	// camera params
    gui.add(exposureCompensation.set("exposure compensation",0,-10,10));
    gui.add(exposureMeteringMode.set("exposure metering mode",0,0,4));
    gui.add(exposureMode.set("exposure mode",1,0,13));
    gui.add(shutterSpeed.set("shutter speed",0,0,330000));//(in micro seconds)
    gui.add(awbMode.set("AutoWhiteBalance mode",0,0,10));
    gui.add(flickerAvoid.set("Flicker Avoid",0,0,4));
	gui.add(roiX.set("ROI x",0,0,1));
    gui.add(roiY.set("ROI y",0,0,1));
    gui.add(roiW.set("ROI w",1,0,1));
    gui.add(roiH.set("ROI h",1,0,1));

	// event Listeners
	fps.addListener(this, &ofApp::fpsChanged);
	learningTime.addListener(this, &ofApp::learningTimeChanged);
	backgroundThreshold.addListener(this, &ofApp::backgroundThresholdChanged);
#ifdef __arm__
	roiX.addListener(this, &ofApp::roiXChanged);
	roiY.addListener(this, &ofApp::roiYChanged);
	roiW.addListener(this, &ofApp::roiWChanged);
	roiH.addListener(this, &ofApp::roiHChanged);
	exposureCompensation.addListener(this, &ofApp::exposureCompensationChanged);
	exposureMeteringMode.addListener(this, &ofApp::exposureMeteringModeChanged);
	exposureMode.addListener(this, &ofApp::exposureModeChanged);
	awbMode.addListener(this, &ofApp::awbModeChanged);
	flickerAvoid.addListener(this, &ofApp::flickerAvoidChanged);
	shutterSpeed.addListener(this, &ofApp::shutterSpeedChanged);
#endif
	
	// console
    consoleListener.setup(this);
    consoleListener.startThread(false);
	
/*
    exposureMeteringModes[0] = "average";
	exposureMeteringModes[1] = "spot";
	exposureMeteringModes[2] = "backlit";
	exposureMeteringModes[3] = "matrix";
	exposureMeteringModes[4] = "max";

	exposureModes[ 0] = "off";
	exposureModes[ 1] = "auto";
	exposureModes[ 2] = "night";
	exposureModes[ 3] = "night preview";
	exposureModes[ 4] = "backlight";
	exposureModes[ 5] = "spotlight";
	exposureModes[ 6] = "sports";
	exposureModes[ 7] = "snow";
	exposureModes[ 8] = "beach";
	exposureModes[ 9] = "very long";
	exposureModes[10] = "fixed fps";
	exposureModes[11] = "antishake";
	exposureModes[12] = "fireworks";
	exposureModes[13] = "max";
	
	flickerAvoids[0]	= "off";
	flickerAvoids[1]	= "auto";
	flickerAvoids[2]	= "50Hz";
	flickerAvoids[3]	= "60Hz";
	flickerAvoids[4]	= "Max";
*/
	
	// camera Grabber
#ifdef __arm__
    cam.setup(160,120,false);//setup camera (w,h,color = true,gray = false);
    cam.setExposureMode(MMAL_PARAM_EXPOSUREMODE_FIXEDFPS);
	// one of these two gives black during night time
//    cam.setExposureCompensation(0);
//    cam.setAWBMode(MMAL_PARAM_AWBMODE_OFF);
#else
    cam.initGrabber(160,120);
#endif

	// OSC
    sender.setup("192.168.255.255", 8000);
    //sender.setup("192.168.1.3", 8000);
 	receiver.setup(OSC_PORT);
	
	//load params
	filename_save = "RPi_" + RPiId + "_settings.xml";
	if(ofFile::doesFileExist(filename_save)){
		ofLog(OF_LOG_NOTICE)<< "loading from file" + filename_save << endl;
		gui.loadFromFile(filename_save);
	}

	// load openCV params
	background.setLearningTime(learningTime);
	background.setThresholdValue(backgroundThreshold);
	//	background.setIgnoreForeground(true);	// if we set setIgnoreForeground to true
	// we will have problems in case of rain.
}




//--------------------------------------------------------------
void ofApp::update(){
    sent_blobs = 0;
	framenr++;
	
	
	// IF running from RPi
#ifdef __arm__
    frame = Mat(cam.grab());
    stringstream ss;
    ss << "rpi_" << RPiId << ".png";
    if(framenr % 100 == 0) {
      toOf(frame,tosave);

      ofSaveImage(tosave, ss.str());
    }

	// IF running from Computer
#else
    string filename;
    filename = "images/file" + ofToString(framenr) + ".png";
    image.loadImage(filename);
    image.rotate90(2);
	image.setImageType(OF_IMAGE_COLOR);
    frame = toCv(image);
#endif
	
    if (framenr == 130) {
        background.reset();
    }


	if(accum.empty()) {
		frame.convertTo(accum, CV_32F);
		cout << "frameinit" << frame.type() << endl;
	}

	
    if (!frame.empty()) {

		// Low pass filter
        threshold( frame, frame, cutDown, 255, 2 );

		// Optical Flow
		if (useOpticalFlow){
			if(usefb) {
				curFlow = &fb;
				fb.setPyramidScale(fbPyrScale);
				fb.setNumLevels(fbLevels);
				fb.setWindowSize(fbWinSize);
				fb.setNumIterations(fbIterations);
				fb.setPolyN(fbPolyN);
				fb.setPolySigma(fbPolySigma);
				fb.setUseGaussian(fbUseGaussian);
			} else {
				curFlow = &lk;
				lk.setMaxFeatures(lkMaxFeatures);
				lk.setQualityLevel(lkQualityLevel);
				lk.setMinDistance(lkMinDistance);
				lk.setWindowSize(lkWinSize);
				lk.setMaxLevel(lkMaxLevel);
			}
			curFlow->calcOpticalFlow(frame);
		}
		
		// accumulate few frames in current frame in order to wipe out noise
		if(useAccum) {
			accum.convertTo(accum, CV_32F);
			cv::accumulateWeighted(frame, accum, accumFactor);
			accum.convertTo(accum, CV_8U);
			// http://ninghang.blogspot.no/2012/11/list-of-mat-type-in-opencv.html
			//		cout << "framein" << frame.type() << endl;
			//		cout << "accumin" << accum.type() << endl;
			
			//BG subtraction
			background.update(accum, thresholded);
			thresholded.update();
		}else{
			//BG subtraction
			background.update(frame, thresholded);
			thresholded.update();
		}
		
		// Filter noise after threshold
        frameProcessed = toCv(thresholded);
        medianBlur ( frameProcessed, frameProcessed, medianBlurFactor );
        erode( frameProcessed, frameProcessed, erodeFactor );
        dilate( frameProcessed, frameProcessed, dilateFactor );

        toOf(frameProcessed, pix);
        grayImage.setFromPixels(pix);

        contourFinder.findContours(grayImage, minContourArea, maxContourArea, maxContours, true); // find holes

        ofxOscMessage numContours;
        stringstream addr;
        addr << "/RPi_" << RPiId << "/total_contours/";
        numContours.setAddress(addr.str());
        //numContours.addIntArg(contourFinder.nBlobs);
        //sender.sendMessage(numContours);

        for (int i = 0; i < contourFinder.nBlobs; i++){

            ofxOscMessage message;
            stringstream messageAddress;
            float x = contourFinder.blobs[i].boundingRect.x;
            float y = contourFinder.blobs[i].boundingRect.y;
            float width = contourFinder.blobs[i].boundingRect.width;
            float height = contourFinder.blobs[i].boundingRect.height;

            //normalization
            x = x / 160;
            y = y / 120;
            width = width / 160;
            height = height / 120;

            //openGl standard
            x = x + width/2;
            y = y + height /2;
            x = x * 2 - 1;
            y = y * -2 + 1;
            width*=2;
            height*=2;

            if ( y - height/2 < 0 ) {
				message.addFloatArg(x);
                message.addFloatArg(y);
                message.addFloatArg(width);
                message.addFloatArg(height);

				messageAddress << "/RPi_" << RPiId << "/contour_" << (i + 1) << "/";
				message.setAddress(messageAddress.str());
                sender.sendMessage(message);
                sent_blobs++;
            }

        }

        numContours.addIntArg(sent_blobs);
		sender.sendMessage(numContours);

    }

	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage rm;//receivedMessage
		ofxOscMessage sm;//sentMessage
		receiver.getNextMessage(&rm);

		if(rm.getAddress() == "/getParams"){
			sm.setAddress("/allParams");
			sm.addStringArg(RPiId);
			sm.addIntArg(cutDown);
			sm.addIntArg(fps);
			sm.addIntArg(learningTime);
			sm.addIntArg(backgroundThreshold);
			sm.addIntArg(erodeFactor);
			sm.addIntArg(dilateFactor);
			sm.addIntArg(medianBlurFactor);
			sm.addIntArg(minContourArea);
			sm.addIntArg(maxContourArea);
			sm.addIntArg(maxContours);

			sender.sendMessage(sm);
		}
		if(rm.getAddress() == "/save"){
			gui.saveToFile(filename_save);
		}
		else if(rm.getAddress() == "/whoIsThere"){
			sm.setAddress("/RPiId");
			sm.addStringArg(RPiId);
			sender.sendMessage(sm);
		}
		else if(rm.getAddress() == "/resetBG" + RPiId){
			frame.convertTo(accum, CV_32F);
			background.reset();
		}
		else if(rm.getAddress() == "/loadFromFile" + RPiId){
			if(ofFile::doesFileExist(filename_save)){
				ofLog(OF_LOG_NOTICE) << "loading from file " + filename_save << endl;
				gui.loadFromFile(filename_save);
			}
			else{
				ofLog(OF_LOG_NOTICE) << "file " + filename_save + " does not exist" << endl;
			}
		}
		
		// openCv params from OSC
		else if(rm.getAddress() == "/cutDown" + RPiId){
			cutDown = rm.getArgAsInt32(0);
		}
		else if(rm.getAddress() == "/fps" + RPiId){
			fps = rm.getArgAsInt32(0);
		}
		else if(rm.getAddress() == "/learningTime" + RPiId){
			learningTime = rm.getArgAsInt32(0);
		}
		else if(rm.getAddress() == "/accumFactor" + RPiId){
			accumFactor = rm.getArgAsFloat(0);
		}
		else if(rm.getAddress() == "/backgroundThreshold" + RPiId){
			backgroundThreshold = rm.getArgAsInt32(0);
		}
		else if(rm.getAddress() == "/erodeFactor" + RPiId){
			erodeFactor = rm.getArgAsInt32(0);
		}
		else if(rm.getAddress() == "/dilateFactor" + RPiId){
			dilateFactor = rm.getArgAsInt32(0);
		}
		else if(rm.getAddress() == "/medianBlurFactor" + RPiId){
			medianBlurFactor = rm.getArgAsInt32(0);
		}
		else if(rm.getAddress() == "/minContourArea" + RPiId){
			minContourArea = rm.getArgAsInt32(0);
		}
		else if(rm.getAddress() == "/maxContourArea" + RPiId){
			maxContourArea = rm.getArgAsInt32(0);
		}
		else if(rm.getAddress() == "/maxContours" + RPiId){
			maxContours = rm.getArgAsInt32(0);
		}
		else if(rm.getAddress() == "/maxContours" + RPiId){
			maxContours = rm.getArgAsInt32(0);
		}
		else if(rm.getAddress() == "/useAccum" + RPiId){
			useAccum = bool(rm.getArgAsInt32(0));
		}

		// camera params from OSC
#ifdef __arm__
		else if(rm.getAddress() == "/roiX" + RPiId){
			roiX = rm.getArgAsFloat(0);
			ROI.x = roiX;
			cam.setROI(ROI);
		}
		else if(rm.getAddress() == "/roiY" + RPiId){
			roiY = rm.getArgAsFloat(0);
			ROI.y = roiY;
			cam.setROI(ROI);
		}
		else if(rm.getAddress() == "/roiW" + RPiId){
			roiW = rm.getArgAsFloat(0);
			ROI.width = roiW;
			cam.setROI(ROI);
		}
		else if(rm.getAddress() == "/roiH" + RPiId){
			roiH = rm.getArgAsFloat(0);
			ROI.height = roiH;
			cam.setROI(ROI);
		}
		else if(rm.getAddress() == "/exposureCompensation" + RPiId){
			exposureCompensation = rm.getArgAsInt32(0);
			cam.setExposureCompensation(exposureCompensation);
		}
		else if(rm.getAddress() == "/exposureMeteringMode" + RPiId){
			exposureMeteringMode = rm.getArgAsInt32(0);
			exposureMeteringModeInt = rm.getArgAsInt32(0);
			cam.setExposureMeteringMode((MMAL_PARAM_EXPOSUREMETERINGMODE_T)exposureMeteringModeInt);
		}
		else if(rm.getAddress() == "/exposureMode" + RPiId){
			exposureMode = rm.getArgAsInt32(0);
			exposureModeInt = rm.getArgAsInt32(0);
			cam.setExposureMode((MMAL_PARAM_EXPOSUREMODE_T)exposureModeInt);
		}
		else if(rm.getAddress() == "/awbMode" + RPiId){
			awbMode = rm.getArgAsInt32(0);
			awbModeInt = rm.getArgAsInt32(0);
			cam.setAWBMode((MMAL_PARAM_AWBMODE_T)awbModeInt);
		}
		else if(rm.getAddress() == "/flickerAvoid" + RPiId){
			flickerAvoid = rm.getArgAsInt32(0);
			flickerAvoidInt = rm.getArgAsInt32(0);
			cam.setFlickerAvoid((MMAL_PARAM_FLICKERAVOID_T)flickerAvoidInt);
		}
		else if(rm.getAddress() == "/shutterSpeed" + RPiId){
			shutterSpeed = rm.getArgAsInt32(0);
			cam.setShutterSpeed(shutterSpeed);
		}
#endif

	}
}

//-- Parameters events listeners

void ofApp::fpsChanged(int &fps) {
    ofSetFrameRate(fps);
}
void ofApp::learningTimeChanged(int &learningTime) {
    background.setLearningTime(learningTime);
}
void ofApp::backgroundThresholdChanged(int &backgroundThreshold) {
    background.setThresholdValue(backgroundThreshold);
}
#ifdef __arm__
void ofApp::roiXChanged(float &roiX) {
	ROI.x = roiX;
	cam.setROI(ROI);
}
void ofApp::roiYChanged(float &roiY) {
	ROI.y = roiY;
	cam.setROI(ROI);
}
void ofApp::roiWChanged(float &roiW) {
	ROI.width = roiW;
	cam.setROI(ROI);
}
void ofApp::roiHChanged(float &roiH) {
	ROI.height = roiH;
	cam.setROI(ROI);
}
void ofApp::exposureCompensationChanged(int &exposureCompensation) {
	cam.setExposureCompensation(exposureCompensation);
}
void ofApp::exposureMeteringModeChanged(int &exposureMeteringMode) {
	exposureMeteringModeInt = exposureMeteringMode;
	cam.setExposureMeteringMode((MMAL_PARAM_EXPOSUREMETERINGMODE_T)exposureMeteringModeInt);
}
void ofApp::exposureModeChanged(int &exposureMode) {
	exposureModeInt = exposureMode;
	cam.setExposureMode((MMAL_PARAM_EXPOSUREMODE_T)exposureModeInt);
}
void ofApp::awbModeChanged(int &awbMode) {
	awbModeInt = awbMode;
	cam.setAWBMode((MMAL_PARAM_AWBMODE_T)awbModeInt);
}
void ofApp::flickerAvoidChanged(int &flickerAvoid) {
	flickerAvoidInt = flickerAvoid;
	cam.setFlickerAvoid((MMAL_PARAM_FLICKERAVOID_T)flickerAvoidInt);
}
void ofApp::shutterSpeedChanged(int &shutterSpeed) {
	cam.setShutterSpeed(shutterSpeed);
}
#endif

//--------------------------------------------------------------
void ofApp::draw(){
//#ifdef __arm__
    drawMat(frame,0,0,320,240);
    drawMat(accum,640,0,320,240);
	drawMat(frameProcessed,0,240,320,240);
    thresholded.draw(320, 0,320,240);
	curFlow->draw(640,240,320,240);

    contourFinder.draw(320,240,320,240);

    gui.draw();
//#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    ofLogVerbose() << "keyPressed: " << key;

    if(key == ' ') {
		frame.convertTo(accum, CV_32F);
        background.reset();
    }

    if(key == 'r') {
        framenr = 1;
        ofLog()<< "restarting video";
    }
	if(key == 's') {
		gui.saveToFile(filename_save);
		ofLog()<< "saved "+filename_save;
	}
	if(key == 'l') {
		gui.loadFromFile(filename_save);
		ofLog()<< "loaded "+filename_save;
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
    //thresh = ofMap(x,0,ofGetWidth(),0,255);
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

