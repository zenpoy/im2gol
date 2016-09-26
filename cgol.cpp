#include "cgol.h"


cgol::GLife::GLife() : generation(0), maxHistorySize(10)
{
    crc32 = std::unique_ptr<Crc32>(new Crc32());
}

cgol::GLife::GLife(int historySize) : generation(0), maxHistorySize(historySize)
{
    crc32 = std::unique_ptr<Crc32>(new Crc32());
}


void cgol::GLife::setBoard(const cv::Mat& board)
{
    cv::Mat tempBoard;
    if (board.channels() == 4) {
        cv::cvtColor(board, tempBoard, CV_RGBA2GRAY);
    }
    else if (board.channels() == 3) {
        cv::cvtColor(board, tempBoard, CV_RGB2GRAY);
    }
    else {
        tempBoard = board.clone();
    }

    tempBoard.convertTo(tempBoard, CV_8U);
    
    double minVal, maxVal;
    cv::minMaxIdx(tempBoard, &minVal, &maxVal);
    cv::Mat fg = tempBoard < (maxVal + minVal) / 2.0;
    tempBoard.setTo(1, fg);
    tempBoard.setTo(0, ~fg);

    int w = board.cols;
    int h = board.rows;

    originalBoard = tempBoard;
    reset();

}

void cgol::GLife::reset() {
    board = originalBoard.clone();
    this->boards.clear();
    this->boards.push_back(this->board);

    generation = 0;
}

cv::Mat & cgol::GLife::getOriginalBoard() {
    return originalBoard;
}

cv::Mat & cgol::GLife::getBoard()
{
    return this->board;
}

std::vector<cv::Mat> cgol::GLife::getBoards(int numOfBoards=1)
{
    std::vector<cv::Mat> res;

    for (int i = 0; i < numOfBoards && i < boards.size(); ++i) {
        res.push_back(boards[i]);
    }

    return res;
}

void cgol::GLife::play(int steps)
{
    int w, h;
    for (int i = 0; i < steps; ++i) {
        if (board.isSubmatrix()) {
            w = board.cols + 2;
            h = board.rows + 2;
            board.adjustROI(1, 1, 1, 1);
            board(cv::Rect(0, 1, w, 1)).copyTo(board(cv::Rect(0, h - 1, w, 1)));
            board(cv::Rect(0, h - 2, w, 1)).copyTo(board(cv::Rect(0, 0, w, 1)));
            board(cv::Rect(1, 0, 1, h)).copyTo(board(cv::Rect(w - 1, 0, 1, h)));
            board(cv::Rect(w - 2, 0, 1, h)).copyTo(board(cv::Rect(0, 0, 1, h)));
        }
        else {
            copyMakeBorder(board, board, 1, 1, 1, 1, cv::BORDER_WRAP);
            w = board.cols;
            h = board.rows;
        }
        play(board);
        board = board(cv::Rect(1, 1, w - 2, h - 2));
        boards.push_front(board.clone());
        if (boards.size() > maxHistorySize) {
            boards.pop_back();
        }
        
    }
    generation += steps;
}

int64 cgol::GLife::getGeneration()
{
    return generation;
}

unsigned int cgol::GLife::hash(cv::Mat& map)
{
    crc32->Reset();
    crc32->AddData(map.data, map.step1() * map.rows);
    return crc32->GetCrc32();
}

//void rewarpBorder(cv::Mat& m) {
//    for (int i = 0; i < m.rows; ++i)
//    {
//        auto p = m.ptr<unsigned char>(i);
//        for (int j = 0; j < m.cols; ++j)
//        {
//            
//        }
//    }
//}

void cgol::GLife::play(cv::Mat& board)
{
    unsigned char kerData[9] = {1, 1, 1,
                                1, 0, 1,
                                1, 1, 1};
    cv::Mat kernel(cv::Size(3, 3), CV_8UC1, &kerData);
    cv::Mat res(board.size(), board.type());
    cv::filter2D(board, res, -1, kernel, cv::Point(-1, -1), 0);

    board.setTo(0, res < 2);
    board.setTo(1, res == 3);
    board.setTo(0, res > 3);
    //board.setTo(1, res == 2 & board == 1);

}
