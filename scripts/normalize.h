#include <opencv2/opencv.hpp>

void normalize_to_mean_intensity(cv::Mat &frame, double ref_mean);
double get_mean_intensity(cv::Mat frame);
