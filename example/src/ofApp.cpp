#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(15);

    node.setup("192.168.0.50", true);
    
    // float Gamma value, and the max Colour Value (0-255) for each channel.
    node.SetGammaValues(2.5, 255,255,255); 

    dataFbo.allocate(pixelCount, 1, GL_RGB);
    last = ofGetElapsedTimeMillis();
    col.set(255, 200, 200, 10);
    col2.setHsb(0, 255, 255, 20);
}

//--------------------------------------------------------------
void ofApp::update(){
    if (ofGetFrameNum() % (int)ofGetWidth()) barWidth = (int)ofRandom(10, 50);
    dataFbo.begin();
    col2.setHue(ofGetFrameNum() % 255);
    ofSetColor(col);
    ofDrawRectangle(0, 0, dataFbo.getWidth(), 1);
    ofSetColor(col2);
    ofDrawRectangle(-barWidth + (ofGetFrameNum() % (int)dataFbo.getWidth()), 0, barWidth, 1);
    dataFbo.end();

    ofPixels pix;
    dataFbo.readToPixels(pix);

    std::pair<int, int> StartUC = { 1,1 };
    StartUC = node.setChannels(StartUC.first, StartUC.second, pix);
    node.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    dataFbo.draw(0, 0, ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
