#include <opencv2/opencv.hpp>

#include "align.h"
#include "helpers.h"
#include "videoProcessor.h"

/**
 * @brief Get the center of mass object
 *
 * @param frame single channel 8-bit matrix
 * @return cv::Point, center of mass of planet
 */

cv::Point get_center_of_mass(cv::Mat frame)
{

    cv::Mat mask = get_planet_mask(frame);
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
 * @param frame FraneInfo object with 8-bit single channel matrix
 * @param ref cv::Point center of mass of reference frame
 * @return cv::Mat aligned frame
 */
frameInfo get_aligned_by_centroid(frameInfo frame_obj, cv::Point ref)
{

    frameInfo aligned_frame = frame_obj;
    cv::Mat frame = frame_obj.frame;
    cv::Point cm = frame_obj.cm;

    double offset_x, offset_y;
    offset_x = cm.x - ref.x;
    offset_y = cm.y - ref.y;

    cv::Mat translation_matrix = (cv::Mat_<double>(2, 3) << 1, 0, -1 * offset_x, 0, 1, -1 * offset_y);
    int height = frame.cols;
    int width = frame.rows;

    cv::warpAffine(frame, aligned_frame.frame, translation_matrix, cv::Size(width, height));
    return aligned_frame;
}
