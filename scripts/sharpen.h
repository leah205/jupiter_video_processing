#include <opencv2/opencv.hpp>

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

public:
    WaveletSharpener() {

    };

    void sharpenImage(const cv::Mat &image, cv::Mat &result);
};