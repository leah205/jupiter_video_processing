#include <opencv2/opencv.hpp>
#include <numeric>
#include <iostream>
#include <optional>
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <fstream>

#include <cmath>

#include "stack.h"
#include "align.h"
#include "helpers.h"
#include "videoProcessor.h"
#include "frameInfo.h"

#include "sharpen.h"

using nlohmann::json;

/**
 * @brief processes frame and returns frameInfo object
 *
 * changes frame data type to be one channel. Returns binary planet mask and frame in struct object
 *
 * @param frame
 */
frameInfo VideoProcessor::processFrame(cv::Mat frame) const
{
    frameInfo newFrame;
    extract_channel(frame);
    cv::Mat planet_mask = get_planet_mask(frame);
    smooth_mask(planet_mask);
    newFrame.frame = frame;
    newFrame.planet_mask = planet_mask;
    return newFrame;
    // frames.push_back(newFrame);
};

/**
 * @brief aligns frames to reference
 *
 * aligns frames to contain matrices of sharpest frames
 * with centroid aligned to first frame
 *
 * @param frames
 */
void VideoProcessor::alignFrames(std::vector<frameInfo> &frames)
{
    Aligner aligner{};
    std::cout << "aligning frames..." << std::endl;
    std::vector<frameInfo> aligned_frames{};
    frameInfo ref_frame = frames[0];
    aligner.setRefFrame(ref_frame);
    for (size_t i = 1; i < frames.size(); i++)
    {
        frames[i] = aligner.alignFrame(frames[i]);
    }
};

/**
 * @brief stacks selected aligned frames
 *
 * returns 16-bit stacked frame with pixel intensities averaged among aligned selected frames
 *
 * @param frames
 */

cv::Mat VideoProcessor::stackFrames(const std::vector<frameInfo> &frames) const
{
    cv::Mat output;

    std::cout << "stacking " << frames.size() << " frames..." << std::endl;
    output = stack_frames(frames);

    return output;
}

/**

/**
 * @brief reads frames from .avi file into vector of processed frameInfo objects
 *
 * @param input_path
 * @return std::vector<frameInfo>
 */

std::vector<frameInfo> VideoProcessor::readFrames(const std::string input_path) const
{
    bool ret;
    cv::Mat frame;
    std::vector<frameInfo> frames;
    cv::VideoCapture cap(input_path);
    int frame_num = cap.get(cv::CAP_PROP_FRAME_COUNT);
    int fps = cap.get(cv::CAP_PROP_FPS);

    if (!cap.isOpened())
    {
        throw std::runtime_error("Error: video object was not opened successfully");
    }

    while (true)
    {

        ret = cap.read(frame);

        if (!ret)
        {
            break;
        }

        frames.push_back(processFrame(frame));
    }
    return frames;
}

/**
 * @brief Runs pipeline lucky imaging pipeline
 *
 * reads video, selects 25% highest quality frames, aligns them to highest quality frame, stacks them,
 * corrects for background noise, applies wavelet sharpening to frame, and saves sharpened and unsharpened frame
 *
 *
 * @param input_path
 * @param output_path
 */

void VideoProcessor::processVideo(const std::string_view input_path, const std::string_view output_path)
{
    std::size_t pos = input_path.find_last_of('/');
    std::string_view filename = input_path.substr(pos + 1);
    cv::Mat sharpened_frame{};
    std::string filter = get_filter(input_path);
    std::vector<frameInfo>
        frames = readFrames(std::string{input_path});
    // cv::imshow("first frame unprocessed", frames[0].frame);
    // cv::waitKey(0);
    if (!frames.size())
    {
        throw std::runtime_error("frames vector is empty for: " + std::string{filename});
    }

    int stacked_frames_num;
    float stack_perc = (select_config.count(filter)) ? select_config[filter] : select_config["default"];
    const WaveletSharpener &sharpener = (filter_sharpen_config.count(filter)) ? filter_sharpen_config[filter] : filter_sharpen_config["default"];
    stacked_frames_num = ceil(stack_perc * frames.size());
    std::cout << "selecting " + std::to_string(stack_perc) + " of frames" << std::endl;
    std::vector<frameInfo> selected_frames = ::selectFrames(quality_scorer, frames, stacked_frames_num);
    std::cout << "select num: " << selected_frames.size() << std::endl;
    alignFrames(selected_frames);

    cv::Mat stacked_frame = stackFrames(selected_frames);
    subtract_background_signal(stacked_frame);
    stacked_frame.convertTo(stacked_frame, CV_16UC1, 65535.0 / 255.0);

    std::string prefix = std::string{filename.substr(0, filename.size() - 4)} + "_Stacked";
    json metadata =
        {
            {"orig_frames", frames.size()}, {"stacked_frames", stacked_frames_num}, {"stack_perc", stack_perc}, {"filter", filter}, {"date_prefix", filename.substr(0, filename.find("-Jupiter"))}, {"sharpening_config", sharpener.get_sharpening_config()}};

    if (run_sharpening)
    {
        sharpened_frame = sharpener.runSharpening(stacked_frame);
    }

    saveOutput(metadata, stacked_frame, sharpened_frame, output_path);
}

