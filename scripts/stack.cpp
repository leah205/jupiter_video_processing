#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
#include <cmath>

#include "stack.h"
#include "helpers.h"

/**
 * @brief Get the planet mask object
 *
 * Creates planet mask based on hard coded intensity threshold
 *
 * @param frame
 * @param mask
 */

double get_avg_gradient_mag(cv::Mat frame)
{
    int rows = frame.rows;
    int cols = frame.cols;

    double total_mag;
    cv::Mat mask = get_planet_mask(frame);
    cv::Mat masked;
    frame.copyTo(masked, mask);
    cv::Mat magx = cv::Mat::zeros(rows, cols, CV_32FC1);
    cv::Mat mag_mat = cv::Mat::zeros(rows, cols, CV_32FC1);
    cv::Mat magy = cv::Mat::zeros(rows, cols, CV_32FC1);

    cv::GaussianBlur(frame, frame, cv::Size(3, 3), 0);

    cv::Sobel(masked, magx, CV_8UC1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::Sobel(masked, magy, CV_8UC1, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);
    // optimize later
    uchar *mag_x_ptr = magx.ptr<uchar>(0);
    uchar *mag_y_ptr = magy.ptr<uchar>(0);
    for (int r = 0; r < magx.rows; r++)
    {
        for (int c = 0; c < magx.cols; c++)
        {
            total_mag = std::sqrt(pow(*(mag_x_ptr++), 2) + pow(*(mag_y_ptr++), 2));
        }
    }
    double avg_mag = total_mag / (rows * cols);
    return avg_mag;
}

std::vector<size_t> get_selected_indices(std::vector<cv::Mat> frames, std::vector<double> quality_score_vec, int select_amount)
{
    // change to returning indices and then pass indices function to stacking

    std::vector<size_t> indices(frames.size());
    std::vector<cv::Mat *> selected_frames;
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b)
              { return quality_score_vec[a] > quality_score_vec[b]; });
    std::vector<size_t> selected_indices(indices.begin(), indices.begin() + select_amount);
    return selected_indices;
}

cv::Mat stack_images(std::vector<cv::Mat> frames, std::vector<size_t> selected_indices)
{
    cv::Mat stacked;
    size_t select_num = selected_indices.size();
    int num_rows = frames[0].rows;
    int num_cols = frames[0].cols;
    cv::Mat sum_mat = cv::Mat::zeros(num_rows, num_cols, CV_32SC1);
    for (size_t i = 0; i < select_num; i++)
    {
        int frame_index = selected_indices[i];
        int nc = num_cols;
        int nl = num_rows;
        cv::Mat cur = frames[frame_index];
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