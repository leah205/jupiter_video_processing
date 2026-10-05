#include <opencv2/opencv.hpp>

#ifndef LIMIT_H
#define LIMIT_H

#include "frameInfo.h"

typedef struct scoredFrame : frameInfo
{
    double score;
} scoredFrame;

class Limiter
{
private:
    enum metric
    {
        GRADIENT,
        LAPLACIAN
    } metric;
    int limit_frames_num;
    std::vector<frameInfo> limitedFrames;
    double get_avg_gradient_mag(cv::Mat frame, cv::Mat inner_mask, cv::Rect rect);
    double get_laplacian_variance(const cv::Mat frame, const cv::Mat inner_mask, const cv::Rect rect);
    double assess_frame_quality(frameInfo frame);

public:
    Limiter()
    {
        metric = GRADIENT;
    };
    Limiter(enum metric)
    {
        metric = metric;
    }
    std::vector<frameInfo> limitFrames(std::vector<frameInfo>);

    void setLimitFrameNum(int num);
};

#endif

// std::vector<size_t> get_sharpest_indices(std::vector<frameInfo> frames, int select_amount);
