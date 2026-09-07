#include <opencv2/opencv.hpp>

#include "align.h"
#include "helpers.h"

/**
 * @brief Get the center of mass object
 *
 * @param frame
 * @return cv::Point
 */

cv::Point get_center_of_mass(cv::Mat frame)
{
    cv::Mat mask = get_planet_mask(frame);

    cv::extractChannel(mask, mask, 0);
    cv::Moments m = cv::moments(mask, true);

    if (std::abs(m.m00) < 1e-8)
    {
        throw std::runtime_error("Zero-area blob");
    }

    cv::Point p(m.m10 / m.m00, m.m01 / m.m00);
    return p;
}

/**
 * @brief aligns frame so that centroid matches with planet centroid of reference
 *
 * @param frame
 * @param cm
 * @param ref
 * @return cv::Mat
 */
cv::Mat align_frame(cv::Mat frame, cv::Point cm, cv::Point ref)
{
    cv::Mat aligned_frame;

    cv::extractChannel(frame, frame, 0);
    double offset_x, offset_y;
    offset_x = cm.x - ref.x;
    offset_y = cm.y - ref.y;

    cv::Mat translation_matrix = (cv::Mat_<double>(2, 3) << 1, 0, -1 * offset_x, 0, 1, -1 * offset_y);
    int height = frame.cols;
    int width = frame.rows;

    cv::warpAffine(frame, aligned_frame, translation_matrix, cv::Size(width, height));
    return aligned_frame;
}

std::vector<cv::Mat> align_all_to_ref(std::vector<cv::Mat> frames, cv::Mat ref_frame)
{
    cv::Point ref_cm = get_center_of_mass(ref_frame);
    size_t num_frames = frames.size();
    std::vector<cv::Mat> aligned_frames;
    for (size_t i = 0; i < num_frames; i++)
    {
        cv::Mat frame = frames[i];
        cv::Point cm = get_center_of_mass(frame);
        cv::Mat aligned = align_frame(frame, cm, ref_cm);
        aligned_frames.push_back(aligned);
    }
    return aligned_frames;
}