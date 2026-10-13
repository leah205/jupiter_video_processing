#include <opencv2/opencv.hpp>

#ifndef SHARPEN_H
#define SHARPEN_H

extern "C"
{
#include "wavelib.h"
}

using LevelSharpening = std::pair<int, double>;

void sharpen(const cv::Mat &image, cv::Mat &result);

class WaveletSharpener
{
private:
    wave_object obj;
    wt2_object wt;
    double *input;
    double *wavecoeffs;
    int decomp_levels;
    std::vector<LevelSharpening> level_sharpenings;

public:
    WaveletSharpener(int levels) : decomp_levels(levels) {
                                   };
    void decomposeImage();

    void runSharpening(const cv::Mat &image, cv::Mat &result);

    void sharpenLevel();

    void setLevelSharpening(const int level, const double amplification);
};

#endif