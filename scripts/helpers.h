#include <opencv2/opencv.hpp>

cv::Mat get_planet_mask(cv::Mat frame);
void get_array_info(cv::Mat frame);
void smooth_mask(cv::Mat &mask);
cv::Rect get_cropped_rect(cv::Mat mask);
