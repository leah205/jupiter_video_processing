#include <opencv2/opencv.hpp>
#include <numeric>
#include <iostream>

#include <cmath>

#include "stack.h"
#include "align.h"
#include "helpers.h"
#include "optimisation.h"
#include "videoProcessor.h"

void VideoProcessor::setRef(int index)
{
    if (index < 0 || index > frames.size())
    {
        throw std::invalid_argument("ref index must be in correct range");
    }
    ref_index = index;
}

void VideoProcessor::addFrame(cv::Mat &frame)
{
    frameInfo newFrame;
    extract_channel(frame);
    cv::Mat inner_mask = get_inner_planet_mask(frame);
    cv::Rect rect = get_cropped_rect(frame);
    double quality_score = get_avg_gradient_mag(frame, inner_mask, rect);
    newFrame.frame = frame;
    newFrame.quality_score = quality_score;
    frames.push_back(newFrame);
};

void VideoProcessor::setFrameStackNum(int frame_num)
{
    stacked_frames_num = frame_num;
};

void VideoProcessor::selectFramesByGradient()
{
    quality_sorted_indices.resize(frames.size());

    std::iota(quality_sorted_indices.begin(), quality_sorted_indices.end(), 0);
    std::sort(quality_sorted_indices.begin(), quality_sorted_indices.end(), [&](size_t a, size_t b)
              { return frames[a].quality_score > frames[b].quality_score; });

    std::vector<size_t> selected_indices(quality_sorted_indices.begin(), quality_sorted_indices.begin() + stacked_frames_num);
    selected_frames = selected_indices;
    // selected_frames = get_sharpest_indices(frames, stacked_frames_num);
};

void VideoProcessor::alignSelectedFramesByCentroid()
{
    frameInfo &ref_frame = frames[ref_index];
    for (int i = 0; i < selected_frames.size(); i++)
    {
        frameInfo frame = frames[selected_frames[i]];
        frame.cm = get_center_of_mass(frame.frame);
        aligned_frames.push_back(get_aligned_by_centroid(frame, ref_frame.cm));
    }
};

void VideoProcessor::stackAlignedFrames()
{
    std::cout << "yoohoo" << std::endl;
    stack_frames(aligned_frames);
}
