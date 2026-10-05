#include <opencv2/opencv.hpp>

#include "align.h"
#include "helpers.h"
#include "frameInfo.h"

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

cv::Point get_circle_centroid(const cv::Mat mask)
{
    cv::Point2f center;
    std::vector<cv::Point> points;
    findNonZero(mask, points);
    float radius;
    cv::minEnclosingCircle(points, center, radius);
    return center;
    // cv::Moments m = cv::moments(mask, true);

    // if (std::abs(m.m00) < 1e-8)
    // {
    //     throw std::runtime_error("Zero-area blob");
    // }

    // double x = m.m10 / m.m00;
    // double y = m.m01 / m.m00;

    // cv::Point p(x, y);
    // return p;
}

std::vector<frameInfo> Aligner::alignFramesToRef(std::vector<frameInfo> input_frames)
{
    cv::Point ref_frame_cm = get_center_of_mass(ref_frame.frame);
    for (int i = 0; i < input_frames.size(); i++)
    {

        frameInfo frame = input_frames[i];
        cv::Point cm = get_center_of_mass(frame.planet_mask);
        frameInfo aligned_frame = frame;
        cv::Mat aligned_mat = get_aligned_by_centroid(frame.frame, cm, ref_frame_cm);
        aligned_frame.frame = aligned_mat;
        aligned_frames.push_back(aligned_frame);
    }
    return aligned_frames;
}

void Aligner::setRefFrame(frameInfo frame)
{
    ref_frame = frame;
};

std::vector<frameInfo> Aligner::getAligned()
{
    return aligned_frames;
}
