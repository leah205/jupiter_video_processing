#include <opencv2/opencv.hpp>

#include "align.h"
#include "helpers.h"

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

    cv::Point p(m.m10 / m.m00, m.m01 / m.m00);
    return p;
}

/**
 * @brief aligns frame
 *
 *  aligns frame so that its planet center of mass matches with that of the reference frame
 *
 * @param frame 8-bit single channel matrix
 * @param cm cv::Point center of mass
 * @param ref cv::Point center of mass of reference frame
 * @return cv::Mat aligned frame
 */
cv::Mat align_frame(cv::Mat frame, cv::Point cm, cv::Point ref)
{

    cv::Mat aligned_frame;

    double offset_x, offset_y;
    offset_x = cm.x - ref.x;
    offset_y = cm.y - ref.y;

    cv::Mat translation_matrix = (cv::Mat_<double>(2, 3) << 1, 0, -1 * offset_x, 0, 1, -1 * offset_y);
    int height = frame.cols;
    int width = frame.rows;

    cv::warpAffine(frame, aligned_frame, translation_matrix, cv::Size(width, height));
    return aligned_frame;
}

/**
 * @brief aligns best frames
 *
 * aligns all quality selected frames to the center of mass of reference frame
 *
 * @param frames all frames
 * @param ref_frame frame to align to
 * @param selected_indices indices of frames to align and stack
 * @return std::vector<cv::Mat>
 */

std::vector<cv::Mat> align_selected_to_ref(std::vector<cv::Mat> frames, cv::Mat ref_frame, std::vector<size_t> selected_indices)
{
    int frame_num = 600;
    cv::imshow("worst quality frame: ", frames[selected_indices[frame_num - 1]]);
    cv::waitKey(0);

    cv::imshow("best quality frame: ", frames[selected_indices[0]]);
    cv::waitKey(0);
    cv::Point ref_cm = get_center_of_mass(ref_frame);
    std::vector<cv::Mat> aligned_frames;
    size_t select_amount = selected_indices.size();
    for (size_t i = 0; i < select_amount; i++)
    {
        size_t index = selected_indices[i];
        cv::Mat frame = frames[index];
        cv::Point cm = get_center_of_mass(frame);
        cv::Mat aligned = align_frame(frame, cm, ref_cm);
        aligned_frames.push_back(aligned);
    }
    return aligned_frames;
}