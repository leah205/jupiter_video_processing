#include <opencv2/opencv.hpp>
#include <numeric>
#include <iostream>
#include <optional>
#include <string>
#include <sstream>
#include <iomanip>

#include <cmath>

#include "stack.h"
#include "align.h"
#include "helpers.h"
#include "optimisation.h"
#include "videoProcessor.h"
#include "frameInfo.h"

#include "test_helpers.h"
#include "sharpen.h"

/**
 * @brief adds frame candidate for lucky imaging
 *
 * computes quality score and adds frameInfo object to frame list
 *
 * @param frame
 */
frameInfo VideoProcessor::processFrame(cv::Mat frame) const
{
    frameInfo newFrame;
    extract_channel(frame);
    // subtract_background_signal(frame);
    cv::Mat planet_mask = get_planet_mask(frame);
    smooth_mask(planet_mask);
    newFrame.frame = frame;
    newFrame.planet_mask = planet_mask;
    return newFrame;
    // frames.push_back(newFrame);
};

/**
 * @brief Sets number of frames to stack
 *
 * @param frame_num
 */

/**
 * @brief gets sharpest frames
 *
 */
std::vector<frameInfo> VideoProcessor::selectFrames(std::vector<frameInfo> frames)
{
    Limiter limiter(quality_scorer);
    const int stacked_frames_num = ceil(((double)(frames.size())) / 4);
    limiter.setLimitFrameNum(stacked_frames_num);
    std::vector<frameInfo> selected_frames = limiter.limitFrames(frames);
    limiter.compare_methods(frames);
    return selected_frames;
}

// /**
//  * @brief
//  *
//  * @return std::vector<frameInfo>
//  */
// std::vector<frameInfo> VideoProcessor::getSelected()
// {
//     return selected_frames;
// };

/**
 * @brief aligns frames to reference
 *
 * updates aligned_frames list to contain matrices of sharpest frames
 * with centroid aligned to reference frame
 *
 */
std::vector<frameInfo> VideoProcessor::alignFrames(std::vector<frameInfo> frames)
{
    Aligner aligner{};
    std::cout << "aligning frames..." << std::endl;
    std::vector<frameInfo> aligned_frames{};
    frameInfo ref_frame = frames[0];
    aligner.setRefFrame(ref_frame);
    for (int i = 1; i < frames.size(); i++)
    {
        aligned_frames.push_back(aligner.alignFrame(frames[i]));
    }
    return aligned_frames;
};

/**
 * @brief stacks selected aligned frames
 *
 * averages pixels among aligned selected frames and stores them in stacked matrix
 */

cv::Mat VideoProcessor::stackFrames(std::vector<frameInfo> frames) const
{
    cv::Mat output;
    std::cout << "stacking " << frames.size() << " frames..." << std::endl;
    output = stack_frames(frames);
    subtract_background_signal(output);
    return output;
}

/**
 * @brief generates and stores sharpened output in class
 *
 */
cv::Mat VideoProcessor::sharpenFrame(const cv::Mat &frame) const
{
    cv::Mat sharpened_frame;
    if (frame.empty())
    {
        throw std::runtime_error("no output matrix exists to sharpen");
    }
    sharpener.runSharpening(frame, sharpened_frame);
    return sharpened_frame;
}

