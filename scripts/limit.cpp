#include <opencv2/opencv.hpp>
#include <numeric>
#include <iostream>

#include <cmath>
#include "limit.h"
#include "videoProcessor.h"
#include "helpers.h"

double Limiter::get_laplacian_variance(const cv::Mat frame, const cv::Mat inner_mask, const cv::Rect rect)
{
    cv::Mat croppedFrame = frame(rect);
    cv::Mat croppedInnerMask = inner_mask(rect);
    cv::Mat lap_frame;
    cv::Scalar mean, stddev;
    // cv::imshow("cropped inner mask", croppedInnerMask);
    // cv::waitKey(0);
    cv::Laplacian(frame, lap_frame, CV_8UC1);
    cv::meanStdDev(lap_frame, mean, stddev, croppedInnerMask);
    return stddev[0] * stddev[0];
}

/**
 * @brief Get the avg gradient mag
 *
 * computes the average gradient magnitude of pixels designated by a mask within a cropped region of a frame
 *
 * @param frame 8-bit single channel matrix
 * @param inner_mask 8-bit single channel mask for pixels to be used in quality computation
 * @param rect object for cropping frame around disc
 * @return double of average gradient magnitude of all pixels in frame
 */
double Limiter::get_avg_gradient_mag(const cv::Mat frame, const cv::Mat inner_mask, const cv::Rect rect)
{
    // gets cropped frame around disc
    cv::Mat croppedFrame = frame(rect);
    // gets cropped mask around disc
    cv::Mat croppedInnerMask = inner_mask(rect);

    cv::Mat magx_frame;
    cv::Mat magy_frame;

    cv::Sobel(croppedFrame, magx_frame, CV_32FC1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::Sobel(croppedFrame, magy_frame, CV_32FC1, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);

    cv::Mat mag;
    cv::magnitude(magx_frame, magy_frame, mag);
    double avg_mag = (double)cv::mean(mag, croppedInnerMask)[0];

    return avg_mag;
}

double Limiter::assess_frame_quality(frameInfo frame)
{
    double score;
    cv::Mat inner_mask = get_inner_planet_mask(frame.planet_mask);
    cv::Rect rect = get_cropped_rect(frame.frame);
    switch (metric)
    {
    case GRADIENT:
        score = get_avg_gradient_mag(frame.frame, inner_mask, rect);
        break;
    case LAPLACIAN:
        score = get_laplacian_variance(frame.frame, inner_mask, rect);
        break;
    default:
        score = get_avg_gradient_mag(frame.frame, inner_mask, rect);
    }
    return score;
}

std::vector<frameInfo> Limiter::limitFrames(std::vector<frameInfo> frames)
{
    size_t num_frames = frames.size();
    std::vector<frameInfo> selected_frames;
    std::vector<size_t> quality_sorted_indices(num_frames);
    std::vector<scoredFrame> scoredFrames;

    for (int i = 0; i < frames.size(); i++)
    {
        scoredFrame new_frame{frames[i], assess_frame_quality(frames[i])};
        scoredFrames.push_back(new_frame);
    }
    // quality_sorted_indices.resize(frames.size());
    // std::cout << "frame quality score" << frames[0].quality_score << std::endl;

    std::iota(quality_sorted_indices.begin(), quality_sorted_indices.end(), 0);
    std::sort(quality_sorted_indices.begin(), quality_sorted_indices.end(), [&](size_t a, size_t b)
              { return scoredFrames[a].score > scoredFrames[b].score; });

    // std::vector<size_t> selected_indices(quality_sorted_indices.begin(), quality_sorted_indices.begin() + limit_frames_num);
    for (int i = 0; i < limit_frames_num; i++)
    {
        std::cout << quality_sorted_indices[i] << std::endl;
        selected_frames.push_back(static_cast<frameInfo>(scoredFrames[quality_sorted_indices[i]]));
    }
    return selected_frames;
}

void Limiter::setLimitFrameNum(int num)
{
    limit_frames_num = num;
}