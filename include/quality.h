
#include <opencv2/opencv.hpp>
#ifndef QUALITY_H
#define QUALITY_H
#include "frameInfo.h"

double get_avg_gradient_mag(cv::Mat frame, cv::Mat inner_mask, cv::Rect rect);
double get_laplacian_variance(const cv::Mat frame, const cv::Mat inner_mask, const cv::Rect rect);

enum class QualityMetric
{
    GRADIENT,
    LAPLACIAN
};

class FrameQuality
{
private:
    QualityMetric m_metric;

public:
    FrameQuality(QualityMetric metric) : m_metric(metric)
    {
    }

    double score(const frameInfo &frame) const;
};
#endif