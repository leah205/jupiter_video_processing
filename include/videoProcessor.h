#include <opencv2/opencv.hpp>
#include "align.h"
#include "limit.h"
#include "frameInfo.h"
#include "quality.h"
#include "sharpen.h"
#include <map>

#ifndef VIDEO_H
#define VIDEO_H

typedef struct
{
    float stack_percentage;
    WaveletSharpener sharpener;

} FilterConfig;

class VideoProcessor
{
private:
    static constexpr int wavelet_levels = 4;
    FrameQuality quality_scorer = FrameQuality(QualityMetric::GRADIENT);
    std::vector<frameInfo> frames;
    std::string input_dir;
    std::string output_dir;
    std::map<std::string, FilterConfig> stack_amounts = {};
    bool run_sharpening = true;
    WaveletSharpener sharpener;

public:
    VideoProcessor()
        : sharpener(wavelet_levels)

    {
    };

    frameInfo processFrame(cv::Mat frame) const;

    cv::Mat sharpenFrame(const cv::Mat &frame) const;

    void addSharpeningLevel(int level, double amplification);

    std::vector<frameInfo> selectFrames(const std::vector<frameInfo> &frames);

    std::vector<frameInfo> readFrames(const std::string input_path) const;

    void processVideo(const std::string_view input_path, const std::string_view output_path);

    cv::Mat stackFrames(const std::vector<frameInfo> &frames) const;

    void alignFrames(std::vector<frameInfo> &frames);

    void saveSharpenedOutput(std::string_view output_path, std::string_view output_prefix, cv::Mat frame) const;

    constexpr void setRunSharpening(const bool run);
};

#endif