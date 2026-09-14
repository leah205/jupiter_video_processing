#include <opencv2/opencv.hpp>

cv::Mat get_planet_mask(const cv::Mat frame);
void get_array_info(const cv::Mat frame);
void smooth_mask(cv::Mat &mask);
cv::Rect get_cropped_rect(const cv::Mat mask);
void extract_channel(cv::Mat &frame);
cv::Mat get_inner_planet_mask(cv::Mat mask);
