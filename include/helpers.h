#include <opencv2/opencv.hpp>
#include <filesystem>

namespace fs = std::filesystem;

cv::Mat get_planet_mask(const cv::Mat frame);
void get_array_info(const cv::Mat frame);
void smooth_mask(cv::Mat &mask);
cv::Rect get_cropped_rect(const cv::Mat mask);
void extract_channel(cv::Mat &frame);
cv::Mat get_inner_planet_mask(cv::Mat mask);
void subtract_background_signal(cv::Mat &frame);
// void save_sharpened_output(std::string prefix, const cv::Mat &frame, int sharpen_level, double sharpen_amp);
void save_output(std::string_view output, std::string_view prefix, const cv::Mat &frame);
static bool ends_with(std::string_view str, std::string_view suffix);
std::vector<std::string> get_avi_files(fs::path input_dir);
