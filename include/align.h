#include <opencv2/opencv.hpp>

#ifndef ALIGN_H
#define ALIGN_H

#include "frameInfo.h"

cv::Mat get_aligned_by_centroid(cv::Mat frame, cv::Point cm, cv::Point ref);
cv::Point get_center_of_mass(const cv::Mat mask);

class Aligner
{
private:
    frameInfo ref_frame;
    cv::Point ref_cm;

public:
    void setRefFrame(frameInfo frame);
    frameInfo alignFrame(frameInfo frame);
};

#endif
