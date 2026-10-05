#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
#include <cmath>

#include "stack.h"
#include "helpers.h"
#include "align.h"
#include "optimisation.h"
// #include "videoProcessor.h"
#include "frameInfo.h"

// NOTE: NEED TO CHANGE BACK TO 16 BIT LATER

/**
 * @brief gets the stacked frame matrix
 *
 *
 * @param frames list of matrix objects to stack
 * @return single-channel 16-bit cv::Mat
 */

cv::Mat stack_frames(std::vector<frameInfo> frames)
{
    cv::Mat stacked;
    size_t num_frames = frames.size();
    std::vector<cv::Mat> frames_ecc;

    if (num_frames == 0)
    {
        throw std::runtime_error("stack frames called with zero frames");
    }
    int num_rows = frames[0].frame.rows;
    int num_cols = frames[0].frame.cols;

    cv::Mat sum_mat = cv::Mat::zeros(num_rows, num_cols, CV_32SC1);

    for (size_t i = 0; i < frames.size(); i++)
    {

        sum_mat += frames[i].frame;
    }

    sum_mat = sum_mat / cv::Scalar(num_frames);
    // sum_mat.convertTo(stacked, CV_16UC1, 65535.0 / 255.0);

    sum_mat.convertTo(stacked, CV_8UC1);
    return stacked;
}