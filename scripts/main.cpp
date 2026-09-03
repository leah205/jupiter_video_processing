#include <iostream>
#include <opencv2/opencv.hpp>

// compile with g++ -o main main.cpp
// g++ main.cpp -o my_program $(pkg-config --cflags --libs opencv4) - turn into make file

int main()
{
    cv::VideoCapture cap("../data/2026-03-18-0236_9-Jupiter_656HIA.avi");

    if (!cap.isOpened())
    {
        std::cout << "Error: video object was not opened successfully" << std::endl;
        return -1;
    }
    else
    {
        std::cout << "Video opened successfully" << std::endl;
    }

    cv::Mat frame;
    bool ret = cap.read(frame);

    if (ret)
    {
        cv::imshow("First frame", frame);
        cv::waitKey(0);
        cv::destroyAllWindows();
    }
    else
    {
        std::cout << "Error: could not read frame" << std::endl;
    }

    cap.release();
    return 0;
}