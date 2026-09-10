#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
#include <cmath>

#include "stack.h"
#include "align.h"

// g++ main.cpp -pg -O0  -g -o my_program $(pkg-config --cflags --libs opencv4)

/**
 * @brief converts frame to single channel
 *
 * @param frame
 */
void compress(cv::Mat &frame)
{
    cv::extractChannel(frame, frame, 0);
}

int main()
{
    double duration = static_cast<double>(cv::getTickCount());
    cv::VideoCapture cap("../data/2026-03-18-0236_9-Jupiter_656HIA.avi");
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

    std::vector<cv::Mat> frames;
    std::vector<double> mags_vec;

    std::cout << "reading frames..." << std::endl;

    cv::Mat ref_frame;
    bool ret = cap.read(ref_frame);

    compress(ref_frame);
    frames.push_back(ref_frame);

    cv::Mat frame;
    while (true)
    {

        ret = cap.read(frame);

        if (!ret)
        {
            break;
        }

        compress(frame);
        frames.push_back(frame);
    }

    std::cout << "selecting frames..." << std::endl;

    for (int i = 0; i < frame_num; i++)
    {
        mags_vec.push_back(get_avg_gradient_mag(frames[i]));
    }

    size_t select_amount = ceil((double)(frame_num) / 4);
    std::vector<size_t> selected_indices = get_selected_indices(mags_vec, select_amount);

    std::cout << "aligning frames..." << std::endl;

    std::vector<cv::Mat> aligned_frames = align_selected_to_ref(frames, ref_frame, selected_indices);

    std::cout << "stacking " << select_amount << " frames... " << std::endl;
    cv::Mat stacked = stack_frames(aligned_frames);
    cv::imshow("stacked", stacked);
    cv::waitKey(0);
    cap.release();

    duration = static_cast<double>(cv::getTickCount()) - duration;
    duration /= cv::getTickFrequency();
    std::cout << "duration of program: " << duration << " s" << std::endl;
    return 0;
}