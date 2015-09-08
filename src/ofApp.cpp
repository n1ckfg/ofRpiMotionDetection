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

    ofLog() << "RPid " << RPiId;
    gui.setup("panel");
    gui.setPosition(650,10);
    gui.add(cutDown.set( "cutDown", 100, 1, 255 ));
    gui.add(fps.set("fps", 16, 1, 30));
    gui.add(learningTime.set("learningTime",100,1,1000));
    gui.add(backgroundThreshold.set("backgroundThreshold",15,1,300));
    gui.add(erodeFactor.set("erodeFactor",0,0,3));
    gui.add(dilateFactor.set("dilateFactor",0,0,3));
    gui.add(medianBlurFactor.set("medianBlur",1,0,5));
    gui.add(minContourArea.set("minContourArea",20, 5, 40));
    gui.add(maxContourArea.set("maxContourArea",(160*120)/3,40, 160*120)); // one third of the screen.
    gui.add(maxContours.set("maxContours", 5, 1, 10));
    ofSetFrameRate(fps);
    
    consoleListener.setup(this);
    consoleListener.startThread(false, false);

    background.setLearningTime(learningTime);
    background.setThresholdValue(backgroundThreshold);


    fps.addListener(this, &ofApp::fpsChanged);
    learningTime.addListener(this, &ofApp::learningTimeChanged);
    backgroundThreshold.addListener(this, &ofApp::backgroundThresholdChanged);


#ifdef __arm__
    cam.setup(160,120,false);//setup camera (w,h,color = true,gray = false);
    cam.setExposureMode(MMAL_PARAM_EXPOSUREMODE_FIXEDFPS);
    //cam.setExposureCompensation(0);
    //cam.setAWBMode(MMAL_PARAM_AWBMODE_OFF);
    //cam.setExposureCompensation(0);
#else
    cam.initGrabber(160,120);
#endif
    

    sender.setup("192.168.255.255", 8000);
}

static int framenr = 1;


//--------------------------------------------------------------
void ofApp::update(){
    framenr++;
#ifdef __arm__
    frame = Mat(cam.grab());
    stringstream ss;
    ss << "rpi_" << RPiId << ".png";
    if(framenr % 100 == 0) {    
      toOf(frame,tosave);
    
      ofSaveImage(tosave, ss.str());
    } 
#else
    ofImage image;

    stringstream filename;
    filename << "file" << framenr << ".png";
    image.loadImage(filename.str());
    image.rotate90(2);
    frame = toCv(image);
    
#endif

    if (framenr == 130) {
        background.reset();
    }
 
    if (!frame.empty()) {

        //threshold( frame, frame, cutDown, 255,2 );
       
        background.update(frame, thresholded);
        thresholded.update();

        frameProcessed = toCv(thresholded);
        medianBlur ( frameProcessed, frameProcessed, medianBlurFactor );
        erode( frameProcessed, frameProcessed, erodeFactor );
        dilate( frameProcessed, frameProcessed, dilateFactor );

        ofPixels pix;
        ofImage img;
        toOf(frameProcessed, pix);
        grayImage.setFromPixels(pix);

        
        contourFinder.findContours(grayImage, minContourArea, maxContourArea, maxContours, true); // find holes

        ofxOscMessage numContours;
        stringstream addr;
        addr << "/RPi_" << RPiId << "/total_contours/";
        numContours.setAddress(addr.str());
        numContours.addIntArg(contourFinder.nBlobs);
        sender.sendMessage(numContours);

        for (int i = 0; i < contourFinder.nBlobs; i++){

            ofxOscMessage message;
            stringstream messageAddress;
            messageAddress << "/RPi_" << RPiId << "/contour_" << (i + 1) << "/";
            message.setAddress(messageAddress.str());
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

            message.addFloatArg(x);
            message.addFloatArg(y);
            message.addFloatArg(width);
            message.addFloatArg(height);

            sender.sendMessage(message);
        }
    }

    
}

//-- Parameters events listeners

void ofApp::fpsChanged(int & fps) {
    ofSetFrameRate(fps);
}

void ofApp::learningTimeChanged(int &learningTime) {
    background.setLearningTime(learningTime);
}

void ofApp::backgroundThresholdChanged(int &backgroundThreshold) {
    background.setThresholdValue(backgroundThreshold);
}

//--------------------------------------------------------------
void ofApp::draw(){
    drawMat(frame,0,0,320,240);
    drawMat(frameProcessed,0,240,320,240);
    thresholded.draw(320, 0,320,240);

    contourFinder.draw(320,240,320,240);

    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    ofLogVerbose() << "keyPressed: " << key;

    if(key == ' ') {
        background.reset();
    }

    if(key == 'r') {
        framenr = 1;
        ofLog()<< "restarting video";
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

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
