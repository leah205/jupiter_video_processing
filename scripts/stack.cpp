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
    cv::Mat magx;
    cv::Mat magy;
    cv::GaussianBlur(frame, frame, cv::Size(3, 3), 0);

    cv::Sobel(masked, magx, CV_32FC1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::Sobel(masked, magy, CV_32FC1, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);

    int nr = rows;
    int nc = cols;
    if (magx.isContinuous() && magy.isContinuous())
    {
        nc = nr * nc;
        nr = 1;
    }
    for (int r = 0; r < nr; r++)
    {
        float *mag_x_ptr = magx.ptr<float>(r);
        float *mag_y_ptr = magy.ptr<float>(r);

        for (int c = 0; c < nc; c++)
        {
            float mag_x = (float)*mag_x_ptr;
            float mag_y = (float)*mag_y_ptr;
            total_mag += (double)std::sqrt((mag_x * mag_x + mag_y * mag_y));
            mag_x_ptr++;
            mag_y_ptr++;
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

cv::Mat stack_frames(std::vector<cv::Mat> frames)
{
    cv::Mat stacked;
    size_t num_frames = frames.size();
    int num_rows = frames[0].rows;
    int num_cols = frames[0].cols;
    std::cout << "channels: " << frames[0].channels() << std::endl;
    std::cout << "depth: " << frames[0].depth() << std::endl;

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