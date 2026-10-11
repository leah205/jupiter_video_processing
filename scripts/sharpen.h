#include <opencv2/opencv.hpp>

#ifndef SHARPEN_H
#define SHARPEN_H

extern "C"
{
#include "wavelib.h"
}

void sharpen(const cv::Mat &image, cv::Mat &result);

class WaveletSharpener
{
private:
    wave_object obj;
    wt2_object wt;
    double *input;
    int decomp_levels;

public:
    WaveletSharpener(int levels) : decomp_levels(levels) {

                                   };

    void sharpenImage(const cv::Mat &image, cv::Mat &result);

    void setLevelSharpening(const int level, const int amplification);
};

#endif