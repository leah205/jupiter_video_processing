#include <iostream>
#include <opencv2/opencv.hpp>

// compile with g++ -o main main.cpp
// g++ main.cpp -o my_program $(pkg-config --cflags --libs opencv4) - turn into make file

void get_planet_mask(cv::Mat frame, cv::Mat mask)
{
    cv::Mat blurred;
    // frame.copyTo(blurred);
    // should i blur here?
    cv::GaussianBlur(frame, blurred, cv::Size(3, 3), 0);
    cv::threshold(blurred, mask, 10, 255, cv::THRESH_BINARY);
};

// void get_center_of_mass(frame)
// {
// }

void get_array_info(cv::Mat frame)
{
    // 320
    std::cout << "rows: " << frame.rows << std::endl;
    // 320
    std::cout << "cols: " << frame.cols << std::endl;
    std::cout << "channels: " << frame.channels() << std::endl;
    // CV_8U
    std::cout << "data type: " << frame.type() << std::endl;
    std::cout << "depth: " << frame.depth() << std::endl;
}

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
        get_array_info(frame);
        cv::Mat mask = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC3);
        get_planet_mask(frame, mask);

        cv::imshow("First frame", frame);
        cv::imshow("planet mask", mask);
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