#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
#include <cmath>
#include <string>

#include "videoProcessor.h"
#include "optimisation.h"
#include "helpers.h"
#include "test_helpers.h"
#include "helpers.h"
#include "align.h"
#include "stack.h"
#include "quality.h"

// g++ main.cpp -pg -O0  -g -o my_program $(pkg-config --cflags --libs opencv4)

int main()
{
    double duration = static_cast<double>(cv::getTickCount());
    // FrameQuality quality_scorer = FrameQuality(QualityMetric::GRADIENT);
    VideoProcessor processor;

    std::string input_path = "data/2026-03-18-0236_9-Jupiter_656HIA.avi";
    // std::string filename = "2026-03-18-0241_0-Jupiter_620CH4.avi";

    // std::string filename = "2026-03-18-0239_0-Jupiter_632OI.avi";

    // std::string filename = "2026-03-18-0704_1-Jupiter_685NIR.avi";

    // std::string filename = "2026-03-18-0706_3-Jupiter_450BLU.avi";

    // std::string input_dir = "../data/";
    // std::string path = input_dir + filename;

    std::string output_path = "output/";

    int sharpen_level = 2;
    double sharpen_amp = 1.3;
    processor.setSharpeningLevel(sharpen_level, sharpen_amp);
    processor.processVideo(input_path, output_path);

    // cv::imwrite(output_dir + output_file, output);
    // // processor.getMaskAreas();
    // // cv::imshow("final", processor.getOutput());
    // // cv::waitKey(0);

    // cv::VideoCapture r_cap("../registax/2026-03-18-0236_9-Jupiter_656HIAStack600.png");
    // // cv::VideoCapture r_cap("../registax/2026-03-18-0239_0-Jupiter_632OIStack600.png");

    // cv::Mat registax;
    // ret = r_cap.read(registax);
    // extract_channel(registax);
    // cv::Point r_cm = get_center_of_mass(registax);
    // cv::Point ref_cm = get_center_of_mass(output);

    // cv::Mat aligned_reg = get_aligned_by_centroid(registax, r_cm, ref_cm);

    // std::cout
    //     << "correlation to registax output: " << compute_ecc(aligned_reg, output) << std::endl;
    // cv::Mat inner_mask = get_inner_planet_mask(get_planet_mask(registax));
    // // cv::Rect rect = get_cropped_rect(registax);

    // // std::cout << "registax output magnitude: " << get_avg_gradient_mag(registax, inner_mask, rect) << std::endl;

    // // inner_mask = get_inner_planet_mask(get_planet_mask(output));
    // // rect = get_cropped_rect(output);

    // // std::cout << "output magnitude: " << get_avg_gradient_mag(output, inner_mask, rect) << std::endl;

    duration = static_cast<double>(cv::getTickCount()) - duration;
    duration /= cv::getTickFrequency();
    std::cout << "duration of program: " << duration << " s" << std::endl;

    // // cv::imwrite("ouput.png", output);
}
