#include <opencv2/opencv.hpp>

#ifndef SHARPEN_H
#define SHARPEN_H

extern "C"
{
#include "wavelib.h"
}

using SharpeningMap = std::map<int, int>;

void sharpen(const cv::Mat &image, cv::Mat &result);

class WaveletSharpener
{
private:
    wave_object obj;
    wt2_object wt;
    double *input;
    double *wavecoeffs;
    int decomp_levels;
    SharpeningMap level_sharpenings;

public:
    WaveletSharpener(int levels) : decomp_levels(levels) {
                                   };
    void decomposeImage();

    void runSharpening(const cv::Mat &image, cv::Mat &result);

    void sharpenLevel();

    void setLevelSharpening(const int level, const double amplification);
};

#endif