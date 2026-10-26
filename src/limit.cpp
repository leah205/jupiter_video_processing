#include <opencv2/opencv.hpp>
#include <numeric>
#include <iostream>

#include <cmath>
#include "limit.h"
#include "videoProcessor.h"
#include "helpers.h"

/**
 * @brief gets the highest quality frames
 *
 * computes a score for each frame and gets the limit_frames_num best frames
 *
 * @param frames
 * @return std::vector<frameInfo>
 */

std::vector<frameInfo> Limiter::limitFrames(std::vector<frameInfo> frames)
{
    size_t num_frames = frames.size();

    if (limit_frames_num > num_frames)
    {
        throw std::runtime_error("frame limit is larger than number of frames");
    }
    std::vector<frameInfo> selected_frames;
    std::vector<size_t> sorted_i(num_frames);
    std::vector<scoredFrame> scoredFrames;

    for (size_t i = 0; i < num_frames; i++)
    {
        scoredFrame new_frame{frames[i], quality_scorer.score(frames[i])};
        scoredFrames.push_back(new_frame);
    }

    std::iota(sorted_i.begin(), sorted_i.end(), 0);
    std::sort(sorted_i.begin(), sorted_i.end(), [&](size_t a, size_t b)
              { return scoredFrames[a].score > scoredFrames[b].score; });

    for (int i = 0; i < limit_frames_num; i++)
    {
        selected_frames.push_back(static_cast<frameInfo>(scoredFrames[sorted_i[i]]));
    }
    return selected_frames;
}
/**
 * @brief
 *
 * @param num
 */

void Limiter::setLimitFrameNum(int num)
{
    limit_frames_num = num;
}