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

    if (used_pixels == 0)
    {
        return 0.0;
    }
    double avg_mag = total_mag / (used_pixels);
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

    double mean_new_corr = 0;
    double min_new_corr = 0;

    double mean_corr = 0;
    double min_corr = 0;
    double mean_shiftmag = 0;
    double max_shiftmag = 0;

    if (num_frames == 0)
    {
        throw std::runtime_error("stack frames called with zero frames");
    }
    int num_rows = frames[0].rows;
    int num_cols = frames[0].cols;

    cv::Mat sum_mat = cv::Mat::zeros(num_rows, num_cols, CV_32SC1);

    for (size_t i = 0; i < frames.size(); i++)
    {
        cv::Mat frame_ecc;
        cv::Mat new_aligned;

        double shift_mag = transform_ecc(frames[0], frames[i], new_aligned);

        double corr = compute_ecc(frames[0], frames[i]);
        double new_corr = compute_ecc(frames[0], new_aligned);
        cv::Mat difference;
        cv::absdiff(frames[i], new_aligned, difference);

        double min_val, max_val;
        cv::minMaxLoc(difference, &min_val, &max_val);

        mean_corr += corr;
        mean_new_corr += new_corr;
        min_corr = std::min(min_corr, corr);
        min_new_corr = std::min(min_new_corr, new_corr);

        mean_shiftmag += shift_mag;
        max_shiftmag = std::max(shift_mag, max_shiftmag);
        if (i == 0)
        {
            min_corr = corr;
            min_new_corr = min_corr;
        }

        sum_mat += frames[i];
    }

    sum_mat = sum_mat / cv::Scalar(num_frames);

    mean_corr = mean_corr / frames.size();
    mean_new_corr = mean_new_corr / frames.size();
    mean_shiftmag = mean_shiftmag / frames.size();

    // sum_mat.convertTo(stacked, CV_16UC1, 65535.0 / 255.0);
    std::cout << "min coor: " << min_corr << std::endl;
    std::cout << "mean coor: " << mean_corr << std::endl;

    std::cout << "min new coor: " << min_new_corr << std::endl;
    std::cout << "mean new coor: " << mean_new_corr << std::endl;

    std::cout << "mean shift mag: " << mean_shiftmag << std::endl;
    std::cout << "max shift mag: " << max_shiftmag << std::endl;

    sum_mat.convertTo(stacked, CV_8UC1);
    return stacked;
}