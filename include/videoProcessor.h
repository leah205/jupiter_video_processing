#include <opencv2/opencv.hpp>
#include "align.h"
#include "limit.h"
#include "frameInfo.h"
#include "quality.h"
#include "sharpen.h"
#include <map>
#include <nlohmann/json.hpp>

using nlohmann::json;

#ifndef VIDEO_H
#define VIDEO_H

enum class SharpenPreset
{
    DEFAULT,
    COARSE,
    COUNT
};

// typedef struct FilterConfig
// {
//     std::vector<WaveletSharpener> sharpen_config{};
//     int stack_perc{};
// } FilterConfig;

class VideoProcessor
{
private:
    static constexpr int wavelet_levels = 4;
    FrameQuality quality_scorer = FrameQuality(QualityMetric::GRADIENT);
    std::vector<frameInfo> frames;
    std::string input_dir;
    std::string output_dir;
    std::map<std::string, float> select_config = {};
    std::map<std::string, WaveletSharpener> filter_sharpen_config = {};
    std::vector<SharpeningMap> sharpen_configs{static_cast<int>(SharpenPreset::COUNT), SharpeningMap{}};
    // std::vector<FilterConfig> filter_configs;
    std::vector<std::string> m_filters;
    bool run_sharpening = true;
    WaveletSharpener sharpener;
    typedef struct VideoData
    {
        int total_frames{};
        int num_stacked{};
        double stack_percentage{};
        std::string filter{};
        std::string sharpening{};
        std::string date_prefix{};

    } VideoData;

public:
    VideoProcessor() : sharpener(wavelet_levels)

    {
        sharpen_configs[static_cast<int>(SharpenPreset::DEFAULT)] = {{2, 6.0}, {3, 5.0}, {4, 2.0}};
        sharpen_configs[static_cast<int>(SharpenPreset::COARSE)] = {{3, 3.0}, {4, 2.0}};
        filter_sharpen_config["default"] = WaveletSharpener{wavelet_levels};
        setSharpening(SharpenPreset::DEFAULT);
    };

    frameInfo processFrame(cv::Mat frame) const;

    void saveOutput(const json metadata, const cv::Mat &frame, const cv::Mat &sharpened_frame, const std::string_view output) const;

    void setSharpening(const enum SharpenPreset, const std::vector<std::string> &filters = {});

    void setSharpening(const SharpeningMap &sharpen_map, const std::vector<std::string> &filters = {});

    void setStackPercentage(float percentage);

    void setStackPercentage(float percentage, std::vector<std::string> filters);

    std::vector<frameInfo> readFrames(const std::string input_path) const;

    void processVideo(const std::string_view input_path, const std::string_view output_path);

    cv::Mat stackFrames(const std::vector<frameInfo> &frames) const;

    void alignFrames(std::vector<frameInfo> &frames);

    void setFilters(const std::vector<std::string> &filters);

    std::vector<std::string> getFilters(void) const;

    constexpr void setRunSharpening(const bool run);
};

#endif