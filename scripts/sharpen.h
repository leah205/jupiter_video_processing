#include <opencv2/opencv.hpp>

#ifndef SHARPEN_H
#define SHARPEN_H

extern "C"
{
#include "wavelib.h"
}

using SharpeningMap = std::map<int, int>;

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
    wave_object obj;
    double *input;
    double *wavecoeffs;
    SharpeningConfig config;

public:
    WaveletSharpener(int levels = 6, std::string method = "db4") : config{.decomp_levels = levels, .method = method}
    {
        obj = wave_init(method.c_str());
    };
    void decomposeImage();

    void runSharpening(const cv::Mat &image, cv::Mat &result) const;

    void sharpenLevel();

    const SharpeningConfig &getSharpenConfig() const;

    void setLevelSharpening(const int level, const double amplification);
};

#endif