/**
 * @brief sets variable to determine whether pipeline sharpens image
 *
 * @param run
 */

constexpr void VideoProcessor::setRunSharpening(const bool run)
{
    run_sharpening = run;
}

// void VideoProcessor::saveSharpenedOutput(std::string_view output_path, std::string_view prefix, cv::Mat frame) const
// {
//     if (run_sharpening == true)
//     {
//         auto &config = sharpener.getSharpenConfig();
//         std::string levels_str;
//         for (const auto &[level, amp] : config.level_sharpenings)
//         {
//             std::ostringstream oss;
//             oss << std::fixed << std::setprecision(2) << amp;
//             std::string rounded_amp = oss.str();

//             levels_str += "_" + std::to_string(level) + "X" + rounded_amp;
//         }

//         cv::imwrite(std::string{output_path} + std::string{prefix} + "_" + "WV-" + "Aligned" + ".png", frame);
//     }
// }

// /**
//  * @brief Set the Sharpening Level object
//  *
//  * configure a set of ampplifications one for each spatial scale (decomposition level)
//  * to enhance the contrast of(sharpen)
//  *
//  * @param level
//  * @param amplification
//  */
// void VideoProcessor::setSharpening(const SharpeningMap &sharpen_map)
// {
//     WaveletSharpener sharpener;
//     for (const auto &[level, amp] : sharpen_map)
//     {
//         sharpener.addLevelSharpening(level, amp);
//     }
//     filter_sharpen_config["default"] = sharpener;
// }

/**
 * @brief Set the Sharpening Level object
 *
 * configure a set of amplification, one for each spatial scale (decomposition level)
 * to enhance the contrast of(sharpen) the output image
 *
 * @param level
 * @param amplification
 * @param filters
 */
void VideoProcessor::setSharpening(const SharpeningMap &sharpen_map, const std::vector<std::string> &filters)
{
    if (!filters.size())
    {
        WaveletSharpener sharpener;
        for (const auto &[level, amp] : sharpen_map)
        {
            sharpener.addLevelSharpening(level, amp);
        }
        filter_sharpen_config["default"] = sharpener;
    }

    for (const auto &filter : filters)
    {
        WaveletSharpener sharpener;

        for (const auto &[level, amp] : sharpen_map)
        {
            sharpener.addLevelSharpening(level, amp);
        }
        filter_sharpen_config[filter] = sharpener;
    }
};

void VideoProcessor::setSharpening(SharpenPreset configType, const std::vector<std::string> &filters)
{
    const auto &sharpen_map = sharpen_configs[static_cast<int>(configType)];
    setSharpening(sharpen_map, filters);
}

void VideoProcessor::setStackPercentage(float percentage)
{
    select_config["default"] = percentage;
}

/**
 * @brief configures amount of frames to stack for individual filter
 *
 * @param percentage between 0 and 1
 * @param filter string wavelength
 */

void VideoProcessor::setStackPercentage(float percentage, std::vector<std::string> filters)
{

    for (auto &filter : filters)
    {
        select_config[filter] = percentage;
    }
}

void VideoProcessor::saveOutput(const json metadata, const cv::Mat &frame, const cv::Mat &sharpened_frame, const std::string_view output) const
{
    std::string date_prefix = metadata["date_prefix"].get<std::string>();
    std::string filter = metadata["filter"].get<std::string>();
    std::string prefix = date_prefix + "-Jupiter_" + filter;
    std::string output_path = std::string{output};

    std::filesystem::create_directories(output_path);
    std::string output_stacked = output_path + "/" + prefix + ".png";
    cv::imwrite(output_stacked, frame);

    if (!sharpened_frame.empty())
    {
        std::string output_sharpened = output_path + "/" + prefix + "_WV.png";
        cv::imwrite(output_sharpened, sharpened_frame);
    }
    std::ofstream metadata_file(output_path + "/" + prefix + ".json");
    metadata_file << metadata.dump(4);
};
