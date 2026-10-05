
#ifndef STACK_H
#define STACK_H

#include <opencv2/opencv.hpp>
#include "videoProcessor.h"
#include "frameInfo.h"

// class Stacker
// {
//     private:

// }
#endif

cv::Mat stack_frames(std::vector<frameInfo> frames);
