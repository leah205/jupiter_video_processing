#include <opencv2/opencv.hpp>
#include "helpers.h"

/**
 * @brief print information on cv::Mat object
 *
 * @param frame
 */

void get_array_info(cv::Mat frame)
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
 * Creates planet mask based on hard coded intensity threshold
 *
 * @param frame 8-bit one channel cv::Mat
 * @param mask binary mask indicating whether pixel falls on planet
 */

void salt(cv::Mat &image, int n)
{
    for (int k = 0; k < n; k++)
    {
        int i = rand() % image.rows;
        int j = rand() % image.cols;
        image.at<uchar>(j, i) = 255;
    }
}
void pepper(cv::Mat &image, int n)
{
    for (int k = 0; k < n; k++)
    {
        int i = rand() % image.rows;
        int j = rand() % image.cols;
        image.at<uchar>(j, i) = 0;
    }
}
cv::Mat get_planet_mask(cv::Mat frame)
{

    cv::Mat mask;
    cv::Mat blurred;
    cv::GaussianBlur(frame, blurred, cv::Size(3, 3), 0);
    cv::threshold(blurred, mask, 10, 255, cv::THRESH_BINARY);

    return mask;
};

void smooth_mask(cv::Mat &mask)
{
    cv::Mat element = cv::getStructuringElement(
        cv::MORPH_ELLIPSE,
        cv::Size(5, 5));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, element);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, element);
}

cv::Rect get_cropped_rect(cv::Mat mask)
{

    cv::Rect rect = cv::boundingRect(mask);
    return rect;
}
