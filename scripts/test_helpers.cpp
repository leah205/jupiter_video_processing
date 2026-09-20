#include <opencv2/opencv.hpp>

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