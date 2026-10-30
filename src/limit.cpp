#include <opencv2/opencv.hpp>
#include <numeric>
#include <iostream>

#include <cmath>
#include "limit.h"
#include "videoProcessor.h"
#include "helpers.h"

/**
 * @brief selects the best frames
 *
 * Uses scorer to score each frame and then returns the top limit_num frames
 *
 * @param scorer
 * @param frames
 * @param limit_num
 * @return std::vector<frameInfo>
 */

std::vector<frameInfo> selectFrames(const FrameQuality &scorer, const std::vector<frameInfo> &frames, int limit_num)
{
    size_t num_frames = frames.size();

    if (limit_num > num_frames)
    {
        throw std::runtime_error("frame limit is larger than number of frames");
    }
    std::vector<frameInfo> selected_frames;
    std::vector<size_t> sorted_i(num_frames);
    std::vector<scoredFrame> scoredFrames;

    for (size_t i = 0; i < num_frames; i++)
    {
        scoredFrame new_frame{frames[i], scorer.score(frames[i])};
        scoredFrames.push_back(new_frame);
    }

    std::iota(sorted_i.begin(), sorted_i.end(), 0);
    std::sort(sorted_i.begin(), sorted_i.end(), [&](size_t a, size_t b)
              { return scoredFrames[a].score > scoredFrames[b].score; });

    for (int i = 0; i < limit_num; i++)
    {
        selected_frames.push_back(static_cast<frameInfo>(scoredFrames[sorted_i[i]]));
    }
    return selected_frames;
}
