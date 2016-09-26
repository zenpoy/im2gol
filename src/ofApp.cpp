#include "ofApp.h"


void drawHelp() {
    int lineHeight = 15;
    int i = 45;
    ofDrawBitmapString(std::string("<space>: next board"), 10, i);
    i += lineHeight;
    ofDrawBitmapString(std::string("r: random or from image"), 10, i);
    i += lineHeight;
    ofDrawBitmapString(std::string("i: restart"), 10, i);
    i += lineHeight;
    ofDrawBitmapString(std::string("=: increase history"), 10, i);
    i += lineHeight;
    ofDrawBitmapString(std::string("-: decrease history"), 10, i);
    i += lineHeight;
    ofDrawBitmapString(std::string("o: show original (white)"), 10, i);
    i += lineHeight;
    ofDrawBitmapString(std::string("]: increase speed"), 10, i);
    i += lineHeight;
    ofDrawBitmapString(std::string("[: decrease speed"), 10, i);
    i += lineHeight;
    ofDrawBitmapString(std::string("f: fullscreen"), 10, i);
    i += lineHeight;
    ofDrawBitmapString(std::string("p: play/pause"), 10, i);
    i += lineHeight;
    ofDrawBitmapString(std::string("\\: next genreation"), 10, i);
    i += lineHeight;
    ofDrawBitmapString(std::string("h: show help"), 10, i);
    i += lineHeight;
}

//--------------------------------------------------------------
void ofApp::setup()
{
    maxHistory = 20;
    minHistory = 1;

    gol = std::shared_ptr<cgol::GLife>(new cgol::GLife(maxHistory));
    //cv::Mat map(cv::Size(30, 30), CV_8UC1);
    cv::theRNG().state = ofGetSystemTime();
   /* cv::randu(map, cv::Scalar(0), cv::Scalar(2));
    gol->setBoard(map);*/


    ofEnableAlphaBlending();

    fps = 5;
    maxFps = 10000;
    minFps = 1;

    history = 1;

    ofSetWindowShape(500, 500);
    //ofSetVerticalSync(true);
    //ofSetFrameRate(30);
    ofSetDrawBitmapMode(ofDrawBitmapMode::OF_BITMAPMODE_SIMPLE);

    fileCount = 0;
    currFileIndex = 0;
    if (args.size() > 1)
    {
        dir = ofDirectory(args[1]);
        dir.allowExt("jpg");
        dir.allowExt("png");
        dir.allowExt("gif");
        fileCount = dir.listDir();
    }
    else {
        exit();
    }

    if (args.size() > 2)
    {
        fps = std::stoi(std::string(args[2]));
    }



    keyPressed(' ');

    drawOriginal = false;
    isPlay = false;
    isPlayOnce = false;
    isHelp = false;
    isSaveVideo = false;
}

//--------------------------------------------------------------
void ofApp::update()
{
    if ((ofGetElapsedTimeMillis() > (1000.0 / fps) + 1  && isPlay) || isPlayOnce)
    {
        gol->play();
        if (isPlayOnce)
        {
            isPlayOnce = false;
        }
        ofResetElapsedTimeCounter();
    }
}

