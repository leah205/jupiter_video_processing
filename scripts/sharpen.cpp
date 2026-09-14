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

    // result.create(image.size(), image.type());
    // for (int j = 1; j < image.rows - 1; j++)
    // {
    //     const uchar *previous = image.ptr<const uchar>(j - 1); // previous row
    //     const uchar *current = image.ptr<const uchar>(j);
    //     const uchar *next = image.ptr<const uchar>(j + 1);

    //     uchar *output = result.ptr<uchar>(j);

    //     for (int i = 1; i < image.cols - 1; i++)
    //     {
    //         *output++ = cv::saturate_cast<uchar>(5 * current[i] - current[i - 1] - current[i + 1] - previous[i] - next[i]);
    //     }
    // }
    // result.row(0).setTo(cv::Scalar(0));
    // result.row(result.rows - 1).setTo(cv::Scalar(0));
    // result.col(0).setTo(cv::Scalar(0));
    // result.col(result.cols - 1).setTo(cv::Scalar(result.cols - 1));
}