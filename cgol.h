#pragma once

#include "opencv2\opencv.hpp"
#include "crc32.h"
#include <memory>

namespace cgol {
class GLife
{
public:
    GLife();
    GLife(int historySize);
    void setBoard(const cv::Mat& board);
    void reset();
    cv::Mat & getOriginalBoard();
    cv::Mat& getBoard();
    std::vector<cv::Mat> getBoards(int numOfBoards);
    void play(int setps = 1);
    int64 getGeneration();

    unsigned int hash(cv::Mat & map);

private:

    void play(cv::Mat& board);
    cv::Mat board;
    std::deque<cv::Mat> boards;
    cv::Mat originalBoard;
    int64 generation;
    int maxHistorySize;

    std::unique_ptr<Crc32> crc32;

};
}