void drawBoard(cv::Mat& map, ofColor& fgColor, float modus = 1.0f)
{
    ofPushMatrix();
    ofScale(1.0 / (double)MAX(map.cols, map.rows), 1.0 / (double)MAX(map.cols, map.rows));
    ofTranslate(map.rows > map.cols ? (map.rows - map.cols) / 2.0 : 0,
                map.cols > map.rows ? (map.cols - map.rows) / 2.0 : 0);

    //float modus = 1.0;
    const float borderFactor = 0.0;
    float boarder = modus * borderFactor;
    float side = modus * (1.0 - borderFactor);

    ofTranslate((1.0 - side) / 2.0f, (1.0 - side) / 2.0f);

    ofFill();
    for (int i = 0; i < map.rows; ++i)
    {
        auto p = map.ptr<unsigned char>(i);
        for (int j = 0; j < map.cols; ++j)
        {
            if (p[j] == 1)
            {
                ofSetColor(fgColor);
                ofDrawRectangle(ofPoint(j, i), side, side);
            }
        }
    }

    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(ofColor(16, 16, 20, 255));

    auto h = ofGetWindowHeight();
    auto w = ofGetWindowWidth();

    ofPushMatrix();

    ofTranslate(w > h ? (w - h) / 2.0 : 0, h > w ? (h - w) / 2.0 : 0);
    ofScale((double)MIN(w, h), (double)MIN(w, h));

    ofPushMatrix();
    auto c = gol->getBoard().cols;
    auto r = gol->getBoard().rows;
    ofScale(1.0 / (double)MAX(c, r), 1.0 / (double)MAX(c, r));
    ofTranslate(r > c ? (r - c) / 2.0 : 0,
                c > r ? (c - r) / 2.0 : 0);

    ofFill();
    ofSetColor(241, 239, 240, 255);
    ofDrawRectangle(ofPoint(0, 0), c, r);

    ofPopMatrix();


    int currAlpha = 255;
    float alphaDecayFactor = 0.5;
    auto& boards = gol->getBoards(history);
    //for (int i = 0; i < boards.size(); ++i)
    //{
    //    auto& map = boards[i];
    //    if (i == 0)
    //    {
    //        drawBoard(map, ofColor(205, 0, 0, currAlpha));
    //    }
    //    else
    //    {
    //        drawBoard(map, ofColor(0, 0, 205, currAlpha));
    //    }
    //    currAlpha *= alphaDecayFactor;

    //}
    drawBoard(boards.back(), ofColor(142, 1, 10, currAlpha));


    if (drawOriginal)
    {
        drawBoard(gol->getOriginalBoard(), ofColor(0, 0, 0, 128), 0.5);
    }

    ofPopMatrix();

    if (isHelp)
    {
        ofSetColor(255);
        ofDrawBitmapString(std::to_string(gol->getGeneration()), 30, 35);
        ofDrawBitmapString(currMap, 10, h - 15);
        drawHelp();
    }


    if (vw != nullptr) {
        img.grabScreen(0, 0, w, h);
        cv::Mat frame(img.getHeight(), img.getWidth(), CV_8UC3, img.getPixelsRef().begin());
        cv::Mat frameOut;
        cv::cvtColor(frame, frameOut, CV_RGB2BGR);
        vw->write(frameOut);
    }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

    switch (key)
    {   
    case ' ':
    {
        if (fileCount > 0)
        {
            //std::string f = dir.getPath(std::rand() % fileCount);
            std::string f = dir.getFile(currFileIndex % fileCount).getAbsolutePath();
            cv::Mat map = cv::imread(f);
            gol->setBoard(map);
            currMap = f;
            currFileIndex++;
            currFileIndex %= fileCount;
            break;
        }
    }
    case 'r':
    {
        currMap = "Random map: " + std::to_string(cv::theRNG().state);
        cv::Mat map(cv::Size(30, 30), CV_8UC1);
        cv::randu(map, cv::Scalar(0), cv::Scalar(2));
        gol->setBoard(map);
        break;
    }
    case 'i':
    {
        gol->reset();
        break;
    }
    case '=':
    {
        history++;
        history = history > maxHistory ? maxHistory : history;
        break;
    }
    case '-':
    {
        history--;
        history = history < minHistory ? minHistory : history;
        break;
    }
    case 'o':
    {
        drawOriginal = !drawOriginal;
        break;
    }
    case ']':
        fps = 1.5 * fps + 1;
        if (fps >= maxFps)
        {
            fps = maxFps;
        }
        //ofSetFrameRate(fps);
        break;
    case '[':
        fps = (fps - 1) / 1.5;
        if (fps <= minFps)
        {
            fps = minFps;
        }
        //ofSetFrameRate(fps);
        break;
    case 'f':
        //ofToggleFullscreen();
        if (ofGetWindowMode() == ofWindowMode::OF_FULLSCREEN)
        {
            ofSetFullscreen(false);
            ofSetWindowShape(640, 480);
            ofSetWindowPosition(320, 240);
            //ofSetVerticalSync(true);
            //ofSetFrameRate(30);
        }
        else
        {
            ofSetFullscreen(true);
            //ofSetVerticalSync(true);
            //ofSetFrameRate(30);
        }
        break;
    case 'p':
        isPlay = !isPlay;
        break;
    case '\\':
        isPlayOnce = true;
        break;
    case 'e':
        cv::imwrite(currMap + "_" + std::to_string(gol->getGeneration()) + ".png", (1 - gol->getBoard()) * 255);
        break;
    case 'h':
        isHelp = !isHelp;
        break;
    case 'v':
        if (vw == nullptr) {
            vw = new cv::VideoWriter("output.avi", CV_FOURCC('X','V','I','D'), fps, cv::Size(ofGetWindowWidth(), ofGetWindowHeight()));
        }
        else {
            vw->release();
            delete vw;
            vw = nullptr;
        }
        break;
    default:
        break;
    }


}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}
