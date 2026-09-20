#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
#include <cmath>
#include <string>

#include "videoProcessor.h"

// g++ main.cpp -pg -O0  -g -o my_program $(pkg-config --cflags --libs opencv4)

int main()
{
    double duration = static_cast<double>(cv::getTickCount());

    VideoProcessor processor;
    bool ret;
    cv::Mat frame;

    std::string filename = "2026-03-18-0236_9-Jupiter_656HIA.avi";
    std::string output_dir = "../data/";
    std::string path = output_dir + filename;

    cv::VideoCapture cap(path);
    // cv::VideoCapture cap("../data/2026-03-18-0241_0-Jupiter_620CH4.avi");
    // cv::VideoCapture cap("../data/2026-03-18-0239_0-Jupiter_632OI.avi");
    int frame_num = cap.get(cv::CAP_PROP_FRAME_COUNT);
    int fps = cap.get(cv::CAP_PROP_FPS);

    std::cout << "number of frames: " << frame_num << std::endl;
    std::cout << "frames per second: " << fps << std::endl;

    if (!cap.isOpened())
    {
        std::cout << "Error: video object was not opened successfully" << std::endl;
        return -1;
    }
    else
    {
        std::cout << "Video opened successfully" << std::endl;
    }

    while (true)
    {

        ret = cap.read(frame);

        if (!ret)
        {
            break;
        }

        processor.addFrame(frame);
    }

    int select_amount = ceil(((double)(frame_num)) / 4);
    processor.setFrameStackNum(select_amount);
    processor.selectFramesByGradient();
    processor.alignSelectedFramesByCentroid();
    processor.stackAlignedFrames();

    duration = static_cast<double>(cv::getTickCount()) - duration;
    duration /= cv::getTickFrequency();
    std::cout << "duration of program: " << duration << " s" << std::endl;
}
