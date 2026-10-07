#include <opencv2/opencv.hpp>

#ifndef LIMIT_H
#define LIMIT_H

#include "frameInfo.h"
#include "quality.h"

typedef struct scoredFrame : frameInfo
{
    double score;
} scoredFrame;

class Limiter
{
private:
    const FrameQuality &quality;
    int limit_frames_num;
    std::vector<frameInfo> limitedFrames;

public:
    Limiter(const FrameQuality &q)
    {
        quality = q;
    };

    std::vector<frameInfo> limitFrames(std::vector<frameInfo>);

    void setLimitFrameNum(int num);
    void compare_methods(std::vector<frameInfo> frames);
};

#endif

// std::vector<size_t> get_sharpest_indices(std::vector<frameInfo> frames, int select_amount);
