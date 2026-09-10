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
 * computes the average gradient magnitude of all the pixels in the frame
 *
 * @param frame 8-bit single channel matrix
 * @return double of average gradient magnitude of all pixels in frame
 */
double get_avg_gradient_mag(cv::Mat frame)
{

    double total_mag = 0;
    cv::Mat mask = get_planet_mask(frame);
    cv::Mat innerMask = cv::Mat::zeros(mask.size(), CV_8UC1);

    cv::Rect rect = cv::boundingRect(mask);
    double radius = cv::min(rect.width, rect.height) / 2.0;

    cv::Point cm = get_center_of_mass(mask);
    cv::circle(innerMask, cm, radius * 0.9, cv::Scalar(255), cv::FILLED);

    cv::Mat croppedFrame = frame(rect);
    cv::Mat croppedInnerMask = innerMask(rect);

    int rows = croppedFrame.rows;
    int cols = croppedFrame.cols;

    cv::Mat magx_frame;
    cv::Mat magy_frame;

    cv::Sobel(croppedFrame, magx_frame, CV_32FC1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::Sobel(croppedFrame, magy_frame, CV_32FC1, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);

    int nr = rows;
    int nc = cols;

    if (magx_frame.isContinuous() && magy_frame.isContinuous() && croppedFrame.isContinuous() && croppedInnerMask.isContinuous())
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
                float mag_x = (float)*mag_x_ptr;
                float mag_y = (float)*mag_y_ptr;
                total_mag += (double)std::sqrt((mag_x * mag_x + mag_y * mag_y));
            }
            mask_ptr++;
            mag_x_ptr++;
            mag_y_ptr++;
        }
    }
    double avg_mag = total_mag / (rows * cols);
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
    for (size_t i = 0; i < num_frames; i++)
    {
        int nc = num_cols;
        int nl = num_rows;
        cv::Mat cur = frames[i];
        if (cur.isContinuous() && sum_mat.isContinuous())
        {
            nc = num_cols * num_rows;
            nl = 1;
        }
        for (int row = 0; row < nl; row++)
        {
            uchar *p = cur.ptr(row);
            int *sum_ptr = sum_mat.ptr<int>(row);
            for (int col = 0; col < nc; col++)
            {
                *sum_ptr += *p;
                sum_ptr++;
                p++;
            }
        }
    }
    cv::normalize(sum_mat, stacked, 0, 1 << 16, cv::NORM_MINMAX, CV_16UC1);
    return stacked;
}