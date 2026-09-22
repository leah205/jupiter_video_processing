#include <opencv2/opencv.hpp>

void synthetic_shift(const cv::Mat input, cv::Mat &result);

cv::Mat generate_diff(const cv::Mat ref_frame, const cv::Mat frame);
