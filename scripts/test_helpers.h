#include <opencv2/opencv.hpp>

void synthetic_shift(const cv::Mat input, cv::Mat &result);

cv::Mat generate_diff(const cv::Mat ref_frame, const cv::Mat frame);

class Histogram1D
{
private:
    int histSize[1];
    float hranges[2];
    const float *ranges[1];
    int channels[1];

public:
    Histogram1D()
    {
        histSize[0] = 130;
        hranges[0] = 0.0;
        hranges[1] = 130.0;
        ranges[0] = hranges;
        channels[0] = 0;
    };
    cv::MatND getHistogram(const cv::Mat &image);
    cv::Mat getHistogramImage(const cv::Mat &image);
};
