#pragma once

#include "ofMain.h"
#include "cgol.h"
#include "opencv2\opencv.hpp"

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

        std::shared_ptr<cgol::GLife> gol;
        std::vector<std::string> args;
        ofDirectory dir;
        int fileCount;
        int currFileIndex;
        int fps;
        int minFps;
        int maxFps;
        int history;
        int maxHistory;
        int minHistory;
        bool isPlay;
        bool isPlayOnce;
        bool isHelp;
        bool isSaveVideo;

        ofImage img;

        cv::VideoWriter* vw;

        std::string currMap;

        bool drawOriginal;
		
};
