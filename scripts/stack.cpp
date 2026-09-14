#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
#include <cmath>

#include "stack.h"
#include "helpers.h"
#include "align.h"

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

    double total_mag = 0;

    // gets cropped frame around disc
    cv::Mat croppedFrame = frame(rect);

    // gets cropped mask around disc
    cv::Mat croppedInnerMask = inner_mask(rect);

    int rows = croppedFrame.rows;
    int cols = croppedFrame.cols;

    cv::Mat magx_frame;
    cv::Mat magy_frame;

    cv::Sobel(croppedFrame, magx_frame, CV_32FC1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::Sobel(croppedFrame, magy_frame, CV_32FC1, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);

    int nr = rows;
    int nc = cols;

    int used_pixels = 0;

    if (magx_frame.isContinuous() && magy_frame.isContinuous() && croppedInnerMask.isContinuous())
    {
        nc = nr * nc;
        nr = 1;
    }
    for (int r = 0; r < nr; r++)
    {
        float *mag_x_ptr = magx_frame.ptr<float>(r);
        float *mag_y_ptr = magy_frame.ptr<float>(r);
        uchar *mask_ptr = croppedInnerMask.ptr<uchar>(r);

        for (int c = 0; c < nc; c++)
        {
            if (*mask_ptr)
            {
                float mag_x = *mag_x_ptr;
                float mag_y = *mag_y_ptr;
                total_mag += (double)std::sqrt((mag_x * mag_x + mag_y * mag_y));
                used_pixels += 1;
            }
            mask_ptr++;
            mag_x_ptr++;
            mag_y_ptr++;
        }
    }
    double avg_mag = total_mag / (used_pixels);
    return avg_mag;
}

/**
 * @brief Get the selected indices object
 *
 * Gets the indices of the best scoring frames according to some quality metric
 *
 * @param quality_score_vec list of computed quality scores corresponding to frames in video
 * @param select_amount number of frames to select
 * @return std::vector<size_t>
 */

std::vector<size_t> get_selected_indices(std::vector<double> quality_score_vec, int select_amount)
{

    std::vector<size_t> indices(quality_score_vec.size());
    std::vector<cv::Mat *> selected_frames;
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b)
              { return quality_score_vec[a] > quality_score_vec[b]; });
    std::vector<size_t> selected_indices(indices.begin(), indices.begin() + select_amount);
    return selected_indices;
}

/**
 * @brief gets the stacked frame matrix
 *
 * Stacks all the given frames and normalizes intensity
 *
 * @param frames frames to stack
 * @return single-channel 16-bit cv::Mat
 */

cv::Mat stack_frames(std::vector<cv::Mat> frames)
{

    cv::Mat stacked;
    size_t num_frames = frames.size();
    int num_rows = frames[0].rows;
    int num_cols = frames[0].cols;

    cv::Mat sum_mat = cv::Mat::zeros(num_rows, num_cols, CV_32SC1);
    double min, max;

    for (size_t i = 0; i < frames.size(); i++)
    {

        sum_mat += frames[i];
    }

    sum_mat = sum_mat / cv::Scalar(num_frames);
    sum_mat.convertTo(stacked, CV_16UC1, 65535.0 / 255.0);
    return stacked;
}