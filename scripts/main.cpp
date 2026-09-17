#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
#include <cmath>
#include <string>

#include "stack.h"
#include "align.h"
#include "helpers.h"
#include "optimisation.h"

// g++ main.cpp -pg -O0  -g -o my_program $(pkg-config --cflags --libs opencv4)

int main()
{
    double duration = static_cast<double>(cv::getTickCount());
    std::string filename = "2026-03-18-0236_9-Jupiter_656HIA.avi";
    std::string output_dir = "../data/";
    std::string path = output_dir + filename;

    cv::VideoCapture cap(path);
    // cv::VideoCapture cap("../data/2026-03-18-0241_0-Jupiter_620CH4.avi");
    // cv::VideoCapture cap("../data/2026-03-18-0239_0-Jupiter_632OI.avi");
    int frame_num = cap.get(cv::CAP_PROP_FRAME_COUNT);
    int fps = cap.get(cv::CAP_PROP_FPS);

    std::cout << "number of frames: " << frame_num << std::endl;
    std::cout << "frames per second: " << fps << std::endl;

    if (!cap.isOpened())
    {
        std::cout << "Error: video object was not opened successfully" << std::endl;
        return -1;
    }
    else
    {
        std::cout << "Video opened successfully" << std::endl;
    }

    std::vector<cv::Mat> frames;
    std::vector<double> mags_vec;

    std::cout << "reading frames..." << std::endl;

    cv::Mat ref_frame;
    bool ret = cap.read(ref_frame);
    get_array_info(ref_frame);

    extract_channel(ref_frame);
    cv::Mat ref_mask = get_planet_mask(ref_frame);
    // gets mask without limb for quality selection
    cv::Mat inner_mask = get_inner_planet_mask(ref_mask);
    cv::Rect rect = get_cropped_rect(ref_mask);

    std::cout << get_avg_gradient_mag(ref_frame, inner_mask, rect) << std::endl;

    // rect used crop aligned frames to reduce quality selection computation time

    frames.push_back(ref_frame);

    cv::Mat frame;
    while (true)
    {

        ret = cap.read(frame);

        if (!ret)
        {
            break;
        }

        extract_channel(frame);
        frames.push_back(frame);
    }

    std::cout << "selecting frames..." << std::endl;

    for (int i = 0; i < frame_num; i++)
    {
        cv::Mat inner_mask = get_inner_planet_mask(frames[i]);
        cv::Rect rect = get_cropped_rect(frames[i]);
        mags_vec.push_back(get_avg_gradient_mag(frames[i], inner_mask, rect));
    }

    size_t select_amount = ceil((double)(frame_num) / 4);
    std::vector<size_t> selected_indices = get_selected_indices(mags_vec, select_amount);

    std::cout << "aligning frames..." << std::endl;

    std::vector<cv::Mat> aligned_frames = align_selected_to_ref(frames, ref_frame, selected_indices);

    std::cout << "stacking " << select_amount << " frames... " << std::endl;
    cv::Mat stacked = stack_frames(aligned_frames);
    // cv::imshow("stacked", stacked);
    // cv::waitKey(0);
    std::string prefix = filename.substr(0, filename.find("."));
    save_image(prefix, stacked);
    cap.release();

    duration = static_cast<double>(cv::getTickCount()) - duration;
    duration /= cv::getTickFrequency();
    std::cout << "duration of program: " << duration << " s" << std::endl;

    cv::Mat registax_frame;
    cv::VideoCapture reg_cap("../registax/2026-03-18-0236_9-Jupiter_656HIA_stacked.bmp");
    ret = reg_cap.read(registax_frame);
    extract_channel(registax_frame);
    std::cout << compute_diff(stacked, registax_frame) << std::endl;

    cv::Point cm_reg = get_center_of_mass(registax_frame);
    cv::Point cm_ref = get_center_of_mass(ref_frame);
    cv::Mat registax_aligned = align_frame(registax_frame, cm_reg, cm_ref);

    get_array_info(registax_aligned);
    get_array_info(stacked);

    std::cout
        << "corr: " << compute_diff(stacked, registax_aligned) << std::endl;
    inner_mask = get_inner_planet_mask(stacked);
    rect = get_cropped_rect(stacked);
    std::cout << "gradient before blur: " << get_avg_gradient_mag(stacked, inner_mask, rect) << std::endl;

    cv::Mat blurred;
    cv::GaussianBlur(stacked, blurred, cv::Size(3, 3), 0);
    std::cout << "gradient after blur: " << get_avg_gradient_mag(blurred, inner_mask, rect) << std::endl;

    inner_mask = get_inner_planet_mask(registax_aligned);
    rect = get_cropped_rect(registax_aligned);
    std::cout << "registax gradient: " << get_avg_gradient_mag(registax_aligned, inner_mask, rect) << std::endl;

    return 0;
}