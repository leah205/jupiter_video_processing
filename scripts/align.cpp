#include <opencv2/opencv.hpp>

#include "align.h"
#include "helpers.h"
#include "videoProcessor.h"

/**
 * @brief Get the center of mass object
 *
 * @param mask single channel 8-bit binary bit mask of pixels on disc
 * @return cv::Point, center of mass of planet
 */

cv::Point get_center_of_mass(const cv::Mat mask)
{

    cv::Moments m = cv::moments(mask, true);

    if (std::abs(m.m00) < 1e-8)
    {
        throw std::runtime_error("Zero-area blob");
    }

    double x = m.m10 / m.m00;
    double y = m.m01 / m.m00;

    cv::Point p(x, y);
    return p;
}

/**
 * @brief aligns frame by centroid
 *
 *  aligns frame so that its planet center of mass matches with that of the reference frame
 *
 * @param frame ,atrox object with 8-bit single channel matrix
 * @param ref cv::Point center of mass of reference frame
 * @return cv::Mat aligned frame
 */
cv::Mat get_aligned_by_centroid(cv::Mat frame, cv::Point cm, cv::Point ref)
{

    cv::Mat aligned_frame;

    double offset_x, offset_y;
    offset_x = cm.x - ref.x;
    offset_y = cm.y - ref.y;

    // std::cout << "x-shift: " << offset_x << std::endl;
    // std::cout << "y-shift: " << offset_y << std::endl;

    cv::Mat translation_matrix = (cv::Mat_<double>(2, 3) << 1, 0, -1 * offset_x, 0, 1, -1 * offset_y);
    int height = frame.cols;
    int width = frame.rows;

    cv::warpAffine(frame, aligned_frame, translation_matrix, cv::Size(width, height));
    return aligned_frame;
}
