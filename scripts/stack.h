
#include <opencv2/opencv.hpp>
#include "videoProcessor.h"

cv::Mat stack_frames(std::vector<cv::Mat> frames);
double get_avg_gradient_mag(cv::Mat frame, cv::Mat inner_mask, cv::Rect rect);
std::vector<size_t> get_sharpest_indices(std::vector<frameInfo> frames, int select_amount);
