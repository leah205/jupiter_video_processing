#include <opencv2/opencv.hpp>
#include <numeric>
#include <iostream>

#include <cmath>
#include "limit.h"
#include "videoProcessor.h"
#include "test_helpers.h"
#include "helpers.h"

// double Limiter::assess_frame_quality(frameInfo frame)
// {
//     double score;
//     cv::Mat inner_mask = get_inner_planet_mask(frame.planet_mask);
//     cv::Rect rect = get_cropped_rect(frame.frame);
//     switch (metric)
//     {
//     case GRADIENT:

//         score = get_avg_gradient_mag(frame.frame, inner_mask, rect);
//         break;
//     case LAPLACIAN:
//         score = get_laplacian_variance(frame.frame, inner_mask, rect);
//         break;
//     default:
//         score = get_avg_gradient_mag(frame.frame, inner_mask, rect);
//     }
//     return score;
// }

std::vector<frameInfo> Limiter::limitFrames(std::vector<frameInfo> frames)
{
    size_t num_frames = frames.size();
    std::vector<frameInfo> selected_frames;
    std::vector<size_t> sorted_i(num_frames);
    std::vector<scoredFrame> scoredFrames;

    for (int i = 0; i < frames.size(); i++)
    {
        scoredFrame new_frame{frames[i], assess_frame_quality(frames[i])};
        scoredFrames.push_back(new_frame);
    }
    // sorted_l.resize(frames.size());
    // std::cout << "frame quality score" << frames[0].quality_score << std::endl;

    std::iota(sorted_i.begin(), sorted_i.end(), 0);
    std::sort(sorted_i.begin(), sorted_i.end(), [&](size_t a, size_t b)
              { return scoredFrames[a].score > scoredFrames[b].score; });

    // std::vector<size_t> selected_indices(sorted_l.begin(), sorted_l.begin() + limit_frames_num);
    for (int i = 0; i < limit_frames_num; i++)
    {
        selected_frames.push_back(static_cast<frameInfo>(scoredFrames[sorted_i[i]]));
    }
    return selected_frames;
}

void Limiter::setLimitFrameNum(int num)
{
    limit_frames_num = num;
}

void Limiter::compare_methods(std::vector<frameInfo> frames)
{
    size_t num_frames = frames.size();
    std::vector<size_t> sorted_l(num_frames), sorted_g(num_frames);
    std::vector<scoredFrame> scoredFramesL;
    std::vector<scoredFrame> scoredFramesG;
    metric = GRADIENT;
    for (int i = 0; i < frames.size(); i++)
    {
        scoredFrame new_frame{frames[i], assess_frame_quality(frames[i])};
        scoredFramesG.push_back(new_frame);
    }
    metric = LAPLACIAN;
    for (int i = 0; i < frames.size(); i++)
    {
        scoredFrame new_frame{frames[i], assess_frame_quality(frames[i])};
        scoredFramesL.push_back(new_frame);
    }
    // sorted_l.resize(frames.size());
    // std::cout << "frame quality score" << frames[0].quality_score << std::endl;

    std::iota(sorted_l.begin(), sorted_l.end(), 0);
    std::sort(sorted_l.begin(), sorted_l.end(), [&](size_t a, size_t b)
              { return scoredFramesL[a].score > scoredFramesL[b].score; });

    std::iota(sorted_g.begin(), sorted_g.end(), 0);
    std::sort(sorted_g.begin(), sorted_g.end(), [&](size_t a, size_t b)
              { return scoredFramesG[a].score > scoredFramesG[b].score; });
    std::cout << "frame quality score" << sorted_g[10] << std::endl;
    std::cout << "frame quality score" << sorted_l[10] << std::endl;

    getStackSame(sorted_g, sorted_l, limit_frames_num);
}
