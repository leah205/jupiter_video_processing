#include <opencv2/opencv.hpp>

#include "videoProcessor.h"

frameInfo get_aligned_by_centroid(frameInfo frameObj, cv::Point ref);
std::vector<cv::Mat> align_selected_to_ref(std::vector<cv::Mat> frames, cv::Mat ref_frame, std::vector<size_t> selected_indices);
cv::Point get_center_of_mass(cv::Mat frame);
