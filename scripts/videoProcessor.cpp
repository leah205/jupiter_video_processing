#include <opencv2/opencv.hpp>
#include <numeric>
#include <iostream>

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
void VideoProcessor::addFrame(cv::Mat &frame)
{
    frameInfo newFrame;
    extract_channel(frame);
    // subtract_background_signal(frame);
    cv::Mat planet_mask = get_planet_mask(frame);
    smooth_mask(planet_mask);
    newFrame.frame = frame;
    newFrame.planet_mask = planet_mask;
    frames.push_back(newFrame);
};

/**
 * @brief Sets number of frames to stack
 *
 * @param frame_num
 */

void VideoProcessor::setFrameStackNum(int frame_num)
{
    stacked_frames_num = frame_num;
};

void VideoProcessor::setQualityMetricToGradient()
{
    quality_metric = GRADIENT;
}

void VideoProcessor::setQualityMetricToLaplacian()
{
    quality_metric = LAPLACIAN;
}

void VideoProcessor::selectFrames()
{
    limiter.setLimitFrameNum(stacked_frames_num);
    selected_frames = limiter.limitFrames(frames);
    std::cout << selected_frames.size() << std::endl;
}

std::vector<frameInfo> VideoProcessor::getSelected()
{
    return selected_frames;
};

/**
 * @brief aligns frames to reference
 *
 * updates aligned_frames list to contain matrices of sharpest frames
 * with centroid aligned to reference frame
 *
 */
void VideoProcessor::alignFrames()
{
    std::cout << "aligning frames..." << std::endl;
    frameInfo ref_frame = selected_frames[0];
    aligner.setRefFrame(ref_frame);
    // should first frame be in here?
    aligned_frames = aligner.alignFramesToRef(selected_frames);
};

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

/**
 * @brief stacks selected aligned frames
 *
 * averages pixels among aligned selected frames and stores them in stacked matrix
 */

void VideoProcessor::stackAlignedFrames()
{
    std::cout << "stacking " << aligned_frames.size() << " frames..." << std::endl;

    output = stack_frames(aligned_frames);
    std::cout << cv::mean(output, cv::Mat())
              << std::endl;
    // subtract_background_signal(output);
    std::cout << cv::mean(output, cv::Mat())
              << std::endl;
}

void VideoProcessor::generateSharpenedOutput()
{
    if (output.empty())
    {
        throw std::runtime_error("no output matrix exists to sharpen");
    }
    sharpen(output, sharpened_output);
}

cv::Mat VideoProcessor::getOutput()
{
    return output.clone();
}

cv::Mat VideoProcessor::getSharpenedOutput()
{
    return sharpened_output.clone();
}

// void VideoProcessor::getMaskAreas()
// {
//     int min, max, mean;
//     int last_area;
//     cv::Point last_centroid;
//     for (int i = 0; i < frames.size(); i++)
//     {
//         int area = cv::countNonZero(frames[i].planet_mask);
//         cv::Point centroid = get_center_of_mass(frames[i].frame);
//         if (i == 0)
//         {
//             min = area;
//             max = area;
//             last_area = area;
//         }
//         // min = std::min(min, area);
//         // max = std::max(max, area);
//         // std::cout << "area: " << area << std::endl;
//         // std::cout << "cx: " << centroid.x << std::endl;
//         // std::cout << "cy: " << centroid.y << std::endl;

//         // if (last_area - area > 200)
//         // {
//         //     printf("")
//         // }

//         mean += area;
//     }
//     mean = mean / frames.size();
//     std::cout << "area min: " << min << std::endl;
//     std::cout << "area max: " << max << std::endl;

//     std::cout << "area mean: " << mean << std::endl;
// }

void saveOutput(std::string output_dir)
{
}