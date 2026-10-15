#include <opencv2/opencv.hpp>

#ifndef FRAME_H

#define FRAME_H
typedef struct
{
    cv::Mat frame;
    double quality_score;
    cv::Point cm;
    cv::Mat planet_mask;
} frameInfo;

#endif
