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

    // Light Layer
    gui->addLabel("Light Layer");
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

    toggleDrawRGB = false;
    gui->addToggle("Light Layer - draw RGB", &toggleDrawRGB);
    buttonCameraR = false;
    gui->addButton("Light Layer - camera R", &buttonCameraR);
    buttonCameraG = false;
    gui->addButton("Light Layer - camera G", &buttonCameraG);
    buttonCameraB = false;
    gui->addButton("Light Layer - camera B", &buttonCameraB);
    toggleProjectionRGB = true;
    gui->addToggle("enableProjectionRGB", &toggleProjectionRGB);
    
    // Pixel Dome
    gui->addSpacer();
    gui->addLabel("Pixel Dome");
    togglePixelDome1 = false;
    gui->addToggle("draw pixel dome 1", &togglePixelDome1);
    togglePixelDome2 = false;
    gui->addToggle("draw pixel dome 2", &togglePixelDome2);
    toggleDisplayAllVideoGrabber = false;
    gui->addToggle("display all videoGrabber", &toggleDisplayAllVideoGrabber);
    toggleDisplayAllVideoPlayer = false;
    gui->addToggle("display all videoPlayer", &toggleDisplayAllVideoPlayer);
    changeCameraForVideoGrabberMain = false;
    gui->addButton("change camera Main", changeCameraForVideoGrabberMain);
    changeCameraForVideoGrabberR = false;
    gui->addButton("change camera R", changeCameraForVideoGrabberR);
    gui->addSlider("value changer R", 0.0, 10.0, &valueChangerR);
    changeCameraForVideoGrabberG = false;
    gui->addButton("change camera G", changeCameraForVideoGrabberG);
    gui->addSlider("value changer G", 0.0, 10.0, &valueChangerG);
    changeCameraForVideoGrabberB = false;
    gui->addButton("change camera B", changeCameraForVideoGrabberB);
    gui->addSlider("value changer B", 0.0, 10.0, &valueChangerB);
    toggleDebugVideo = false;
    gui->addToggle("use video instead of camera", &toggleDebugVideo);
    // command explanatin
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
        // light layer
        shaderLightLayer.load("shadersGL3_lightLayer/shader");
        // pixel dome - calculate color in the shader
        shaderPixelDome1.load("shadersGL3_pixelDome1/shader");
        // pixel dome - draw three times on fbo
        shaderPixelDome2.load("shadersGL3_pixelDome2/shader");
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
    vector<ofVideoDevice> devices = videoGrabberMain.listDevices();
    for(int i = 0; i < devices.size(); i++){
        cout << devices[i].id << ": " << devices[i].deviceName;
        if( devices[i].bAvailable ){
            cout << endl;
        }else{
            cout << " - unavailable " << endl;
        }
    }
    
    // initialize videograbber
    deviceIdMain = 0;
    deviceIdR = 0;
    deviceIdG = 0;
    deviceIdB = 0;

    // Main
    videoGrabberMain.setDeviceID(deviceIdMain);
    videoGrabberMain.initGrabber(cameraWidth, cameraHeight);
    // R
    videoGrabberR.setDeviceID(deviceIdR);
    videoGrabberR.initGrabber(cameraWidth, cameraHeight);
    // G
    videoGrabberG.setDeviceID(deviceIdG);
    videoGrabberG.initGrabber(cameraWidth, cameraHeight);
    // B
    videoGrabberB.setDeviceID(deviceIdB);
    videoGrabberB.initGrabber(cameraWidth, cameraHeight);
    
    lastShootingTime = ofGetElapsedTimef();
    flashTime = 0.1;
    waitingTime = 3.0;
    projectionColorID = 0;
    
    // debug with video
    // - Main
    videoPlayerMain.loadMovie("movies/main.mp4");
    videoPlayerMain.setLoopState(OF_LOOP_NORMAL);
    videoPlayerMain.play();
    // - R
    videoPlayerR.loadMovie("movies/r.mp4");
    videoPlayerR.setLoopState(OF_LOOP_NORMAL);
    videoPlayerR.play();
    // - G
    videoPlayerG.loadMovie("movies/g.mp4");
    videoPlayerG.setLoopState(OF_LOOP_NORMAL);
    videoPlayerG.play();
    // - B
    videoPlayerB.loadMovie("movies/b.mp4");
    videoPlayerB.setLoopState(OF_LOOP_NORMAL);
    videoPlayerB.play();
}


