#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
#include <cmath>

#include "stack.h"
#include "helpers.h"
#include "align.h"
#include "optimisation.h"
#include "videoProcessor.h"

/**
 * @brief Get the avg gradient mag
 *
 * computes the average gradient magnitude of pixels designated by a mask within a cropped region of a frame
 *
 * @param frame 8-bit single channel matrix
 * @param inner_mask 8-bit single channel mask for pixels to be used in quality computation
 * @param rect object for cropping frame around disc
 * @return double of average gradient magnitude of all pixels in frame
 */
double get_avg_gradient_mag(const cv::Mat frame, const cv::Mat inner_mask, const cv::Rect rect)
{
    // gets cropped frame around disc
    cv::Mat croppedFrame = frame(rect);
    // gets cropped mask around disc
    cv::Mat croppedInnerMask = inner_mask(rect);

    cv::Mat magx_frame;
    cv::Mat magy_frame;

    cv::Sobel(croppedFrame, magx_frame, CV_32FC1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::Sobel(croppedFrame, magy_frame, CV_32FC1, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);

    cv::Mat mag;
    cv::magnitude(magx_frame, magy_frame, mag);
    double avg_mag = (double)cv::mean(mag, croppedInnerMask)[0];

    return avg_mag;
}

// NOTE: NEED TO CHANGE BACK TO 16 BIT LATER

/**
 * @brief gets the stacked frame matrix
 *
 *
 * @param frames list of matrix objects to stack
 * @return single-channel 16-bit cv::Mat
 */

cv::Mat stack_frames(std::vector<cv::Mat> frames)
{
    cv::Mat stacked;
    size_t num_frames = frames.size();
    std::vector<cv::Mat> frames_ecc;

    if (num_frames == 0)
    {
        throw std::runtime_error("stack frames called with zero frames");
    }
    int num_rows = frames[0].rows;
    int num_cols = frames[0].cols;

    cv::Mat sum_mat = cv::Mat::zeros(num_rows, num_cols, CV_32SC1);

    for (size_t i = 0; i < frames.size(); i++)
    {

        sum_mat += frames[i];
    }

    sum_mat = sum_mat / cv::Scalar(num_frames);
    // sum_mat.convertTo(stacked, CV_16UC1, 65535.0 / 255.0);

    sum_mat.convertTo(stacked, CV_8UC1);
    return stacked;
}