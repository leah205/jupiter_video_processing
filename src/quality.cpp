#include <opencv2/opencv.hpp>
#include "quality.h"
#include "frameInfo.h"
#include "test_helpers.h"
#include "helpers.h"

/**
 * @brief generates a quality score for frame
 *
 * @param frame
 * @return double
 */

double FrameQuality::score(const frameInfo &frame) const
{
    cv::Mat inner_mask = get_inner_planet_mask(frame.planet_mask);
    cv::Rect rect = get_cropped_rect(frame.frame);
    double score;
    switch (m_metric)
    {
    case QualityMetric::GRADIENT:
        score = get_avg_gradient_mag(frame.frame, inner_mask, rect);
        break;
    case QualityMetric::LAPLACIAN:
        score = get_laplacian_variance(frame.frame, inner_mask, rect);
    }
    return score;
}

/**
 * @brief calculates the laplacian variance on disk in frame
 *
 * @param frame
 * @param inner_mask
 * @param rect
 * @return double
 */

double get_laplacian_variance(const cv::Mat frame, const cv::Mat inner_mask, const cv::Rect rect)
{
    cv::Mat croppedFrame = frame(rect);
    cv::Mat croppedInnerMask = inner_mask(rect);
    cv::Mat lap_frame;
    cv::Scalar mean, stddev;
    // cv::imshow("cropped inner mask", croppedInnerMask);
    // cv::waitKey(0);
    // cv::Laplacian(frame, lap_frame, CV_8UC1);
    cv::Laplacian(frame, lap_frame, CV_32F);

    cv::meanStdDev(lap_frame, mean, stddev, croppedInnerMask);
    return stddev[0] * stddev[0];
}

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