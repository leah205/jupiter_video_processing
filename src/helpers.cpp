#include <opencv2/opencv.hpp>
#include "helpers.h"
#include <string>
#include <filesystem>

static bool ends_with(std::string_view str, std::string_view suffix)
{
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::string> get_avi_files(fs::path input_dir)
{
    std::vector<std::string> filepaths;

    for (auto &entry : fs::directory_iterator(input_dir))
    {
        if (entry.is_regular_file())
        {
            std::string name = entry.path().filename().string();
            if (ends_with(name, ".avi"))
            {
                filepaths.push_back(input_dir.string() + "/" + name);
            }
        }
    }
    return filepaths;
}

/**
 * @brief print information on cv::Mat object
 *
 * @param frame
 */

void get_array_info(const cv::Mat frame)
{
    // 320
    std::cout << "rows: " << frame.rows << std::endl;
    // 320
    std::cout << "cols: " << frame.cols << std::endl;
    std::cout << "channels: " << frame.channels() << std::endl;
    // CV_8U (char) - 0
    std::cout << "data type: " << frame.type() << std::endl;
    std::cout << "depth: " << frame.depth() << std::endl;
}

/**
 * @brief Get the planet mask object
 *
 * Creates planet mask based on otsu intensity threshold which dynamically finds threshold
 * between pixel intensity peaks
 *
 * @param frame 8-bit one channel cv::Mat
 * @param mask binary mask indicating whether pixel falls on planet
 */
int first = 1;

cv::Mat get_planet_mask(const cv::Mat frame)
{

    cv::Mat otsu_mask;
    cv::Mat blurred;
    cv::GaussianBlur(frame, blurred, cv::Size(3, 3), 0);

    cv::threshold(blurred, otsu_mask, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    return otsu_mask;
};

/**
 * @brief converts 3-channel gray frame to single channel by extracting first channel
 *
 * @param frame 3-channel matrix
 */
void extract_channel(cv::Mat &frame)
{
    cv::extractChannel(frame, frame, 0);
}

/**
 * @brief smooths mask
 *
 * gets rid of gaps on jupiter disc and isolated spots from space in mask
 *
 * @param mask one-channel matrix mask
 */

void smooth_mask(cv::Mat &mask)
{
    cv::Mat element = cv::getStructuringElement(
        cv::MORPH_ELLIPSE,
        cv::Size(5, 5));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, element);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, element);
}

/**
 * @brief Get the cropped rect object
 *
 * Gets the rect object that encompasses the mask's boundaries
 *
 * @param mask binary matrix mask
 * @return cv::Rect
 */

cv::Rect get_cropped_rect(const cv::Mat mask)
{

    cv::Rect rect = cv::boundingRect(mask);
    return rect;
}

/**
 * @brief Get the inner planet mask object
 *
 * gets mask around planet and then erodes mask at the limb using a kernel
 *
 * @param frame  single channel matrix object
 * @return cv::Mat
 */

cv::Mat get_inner_planet_mask(cv::Mat mask)
{

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(11, 11));
    cv::Mat innerMask = cv::Mat::zeros(mask.size(), CV_8UC1);
    cv::erode(mask, innerMask, kernel);

    return innerMask;
}

void save_output(std::string_view output, std::string_view prefix, const cv::Mat &frame)
{
    std::string output_file{output};
    output_file += std::string{prefix} + ".png";
    cv::imwrite(output_file, frame);
}

/**
 * @brief subtract background signal from frame
 *
 * uses mask to find the average background pixel intensity, and subtracts this value from all of the pixels in the frame
 *
 * @param frame frame to be modified in-place
 */
void subtract_background_signal(cv::Mat &frame)
{

    cv::Mat planet_mask = get_planet_mask(frame);
    cv::Mat background_mask;
    cv::bitwise_not(planet_mask, background_mask);
    double background_signal = cv::mean(frame, background_mask)[0];
    // std::cout << "correcting for detected background signal of " << background_signal << std::endl;
    cv::subtract(frame, background_signal, frame);
}