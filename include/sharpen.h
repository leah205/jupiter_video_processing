#include <opencv2/opencv.hpp>

#ifndef SHARPEN_H
#define SHARPEN_H

extern "C"
{
#include "wavelib.h"
}

using SharpeningMap = std::map<int, double>;

struct SharpeningConfig
{
    int decomp_levels;
    std::string method;
    SharpeningMap level_sharpenings;
};

void sharpen(const cv::Mat &image, cv::Mat &result);

class WaveletSharpener
{
private:
    SharpeningConfig config;

public:
    WaveletSharpener(int levels = 4, std::string method = "db4") : config{.decomp_levels = levels,
                                                                          .method = method,
                                                                          .level_sharpenings = {}} {
                                                                   };

    void runSharpening(const cv::Mat &image, cv::Mat &result) const;

    const SharpeningConfig &getSharpenConfig() const;

    void addLevelSharpening(const int level, const double amplification);
};

#endif