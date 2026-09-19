#include <opencv2/opencv.hpp>

// subtract laplacian from image
// sharpened_pizel = 5 * current-left-right-up-down

void sharpen(const cv::Mat &image, cv::Mat &result)
{
    // input must be 8-bit

    cv::Mat kernel(3, 3, CV_32F, cv::Scalar(0));

    kernel.at<float>(1, 1) = 5.0;
    kernel.at<float>(0, 1) = -1.0;
    kernel.at<float>(2, 1) = -1.0;
    kernel.at<float>(1, 0) = -1.0;
    kernel.at<float>(1, 2) = -1.0;

    cv::filter2D(image, result, image.depth(), kernel);
}