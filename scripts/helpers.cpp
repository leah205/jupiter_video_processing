#include <opencv2/opencv.hpp>
#include "helpers.h"
#include <string>

/**
 * @brief print information on cv::Mat object
 *
 * @param frame
 */

void get_array_info(const cv::Mat frame)
{
    // 320
    std::cout << "rows: " << frame.rows << std::endl;
    // 320
    std::cout << "cols: " << frame.cols << std::endl;
    std::cout << "channels: " << frame.channels() << std::endl;
    // CV_8U (char) - 0
    std::cout << "data type: " << frame.type() << std::endl;
    std::cout << "depth: " << frame.depth() << std::endl;
}

/**
 * @brief Get the planet mask object
 *
 * Creates planet mask based on otsu intensity threshold which dynamically finds threshold
 * between pixel intensity peaks
 *
 * @param frame 8-bit one channel cv::Mat
 * @param mask binary mask indicating whether pixel falls on planet
 */
int first = 1;

cv::Mat get_planet_mask(const cv::Mat frame)
{

    cv::Mat otsu_mask;
    cv::Mat blurred;
    cv::GaussianBlur(frame, blurred, cv::Size(3, 3), 0);

    double val = cv::threshold(blurred, otsu_mask, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    // if (first)
    // {
    //     first = 0;
    //     cv::imwrite("planet_mask.png", otsu_mask);
    //     std::cout << "threshold value: " << val << std::endl;
    // }
    // std::cout << "threshold value: " << val << std::endl;

    return otsu_mask;
};

/**
 * @brief converts 3-channel gray frame to single channel by extracting first channel
 *
 * @param frame 3-channel matrix
 */
void extract_channel(cv::Mat &frame)
{
    cv::extractChannel(frame, frame, 0);
}

/**
 * @brief smooths mask
 *
 * gets rid of gaps on jupiter disc and isolated spots from space in mask
 *
 * @param mask one-channel matrix mask
 */

void smooth_mask(cv::Mat &mask)
{
    cv::Mat element = cv::getStructuringElement(
        cv::MORPH_ELLIPSE,
        cv::Size(5, 5));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, element);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, element);
}

/**
 * @brief Get the cropped rect object
 *
 * Gets the rect object that encompasses the mask's boundaries
 *
 * @param mask binary matrix mask
 * @return cv::Rect
 */

cv::Rect get_cropped_rect(const cv::Mat mask)
{

    cv::Rect rect = cv::boundingRect(mask);
    return rect;
}

/**
 * @brief Get the inner planet mask object
 *
 * gets mask around planet and then erodes mask at the limb using a kernel
 *
 * @param frame  single channel matrix object
 * @return cv::Mat
 */

cv::Mat get_inner_planet_mask(cv::Mat mask)
{

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(11, 11));
    cv::Mat innerMask = cv::Mat::zeros(mask.size(), CV_8UC1);
    cv::erode(mask, innerMask, kernel);

    return innerMask;
}

void save_image(std::string prefix, const cv::Mat frame)
{
    cv::imwrite("../output/" + prefix + ".png", frame);
}