//--------------------------------------------------------------
void ofApp::update(){
    //update videograbber
    videoGrabberMain.update();
    videoGrabberR.update();
    videoGrabberG.update();
    videoGrabberB.update();
    
    if (toggleDebugVideo) {
        videoPlayerMain.update();
        videoPlayerR.update();
        videoPlayerG.update();
        videoPlayerB.update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableBlendMode(OF_BLENDMODE_ADD);
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
    if (toggleDrawCamera) videoGrabberMain.draw((ofGetWidth()-videoGrabberMain.getWidth())*0.5, (ofGetHeight()-videoGrabberMain.getHeight())*0.5);
    if (toggleDrawR) drawR();
    if (toggleDrawG) drawG();
    if (toggleDrawB) drawB();
    if (toggleDrawRGB) drawRGB();

    ofPoint origin = ofPoint(300, 300);
    ofPoint size = ofPoint(240, 160);

    
    if (togglePixelDome1){
        if (toggleDebugVideo) {
            drawPixelDome1(videoPlayerMain.getTextureReference(), videoPlayerR.getTextureReference(), videoPlayerG.getTextureReference(), videoPlayerB.getTextureReference(), origin.x, origin.y, size.x, size.y);
        }else{
            drawPixelDome1(videoGrabberMain.getTextureReference(), videoGrabberR.getTextureReference(), videoGrabberG.getTextureReference(), videoGrabberB.getTextureReference(), 100, 600, size.x, size.y);
        }
    }
    if (togglePixelDome2){
        if (toggleDebugVideo) {
            drawPixelDome2(videoPlayerMain.getTextureReference(), videoPlayerR.getTextureReference(), videoPlayerG.getTextureReference(), videoPlayerB.getTextureReference(), origin.x+size.x+20, origin.y, size.x, size.y);
        }else{
            drawPixelDome2(videoGrabberMain.getTextureReference(), videoGrabberR.getTextureReference(), videoGrabberG.getTextureReference(), videoGrabberB.getTextureReference(),500, 600, size.x, size.y);
        }
    }
    if (toggleDrawDebug) drawDebug();
    if (toggleDisplayAllVideoGrabber) drawDisplayAllVideoGrabber();
    if (toggleDisplayAllVideoPlayer) drawDisplayAllVideoPlayer();
}

void ofApp::drawDisplayAllVideoGrabber(){
    ofPoint origin = ofPoint(300, 30);
    ofPoint size = ofPoint(240, 160);
    float gapBetweenGrabber = 20;
    float rectHeight = 5;

    // Main
    ofSetColor(ofColor::white);
    ofRect(origin.x, origin.y-rectHeight, size.x, rectHeight);
    ofSetColor(255);
    videoGrabberMain.draw(origin.x, origin.y, size.x, size.y);
    ofDrawBitmapString(ofToString(deviceIdMain)+": "+ofToString(videoGrabberMain.listDevices()[deviceIdMain].deviceName), origin.x, origin.y+size.y+20);
    // R
    ofSetColor(ofColor::red);
    ofRect(origin.x+1*(size.x+gapBetweenGrabber), origin.y-rectHeight, size.x, rectHeight);
    ofSetColor(255);
    videoGrabberR.draw(origin.x+1*(size.x+gapBetweenGrabber), origin.y, size.x, size.y);
    ofDrawBitmapString(ofToString(deviceIdR)+": "+ofToString(videoGrabberMain.listDevices()[deviceIdR].deviceName), origin.x+1*(size.x+gapBetweenGrabber), origin.y+size.y+20);
    // G
    ofSetColor(ofColor::green);
    ofRect(origin.x+2*(size.x+gapBetweenGrabber), origin.y-rectHeight, size.x, rectHeight);
    ofSetColor(255);
    videoGrabberG.draw(origin.x+2*(size.x+gapBetweenGrabber), origin.y, size.x, size.y);
    ofDrawBitmapString(ofToString(deviceIdG)+": "+ofToString(videoGrabberMain.listDevices()[deviceIdG].deviceName), origin.x+2*(size.x+gapBetweenGrabber), origin.y+size.y+20);
    // B
    ofSetColor(ofColor::blue);
    ofRect(origin.x+3*(size.x+gapBetweenGrabber), origin.y-rectHeight, size.x, rectHeight);
    ofSetColor(255);
    videoGrabberB.draw(origin.x+3*(size.x+gapBetweenGrabber), origin.y, size.x, size.y);
    ofDrawBitmapString(ofToString(deviceIdB)+": "+ofToString(videoGrabberMain.listDevices()[deviceIdB].deviceName), origin.x+3*(size.x+gapBetweenGrabber), origin.y+size.y+20);
}

void ofApp::drawDisplayAllVideoPlayer(){
    ofPoint origin = ofPoint(300, 330);
    ofPoint size = ofPoint(240, 160);
    float gapBetweenGrabber = 20;
    float rectHeight = 5;
    
    // Main -- 
    ofSetColor(ofColor::white);
    ofRect(origin.x, origin.y-rectHeight, size.x, rectHeight);
    ofSetColor(255);
    videoPlayerMain.draw(origin, size.x, size.y);
    // R
    ofSetColor(ofColor::red);
    ofRect(origin.x+1*(size.x+gapBetweenGrabber), origin.y-rectHeight, size.x, rectHeight);
    ofSetColor(255);
    videoPlayerR.draw(origin.x+1*(size.x+gapBetweenGrabber), origin.y, size.x, size.y);
    // G
    ofSetColor(ofColor::green);
    ofRect(origin.x+2*(size.x+gapBetweenGrabber), origin.y-rectHeight, size.x, rectHeight);
    ofSetColor(255);
    videoPlayerG.draw(origin.x+2*(size.x+gapBetweenGrabber), origin.y, size.x, size.y);
    // B
    ofSetColor(ofColor::blue);
    ofRect(origin.x+3*(size.x+gapBetweenGrabber), origin.y-rectHeight, size.x, rectHeight);
    ofSetColor(255);
    videoPlayerB.draw(origin.x+3*(size.x+gapBetweenGrabber), origin.y, size.x, size.y);
}


void ofApp::drawRGB(){
    // draw light layer
    fbo.begin();
    ofClear(0, 0, 0, 0);
    //R
    shaderLightLayer.begin();
    float colorValueR[] = {sliderR, 0.0, 0.0, 1.0};
    shaderLightLayer.setUniform4fv("colorValue", colorValueR);
    imageArrayRGB[0].draw(0, 0);
    shaderLightLayer.end();
    
    //G
    shaderLightLayer.begin();
    float colorValueG[] = {0.0, sliderG, 0.0, 1.0};
    shaderLightLayer.setUniform4fv("colorValue", colorValueG);
    imageArrayRGB[1].draw(0, 0);
    shaderLightLayer.end();
    
    //B
    shaderLightLayer.begin();
    float colorValueB[] = {0.0, 0.0, sliderB, 1.0};
    shaderLightLayer.setUniform4fv("colorValue", colorValueB);
    imageArrayRGB[2].draw(0, 0);
    shaderLightLayer.end();
    
    fbo.end();
    fbo.draw(0, 0);
}

void ofApp::drawPixelDome1(ofTexture& textureMain, ofTexture& textureR, ofTexture& textureG, ofTexture& textureB, float x, float y, float width, float height){
    // draw pixel dome
//    fbo.begin();
//    ofClear(0, 0, 0, 0);
//
//    // draw R
//    // R image
//    ofImage imageR;
//    imageR.setFromPixels(videoGrabberMain.getPixels(), videoGrabberMain.getWidth(), videoGrabberMain.getHeight(), OF_IMAGE_COLOR);
//    imageR.mirror(false, true);
//    // shader
//    shaderLightLayer.begin();
//    float colorValueR[] = {sliderR, 0.0, 0.0, 1.0};
//    shaderLightLayer.setUniform4fv("colorValue", colorValueR);
//    imageR.draw(0, 0);
//    shaderLightLayer.end();
    
    // new method
    fbo.begin();
    ofClear(0, 0, 0, 0);
    shaderPixelDome1.begin();
    shaderPixelDome1.setUniformTexture("texMain", textureMain, 0);
    shaderPixelDome1.setUniformTexture("texR", textureR, 1);
    shaderPixelDome1.setUniform1f("valueChangerR", valueChangerR);
    shaderPixelDome1.setUniformTexture("texG", textureG, 2);
    shaderPixelDome1.setUniform1f("valueChangerG", valueChangerG);
    shaderPixelDome1.setUniformTexture("texB", textureB, 3);
    shaderPixelDome1.setUniform1f("valueChangerB", valueChangerB);
    if (toggleDebugVideo) {
        videoPlayerMain.draw(x, y, width, height);
    }else{
        videoGrabberMain.draw(x, y, width, height);
    }
    shaderPixelDome1.end();
    fbo.end();
    //here use this R image for making with master image from main camera
    
    fbo.draw(0, 0);
}

void ofApp::drawPixelDome2(ofTexture& textureMain, ofTexture& textureR, ofTexture& textureG, ofTexture& textureB, float x, float y, float width, float height){
    fbo.begin();
    ofClear(0, 0, 0, 0);

    // color id: R = 1, G = 2, B = 3

    //R
    shaderPixelDome2.begin();
    ofRect(400, 400, 100, 100);
    shaderPixelDome2.setUniformTexture("texMain", textureMain, 0);
    shaderPixelDome2.setUniformTexture("texR", textureR, 1);
    shaderPixelDome2.setUniformTexture("texG", textureG, 2);
    shaderPixelDome2.setUniformTexture("texB", textureB, 3);
    shaderPixelDome2.setUniform1i("colorId", 1);
    if (toggleDebugVideo) {
        for (int i=0; i<5; i++) {
            videoPlayerMain.draw(x, y, width, height);
        }
    }else{
        videoGrabberMain.draw(x, y, width, height);
    }
    shaderPixelDome2.end();
    
    //G
    shaderPixelDome2.begin();
    ofRect(500, 500, 100, 100);
    shaderPixelDome2.setUniformTexture("texMain", textureMain, 0);
    shaderPixelDome2.setUniformTexture("texR", textureR, 1);
    shaderPixelDome2.setUniformTexture("texG", textureG, 2);
    shaderPixelDome2.setUniformTexture("texB", textureB, 3);
    shaderPixelDome2.setUniform1i("colorId", 2);
    if (toggleDebugVideo) {
        for (int i=0; i<5; i++) {
            videoPlayerMain.draw(x, y, width, height);
        }
    }else{
        videoGrabberMain.draw(x+50, y-50, width, height);
    }
    shaderPixelDome2.end();
    
    //B
    shaderPixelDome2.begin();
    ofRect(600, 600, 100, 100);
    shaderPixelDome2.setUniformTexture("texMain", textureMain, 0);
    shaderPixelDome2.setUniformTexture("texR", textureR, 1);
    shaderPixelDome2.setUniformTexture("texG", textureG, 2);
    shaderPixelDome2.setUniformTexture("texB", textureB, 3);
    shaderPixelDome2.setUniform1i("colorId", 3);
    if (toggleDebugVideo) {
        for (int i = 0; i<5; i++) {
            videoPlayerMain.draw(x, y, width, height);
        }
    }else{
        videoGrabberMain.draw(x+50, y+50, width, height);
    }
    shaderPixelDome2.end();
    
    ofSetColor(255, 0, 0, 200);
    ofCircle(1100, 100, 100);
    
    ofSetColor(0, 255, 0, 200);
    ofCircle(1150, 100, 100);
    
    ofSetColor(0, 0, 255, 200);
    ofCircle(1125, 150, 100);
    
    fbo.end();
    //here use this R image for making with master image from main camera
    
    ofSetColor(255);
    fbo.draw(0, 0);
}

void ofApp::flashDraw(){
    shaderLightLayer.begin();
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
    shaderLightLayer.setUniform4fv("colorValue", col);
    image.draw(0, 0);
    shaderLightLayer.end();
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
    if (name == "Light Layer - camera R" && e.getButton()->getValue()) {
        takePicture(0);
    }else if(name == "Light Layer - camera G" && e.getButton()->getValue()){
        takePicture(1);
    }else if (name == "Light Layer - camera B" && e.getButton()->getValue()){
        takePicture(2);
    }else if (name == "draw debug"){
        if (e.getButton()->getValue()) {
            TIME_SAMPLE_ENABLE();
        }else{
            TIME_SAMPLE_DISABLE();
        }
    }else if(name == "change camera R" && e.getButton()->getValue()){
        deviceIdR = (deviceIdR+1)%videoGrabberR.listDevices().size();
        videoGrabberR.close();
        videoGrabberR.setDeviceID(deviceIdR);
        videoGrabberR.initGrabber(cameraWidth, cameraHeight);
        ofLogNotice("-----\ncamera Main:"+ofToString(deviceIdMain)+"\ncamera R:"+ofToString(deviceIdR)+"\ncamera G:"+ofToString(deviceIdG)+"\ncamera B:"+ofToString(deviceIdB)+"\n-----");
    }else if(name == "change camera G" && e.getButton()->getValue()){
        deviceIdG = (deviceIdG+1)%videoGrabberG.listDevices().size();
        videoGrabberG.close();
        videoGrabberG.setDeviceID(deviceIdG);
        videoGrabberG.initGrabber(cameraWidth, cameraHeight);
        ofLogNotice("\ncamera Main:"+ofToString(deviceIdMain)+"\ncamera R:"+ofToString(deviceIdR)+"\ncamera G:"+ofToString(deviceIdG)+"\ncamera B:"+ofToString(deviceIdB));
    }else if(name == "change camera B" && e.getButton()->getValue()){
        deviceIdB = (deviceIdB+1)%videoGrabberB.listDevices().size();
        videoGrabberB.close();
        videoGrabberB.setDeviceID(deviceIdB);
        videoGrabberB.initGrabber(cameraWidth, cameraHeight);
        ofLogNotice("\ncamera Main:"+ofToString(deviceIdMain)+"\ncamera R:"+ofToString(deviceIdR)+"\ncamera G:"+ofToString(deviceIdG)+"\ncamera B:"+ofToString(deviceIdB));
    }else if(name == "change camera Main" && e.getButton()->getValue()){
        deviceIdMain = (deviceIdMain+1)%videoGrabberMain.listDevices().size();
        videoGrabberMain.close();
        videoGrabberMain.setDeviceID(deviceIdMain);
        videoGrabberMain.initGrabber(cameraWidth, cameraHeight);
        ofLogNotice("\ncamera Main:"+ofToString(deviceIdMain)+"\ncamera R:"+ofToString(deviceIdR)+"\ncamera G:"+ofToString(deviceIdG)+"\ncamera B:"+ofToString(deviceIdB));
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
    imageArrayRGB[RGBID].setFromPixels(videoGrabberMain.getPixels(), videoGrabberMain.getWidth(), videoGrabberMain.getHeight(), OF_IMAGE_COLOR);
    imageArrayRGB[RGBID].mirror(false, true);
}
