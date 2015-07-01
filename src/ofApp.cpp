#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFullscreen(true);
    //    ofSetWindowPosition(300, 1200);
    //    ofSetWindowShape(1200, 640);
    ofBackground(0, 0, 0, 0);
    
    fboR.allocate(ofGetWidth(), ofGetHeight());
    fboR.begin();
    ofClear(0, 0, 0, 0);
    fboR.end();
    
    fboG.allocate(ofGetWidth(), ofGetHeight());
    fboG.begin();
    ofClear(0, 0, 0, 0);
    fboG.end();
    
    fboB.allocate(ofGetWidth(), ofGetHeight());
    fboB.begin();
    ofClear(0, 0, 0, 0);
    fboB.end();
    
    fbo.allocate(ofGetWidth(), ofGetHeight());
    fbo.begin();
    ofClear(0, 0, 0, 0);
    fbo.end();
    
    // ofxUI
    gui = new ofxUICanvas();
    gui->setPosition(10, 20);
    toggleDrawR = true;
    
    gui->addToggle("draw R", &toggleDrawR);
    toggleDrawG = true;
    gui->addToggle("draw G", &toggleDrawG);
    toggleDrawB = true;
    gui->addToggle("draw B", &toggleDrawB);
    toggleDrawDebug = false;
    cursorVisible = true;
    gui->addToggle("draw debug", &toggleDrawDebug);
    sliderR = 1.0;
    gui->addSlider("color R", 0.0, 5.0, &sliderR);
    sliderG = 1.0;
    gui->addSlider("color G", 0.0, 5.0, &sliderG);
    sliderB = 1.0;
    gui->addSlider("color B", 0.0, 5.0, &sliderB);
    toggleDrawCamera = false;
    gui->addToggle("draw camera", &toggleDrawCamera);
    buttonCameraR = false;
    gui->addButton("camera R", &buttonCameraR);
    buttonCameraG = false;
    gui->addButton("camera G", &buttonCameraG);
    buttonCameraB = false;
    gui->addButton("camera B", &buttonCameraB);
    gui->addToggle("draw RGB", &toggleDrawRGB);
    toggleDrawRGB = false;
    toggleProjectionRGB = true;
    gui->addToggle("enableProjectionRGB", &toggleProjectionRGB);
    togglePixelDome = false;
    gui->addToggle("draw pixel dome", &togglePixelDome);
    gui->addSpacer();
    gui->addLabel("press c: hide/show cursor", OFX_UI_FONT_SMALL);
    gui->addLabel("press f: fullscreen", OFX_UI_FONT_SMALL);
    gui->addLabel("press g: hide/show GUI", OFX_UI_FONT_SMALL);
    
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    gui->loadSettings("settings.xml");
    image.loadImage("photo.jpg");
    
#ifdef TARGET_OPENGLES
    //    shader.load("shadersES2/shader");
    ofLogNotice("shader not found: TARGET_OPENGLES");
#else
    if(ofIsGLProgrammableRenderer()){
        shader.load("shadersGL3/shader");
    }else{
        //        shader.load("shadersGL2/shader");
        ofLogNotice("shader not found: shadersGL2");
    }
#endif
    
    
    TIME_SAMPLE_SET_FRAMERATE(60.0f);
    //    ofSetVerticalSync(false);
    colorSelector = 0;
    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    //camera
    cameraWidth = ofGetWidth();
    cameraHeight = ofGetHeight();
    vector<ofVideoDevice> devices = videoGrabber.listDevices();
    for(int i = 0; i < devices.size(); i++){
        cout << devices[i].id << ": " << devices[i].deviceName;
        if( devices[i].bAvailable ){
            cout << endl;
        }else{
            cout << " - unavailable " << endl;
        }
    }
    videoGrabber.setDeviceID(0);
    videoGrabber.setDesiredFrameRate(60);
    videoGrabber.initGrabber(cameraWidth,cameraHeight);
    
    lastShootingTime = ofGetElapsedTimef();
    flashTime = 0.1;
    waitingTime = 3.0;
    projectionColorID = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
    //camera
    videoGrabber.update();
    if (videoGrabber.isFrameNew()){
        //update ofImages here
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (toggleProjectionRGB) {
        if (ofGetElapsedTimef()-lastShootingTime > waitingTime) {
            // draw color
            switch (projectionColorID) {
                case 0:
                    ofSetColor(255, 0, 0, 255);
                    break;
                case 1:
                    ofSetColor(0, 255, 0, 255);
                    break;
                case 2:
                    ofSetColor(0, 0, 255, 255);
                    break;
                default:
                    break;
            }
            ofRect(0, 0, ofGetWidth(), ofGetHeight());
            ofSetColor(255, 255, 255, 255);
            if (ofGetElapsedTimef()-lastShootingTime > waitingTime+flashTime) {
                // shooting
                takePicture(projectionColorID);
                lastShootingTime = ofGetElapsedTimef();
                projectionColorID++;
                if (projectionColorID > 2) {
                    projectionColorID = 0;
                }
            }
        }else{
            //draw light layer
            drawRGB();
        }
    }
    if (toggleDrawCamera) videoGrabber.draw((ofGetWidth()-videoGrabber.getWidth())*0.5, (ofGetHeight()-videoGrabber.getHeight())*0.5);
    if (toggleDrawR) drawR();
    if (toggleDrawG) drawG();
    if (toggleDrawB) drawB();
    if (toggleDrawRGB) drawRGB();
    if (togglePixelDome) drawPixelDome();
    if (toggleDrawDebug) drawDebug();
}

