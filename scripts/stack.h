
#include <opencv2/opencv.hpp>

cv::Mat stack_images(std::vector<cv::Mat> frames, std::vector<size_t> selected_indices);
double get_avg_gradient_mag(cv::Mat frame);
std::vector<size_t> get_selected_indices(std::vector<cv::Mat> frames, std::vector<double> quality_score_vec, int select_amount);
