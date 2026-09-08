#include <opencv2/opencv.hpp>

/**
 * @brief Get the planet mask object
 *
 * Creates planet mask based on hard coded intensity threshold
 *
 * @param frame
 * @param mask
 */

cv::Mat get_planet_mask(cv::Mat frame)
{
    cv::Mat mask;
    cv::Mat blurred;
    // should i blur here?
    // cv::cvtColor(gray, bgr, cv::COLOR_GRAY2BGR);
    cv::GaussianBlur(frame, blurred, cv::Size(3, 3), 0);
    cv::threshold(blurred, mask, 10, 255, cv::THRESH_BINARY);
    return mask;
};