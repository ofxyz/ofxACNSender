#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//node.setup("192.168.0.100", false);
	node.setup("239.255.0.1", true);
}

//--------------------------------------------------------------
void ofApp::update(){
	vector<u_char> data;

	int startUniverse = 1;
	int startChannel = 1;
	int nextChannel = 1;

	for (int i = 0; i < pixelCount*3; i++) {
		// Add a random pixel R, G, B
		data.push_back(static_cast<unsigned char>(round(ofRandom(0, 1)) * 255));
		nextChannel += 1;

		if (data.size() == 512) {
			node.setChannels(startChannel, data.data(), data.size(), startUniverse);
			//node.update();
			startChannel = 1;
			nextChannel = 1;
			startUniverse++;
			data.clear();
		}
	}
	if (data.size() > 0) {
		node.setChannels(startChannel, data.data(), data.size(), startUniverse);
		node.update();
	}
	
}

//--------------------------------------------------------------
void ofApp::draw(){

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