std::vector<frameInfo> VideoProcessor::readFrames(const std::string input_path) const
{
    bool ret;
    cv::Mat frame;
    std::vector<frameInfo> frames;
    cv::VideoCapture cap(input_path);
    int frame_num = cap.get(cv::CAP_PROP_FRAME_COUNT);
    int fps = cap.get(cv::CAP_PROP_FPS);

    std::cout << "number of frames: " << frame_num << std::endl;
    std::cout << "frames per second: " << fps << std::endl;

    if (!cap.isOpened())
    {
        std::cout << "Error: video object was not opened successfully" << std::endl;
        throw -1;
    }
    else
    {
        std::cout << "Video opened successfully" << std::endl;
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

void VideoProcessor::processVideo(const std::string_view input_path, const std::string_view output_path)
{ // make these return values
    std::vector<frameInfo> frames = readFrames(std::string{input_path});
    std::vector<frameInfo> selected_frames = selectFrames(frames);
    std::vector<frameInfo> aligned_frames = alignFrames(selected_frames);
    cv::Mat stacked_frame = stackFrames(aligned_frames);
    std::size_t pos = input_path.find_last_of('/');
    std::string_view filename = input_path.substr(pos + 1);
    std::string_view prefix = filename.substr(0, filename.size() - 4);
    save_output(output_path, prefix, stacked_frame);

    if (run_sharpening)
    {
        cv::Mat sharpened_frame = sharpenFrame(stacked_frame);
        saveSharpenedOutput(output_path, prefix, sharpened_frame);
    }
}

void VideoProcessor::setRunSharpening(const bool run)
{
    run_sharpening = run;
}

void VideoProcessor::saveSharpenedOutput(std::string_view output_path, std::string_view prefix, cv::Mat frame) const
{
    if (run_sharpening == true)
    {
        auto &config = sharpener.getSharpenConfig();
        std::string levels_str;
        for (const auto &[level, amp] : config.level_sharpenings)
        {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << amp;
            std::string rounded_amp = oss.str();

            levels_str += "_" + std::to_string(level) + "X" + rounded_amp;
        }

        cv::imwrite(std::string{output_path} + std::string{prefix} + "WV" + levels_str + "_" + config.method + ".png", frame);
    }
}

/**
 * @brief Set the Sharpening Level object
 *
 * configure a spatial scale (decomposition level)
 * to enhance the contrast of(sharpen)
 *
 * @param level
 * @param amplification
 */
void VideoProcessor::setSharpeningLevel(int level, double amplification)
{
    sharpener.setLevelSharpening(level, amplification);
}

// void VideoProcessor::alignSelectedCentroidEcc()
// {
//     frameInfo ref_frame = frames[selected_frames[0]];
//     cv::Point ref_cm = get_center_of_mass(ref_frame.planet_mask);
//     aligned_frames.push_back(ref_frame.frame);

//     double mean_corr = 0;
//     double min_corr = 0;
//     double mean_shiftmag = 0;
//     double max_shiftmag = 0;

//     int flag = 1;

//     for (int i = 1; i < selected_frames.size(); i++)
//     {
//         frameInfo frame = frames[selected_frames[i]];
//         frame.cm = get_center_of_mass(frame.planet_mask);
//         cv::Mat aligned_mat = get_aligned_by_centroid(aligned_mat, frame.cm, ref_cm);
//         // cv::imshow("aligned", aligned_mat);
//         // cv::waitKey(0);

//         cv::Mat new_aligned;
//         double shift_mag = transform_ecc(ref_frame.frame, aligned_mat, new_aligned);
//         // cv::imshow("new aligned", new_aligned);
//         // cv::waitKey(0);
//         double corr = compute_ecc(frame.frame, new_aligned);

//         aligned_frames.push_back(new_aligned);

//         mean_corr += corr;
//         min_corr = std::min(corr, min_corr);

//         mean_shiftmag += shift_mag;
//         max_shiftmag = std::max(shift_mag, max_shiftmag);
//         // std::cout << shift_mag << std::endl;
//         if (shift_mag > 1.1 && flag)
//         {
//             flag = 0;
//             min_corr = corr;

//             Histogram1D h;
//             cv::imwrite("histogram.png", h.getHistogramImage(frame.frame));

//             // cv::imwrite("centroid_gradient_diff.png", generate_diff(ref_frame, aligned_mat));
//             // cv::imwrite("ecc_gradient_diff.png", generate_diff(ref_frame, new_aligned));

//             // cv::imshow("centroid to ref", generate_diff(ref_frame, aligned_mat));
//             // cv::waitKey(0);

//             // cv::imshow("ecc to ref", generate_diff(ref_frame, new_aligned));
//             // cv::waitKey(0);
//         }
//     }
//     mean_corr = mean_corr / (selected_frames.size() - 1);
//     mean_shiftmag = mean_shiftmag / (selected_frames.size() - 1);

//     std::cout << "min coor: " << min_corr << std::endl;
//     std::cout << "mean coor: " << mean_corr << std::endl;

//     std::cout << "mean shift mag: " << mean_shiftmag << std::endl;
//     std::cout << "max shift mag: " << max_shiftmag << std::endl;
// }