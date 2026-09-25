#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
#include <cmath>
#include <string>

#include "test_helpers.h"
#include "helpers.h"
#include "align.h"

int main()
{
    VideoProcessor processor;
    bool ret;
    cv::Mat frame;
    cv::Mat shifted;

    std::string filename = "2026-03-18-0236_9-Jupiter_656HIA.avi";
    std::string output_dir = "../data/";
    std::string path = output_dir + filename;

    cv::VideoCapture cap(path);

    ret = cap.read(frame);
    extract_channel(frame);
    get_array_info(frame);
    synthetic_shift(frame, shifted);
    cv::Point ref_cm = get_center_of_mass(frame);
    cv::Point cm = get_center_of_mass(shifted);
    get_aligned_by_centroid(frame, cm, ref_cm);
    // generate_diff(ref_frame, frame);
}