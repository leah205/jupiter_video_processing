#include <opencv2/opencv.hpp>

#ifndef LIMIT_H
#define LIMIT_H

#include "frameInfo.h"
#include "quality.h"

typedef struct scoredFrame : frameInfo
{
    double score;
} scoredFrame;

std::vector<frameInfo> selectFrames(const FrameQuality &scorer, const std::vector<frameInfo> &frames, int limit_num);

#endif
