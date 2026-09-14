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