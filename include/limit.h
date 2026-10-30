#include <opencv2/opencv.hpp>

#ifndef LIMIT_H
#define LIMIT_H

#include "frameInfo.h"
#include "quality.h"

typedef struct scoredFrame : frameInfo
{
    double score;
} scoredFrame;

// class Limiter
// {
// private:
//     const FrameQuality &quality_scorer;
//     int limit_frames_num;
//     std::vector<frameInfo> limitedFrames;

// public:
//     Limiter(const FrameQuality &q) : quality_scorer(q) {};

//     std::vector<frameInfo> limitFrames(std::vector<frameInfo>);

//     void setLimitFrameNum(int num);
// };

std::vector<frameInfo> selectFrames(const FrameQuality &scorer, const std::vector<frameInfo> &frames, int limit_num);

#endif