void ofApp::drawRGB(){
    // draw light layer
    fbo.begin();
    ofClear(0, 0, 0, 0);
    //R
    shader.begin();
    float colorValueR[] = {sliderR, 0.0, 0.0, 1.0};
    shader.setUniform4fv("colorValue", colorValueR);
    imageArrayRGB[0].draw(0, 0);
    shader.end();
    
    //G
    shader.begin();
    float colorValueG[] = {0.0, sliderG, 0.0, 1.0};
    shader.setUniform4fv("colorValue", colorValueG);
    imageArrayRGB[1].draw(0, 0);
    shader.end();
    
    //B
    shader.begin();
    float colorValueB[] = {0.0, 0.0, sliderB, 1.0};
    shader.setUniform4fv("colorValue", colorValueB);
    imageArrayRGB[2].draw(0, 0);
    shader.end();
    
    fbo.end();
    fbo.draw(0, 0);
}

void ofApp::flashDraw(){
    shader.begin();
    colorSelector++;
    colorSelector %= 3;
    float col[4];
    switch (colorSelector) {
        case 0:
            col[0] = 1.0;
            col[1] = 0.0;
            col[2] = 0.0;
            col[3] = 1.0;
            break;
        case 1:
            col[0] = 0.0;
            col[1] = 1.0;
            col[2] = 0.0;
            col[3] = 1.0;
            break;
        case 2:
            col[0] = 0.0;
            col[1] = 0.0;
            col[2] = 1.0;
            col[3] = 1.0;
            break;
        default:
            break;
    }
    shader.setUniform4fv("colorValue", col);
    image.draw(0, 0);
    shader.end();
}

void ofApp::drawR(){
    fboR.begin();
    ofClear(0, 0, 0, 255);
    ofSetColor(255, 0, 0, 127);
    ofCircle(100, 100, 100);
    fboR.end();
    ofSetColor(255, 255, 255, 255);
    fboR.draw(0, 0);
}

void ofApp::drawG(){
    fboG.begin();
    ofClear(0, 0, 0, 255);
    ofSetColor(0, 255, 0, 127);
    ofCircle(150, 100, 100);
    fboG.end();
    ofSetColor(255, 255, 255, 255);
    fboG.draw(0, 0);
}

void ofApp::drawB(){
    fboB.begin();
    ofClear(0, 0, 0, 255);
    ofSetColor(0, 0, 255, 127);
    ofCircle(125, 150, 100);
    fboB.end();
    ofSetColor(255, 255, 255, 255);
    fboB.draw(0, 0);
}

void ofApp::drawPixelDome(){

}

void ofApp::imageFilter(ofImage *img, ofImage *filteredImage, float r, float g, float b){
    *filteredImage = *img;
    unsigned char* imageData =  filteredImage->getPixels();
    for (int i = 0; i < img->getHeight(); i++) {
        for (int j = 0; j < img->getWidth(); j++) {
            imageData[(int)img->getWidth()*3*i+3*j] *= r;
            imageData[(int)img->getWidth()*3*i+3*j+1] *= g;
            imageData[(int)img->getWidth()*3*i+3*j+2] *= b;
        }
    }
    filteredImage->update();
}

void ofApp::drawDebug(){
    // show frame rate
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 0));
    // guide line
    ofSetColor(255, 255, 255);
    ofLine(0, ofGetHeight()/2., ofGetWidth(), ofGetHeight()/2.);
    ofLine(ofGetWidth()/2., 0, ofGetWidth()/2., ofGetHeight());
    ofSetColor(255, 255, 255, 255);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'c':
            if (cursorVisible) {
                cursorVisible = false;
                ofHideCursor();
            }else{
                cursorVisible = true;
                ofShowCursor();
            }
            break;
        case 'g':
            gui->toggleVisible();
            break;
        case 't':
            
            break;
        case 'f':
            ofToggleFullscreen();
            break;
        default:
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

//ofxUI
void ofApp::exit(){
    gui->saveSettings("settings.xml");
    delete gui;
}

void ofApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    //    int kind = e.widget->getKind();
    if (name == "camera R" && e.getButton()->getValue()) {
        takePicture(0);
    }else if(name == "camera G" && e.getButton()->getValue()){
        takePicture(1);
    }else if (name == "camera B" && e.getButton()->getValue()){
        takePicture(2);
    }else if (name == "draw debug"){
        if (e.getButton()->getValue()) {
            TIME_SAMPLE_ENABLE();
        }else{
            TIME_SAMPLE_DISABLE();
        }
    }
    
    
    //    if(name == "draw R"){
    //        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
    //        toggleDrawR = toggle->getValue();
    //    }else if (name == "draw G"){
    //        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
    //        toggleDrawG = toggle->getValue();
    //    }else if (name == "draw B"){
    //        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
    //        toggleDrawB = toggle->getValue();
    //    }else if (name == "draw debug"){
    //        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
    //        toggleDrawDebug = toggle->getValue();
    //    }
}

void ofApp::takePicture(int RGBID){
    imageArrayRGB[RGBID].setFromPixels(videoGrabber.getPixels(), videoGrabber.getWidth(), videoGrabber.getHeight(), OF_IMAGE_COLOR);
    imageArrayRGB[RGBID].mirror(false, true);
}
