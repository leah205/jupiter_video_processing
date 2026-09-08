#include <opencv2/opencv.hpp>

cv::Mat align_frame(cv::Mat frame, cv::Point cm, cv::Point ref);
std::vector<cv::Mat> align_selected_to_ref(std::vector<cv::Mat> frames, cv::Mat ref_frame, std::vector<size_t> selected_indices);
