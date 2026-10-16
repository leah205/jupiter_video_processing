#include <opencv2/opencv.hpp>

#ifndef OP_HEADER
#define OP_HEADER

double transform_ecc(const cv::Mat ref_frame, const cv::Mat aligned_frame, cv::Mat &new_aligned);
double compute_ecc(const cv::Mat ref_frame, const cv::Mat aligned_frame);

#endif
