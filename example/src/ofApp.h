#pragma once

#include "ofMain.h"
#include "ofxACNSender.h"

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
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofxACNSender node;
		ofEventListeners listeners;

		int pixelCount = 94 * 4; // 376 * 3 = 1128
		unsigned long last;
		ofFbo dataFbo;
		ofColor col;
};
