#include <opencv2/opencv.hpp>
#include "test_helpers.h"

/**
 * @brief adds white pixels throughout image randomly
 *
 * @param image
 * @param n number of white pixels to generate
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

/**
 * @brief adds black pixels throughout image randomly
 *
 * @param image
 * @param n number of black pixels to generate
 */
void pepper(cv::Mat &image, int n)
{
    for (int k = 0; k < n; k++)
    {
        int i = rand() % image.rows;
        int j = rand() % image.cols;
        image.at<uchar>(j, i) = 0;
    }
}

void synthetic_shift(const cv::Mat input, cv::Mat &result)
{
    // int offset_x = rand() % 20;
    // int offset_y = rand() % 20;

    int offset_x = 10;
    int offset_y = -8;
    cv::Mat translation_matrix = (cv::Mat_<double>(2, 3) << 1, 0, -1 * offset_x, 0, 1, -1 * offset_y);
    cv::warpAffine(input, result, translation_matrix, cv::Size(input.cols, input.rows));
}

cv::Mat generate_diff(const cv::Mat ref_frame, const cv::Mat frame)
{
    cv::Mat diff;
    double diff_min, diff_max;

    // cv::absdiff(ref_frame, frame, diff);
    diff = ref_frame - frame;
    cv::GaussianBlur(diff, diff, cv::Size(0, 0), 1);
    diff = diff * 20;
    cv::minMaxLoc(diff, &diff_min, &diff_max);
    // double scale = 127 / std::max(-diff_min, diff_max);
    double scale = 1;

    // cv::normalize(diff, diff, 0, 255, cv::NORM_MINMAX);
    diff.convertTo(diff, CV_8UC1, scale, 128);
    return diff;
};

cv::MatND Histogram1D::getHistogram(const cv::Mat &image)
{
    // returns 2d hue saturation histogram
    cv::Mat hist;

    cv::calcHist(&image, 1, channels, cv::Mat(), hist, 1, histSize, ranges);
    return hist;
};

cv::Mat Histogram1D::getHistogramImage(const cv::Mat &image)
{
    cv::MatND hist = getHistogram(image);
    // for (int i = 0; i < 130; i++)
    // {
    //     std::cout << "value " << i << "=" << hist.at<float>(i) << std::endl;
    // }

    double maxVal = 0;
    double minVal = 0;
    cv::minMaxLoc(hist, &minVal, &maxVal, 0, 0);
    cv::Mat histImg(histSize[0], histSize[0], CV_8U, cv::Scalar(255));
    int hpt = static_cast<int>(0.9 * histSize[0]);
    std::cout << "hist size" << histSize[0] << std::endl;
    for (int h = 0; h < histSize[0]; h++)
    {
        float binVal = hist.at<float>(h);
        int intensity = static_cast<int>(binVal * hpt / maxVal);
        cv::line(histImg, cv::Point(h, histSize[0]), cv::Point(h, histSize[0] - intensity), cv::Scalar::all(0));
    }
    return histImg;
}
