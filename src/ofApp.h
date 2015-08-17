#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxXmlSettings.h"
#include "ofxTimeMeasurements.h"

class ofApp : public ofBaseApp{

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
		
    void flashDraw();
    void drawR();
    void drawG();
    void drawB();
    void drawRGB();
    void drawPixelDome1(ofTexture& textureMain, ofTexture& textureR, ofTexture& textureG, ofTexture& textureB, float x, float y, float width, float height);
    void drawPixelDome2(ofTexture& textureMain, ofTexture& textureR, ofTexture& textureG, ofTexture& textureB, float x, float y, float width, float height);
    void drawDebug();
    void imageFilter(ofImage *img, ofImage *filteredImage, float r, float g, float b);
    void drawDisplayAllVideoGrabber();
    void drawDisplayAllVideoPlayer();
    
    //ofxUI
    void exit();
    void guiEvent(ofxUIEventArgs &e);
    
    //camera
    void takePicture(int RGBID);
    
    ofFbo fboR, fboG, fboB, fbo;
    ofImage image;
    ofImage imageArrayRGB[3];
    ofShader shaderLightLayer, shaderPixelDome1, shaderPixelDome2;
    int colorSelector;
    
    //ofxUI
    ofxUICanvas *gui;
    bool toggleDrawR, toggleDrawG, toggleDrawB, toggleDrawDebug, toggleDrawCamera, buttonCameraR, buttonCameraG, buttonCameraB, toggleDrawRGB,toggleProjectionRGB, togglePixelDome1, togglePixelDome2, toggleDisplayAllVideoGrabber, toggleDisplayAllVideoPlayer, changeCameraForVideoGrabberMain, changeCameraForVideoGrabberR, changeCameraForVideoGrabberG, changeCameraForVideoGrabberB, toggleDebugVideo;
    float sliderR, sliderG, sliderB, valueChangerR, valueChangerG, valueChangerB;
    //camera
    int cameraWidth;
    int cameraHeight;
    ofVideoGrabber videoGrabberMain, videoGrabberR, videoGrabberG, videoGrabberB;
    int deviceIdMain, deviceIdR, deviceIdG, deviceIdB;
    
    //enable projection installation
//    bool enableProjectionRGB;
    int lastShootingTime;
    float flashTime;
    float waitingTime;
    int projectionColorID;
    
    bool cursorVisible;
    
    // debug
    ofVideoPlayer videoPlayerMain, videoPlayerR, videoPlayerG, videoPlayerB;
};
