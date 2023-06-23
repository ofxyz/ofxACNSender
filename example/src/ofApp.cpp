#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(30);

    //node.setup("192.168.0.100", false);
    node.setup("239.255.0.1", true);

    dataFbo.allocate(pixelCount, 1, GL_RGB);
    last = ofGetElapsedTimeMillis();
    col.setHsb(0, 255, 255);
}

//--------------------------------------------------------------
void ofApp::update(){

    dataFbo.begin();
    ofClear(0);
    col.setHue(ofGetFrameNum() % 256);
    ofSetColor(col);
    ofDrawRectangle(0, 0, dataFbo.getWidth(), 1);
    ofSetColor(0, 0, 0);
    ofDrawRectangle(ofGetFrameNum() % (int)dataFbo.getWidth(), 0, 1, 1);
    dataFbo.end();

    ofPixels pix;
    dataFbo.readToPixels(pix);

    std::pair<int, int> StartUC = { 1,1 };
    StartUC = node.setChannels(StartUC.first, StartUC.second, pix);
    StartUC = node.setChannels(StartUC.first, StartUC.second, pix);
    StartUC = node.setChannels(StartUC.first, StartUC.second, pix);
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
