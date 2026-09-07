#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
#include <cmath>

#include "stack.h"
#include "align.h"

// g++ main.cpp -pg -O0  -g -o my_program $(pkg-config --cflags --libs opencv4)

void get_array_info(cv::Mat frame)
{
    // 320
    std::cout << "rows: " << frame.rows << std::endl;
    // 320
    std::cout << "cols: " << frame.cols << std::endl;
    std::cout << "channels: " << frame.channels() << std::endl;
    // CV_8U (char)
    std::cout << "data type: " << frame.type() << std::endl;
    std::cout << "depth: " << frame.depth() << std::endl;
}

/**
 * @brief returns frame with values normalized between 0 and 255
 *
 * @param frame
 */
cv::Mat preprocess(cv::Mat frame)
{
    cv::Mat processed;
    cv::normalize(frame, processed, 0, 255, cv::NORM_MINMAX);
    return processed;
}

/**
 * @brief stacks frames and normalizes 16-bit result
 *
 * @param frames
 * @param num_frames
 * @return cv::Mat
 */

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
    double mags_arr[frame_num];
    double *mags_ptr = mags_arr;

    std::cout << "performing initial alignment..." << std::endl;

    cv::Mat ref_frame;
    bool ret = cap.read(ref_frame);
    ref_frame = preprocess(ref_frame);
    frames.push_back(ref_frame);
    *mags_ptr = get_avg_gradient_mag(ref_frame);
    mags_ptr++;
    // get_array_info(frame);

    cv::Mat frame;

    while (true)
    {

        ret = cap.read(frame);

        if (!ret)
        {
            break;
        }

        preprocess(frame);

        frames.push_back(frame);
        *mags_ptr = get_avg_gradient_mag(frame);
        mags_ptr++;
    }

    std::vector<cv::Mat> aligned_frames = align_all_to_ref(frames, ref_frame);

    std::cout
        << "selecting frames..." << std::endl;
    size_t select_amount = ceil((double)(frame_num) / 4);
    std::vector<size_t> selected_indices = get_selected_indices(frames, mags_arr, select_amount);
    std::cout << "stacking frames... " << std::endl;
    cv::Mat stacked = stack_images(frames, selected_indices);
    // cv::imshow("stacked", stacked);
    // cv::waitKey(0);
    cap.release();

    duration = static_cast<double>(cv::getTickCount()) - duration;
    duration /= cv::getTickFrequency();
    std::cout << "duration of program: " << duration << " s" << std::endl;
    return 0;
}