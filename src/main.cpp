#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
#include <cmath>
#include <string>
#include <filesystem>

#include "videoProcessor.h"
#include "optimisation.h"
#include "helpers.h"
#include "test_helpers.h"
#include "helpers.h"
#include "align.h"
#include "stack.h"
#include "quality.h"

// g++ main.cpp -pg -O0  -g -o my_program $(pkg-config --cflags --libs opencv4)

static bool ends_with(std::string_view str, std::string_view suffix)
{
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

int main()
{
    double duration = static_cast<double>(cv::getTickCount());
    VideoProcessor processor;

    namespace fs = std::filesystem;
    fs::path input_dir = "data";

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

    std::string output_path = "output/";

    int sharpen_level = 4;
    double sharpen_amp = 2.0;
    processor.addSharpeningLevel(sharpen_level, sharpen_amp);
    sharpen_level = 3;
    sharpen_amp = 4.0;
    processor.addSharpeningLevel(sharpen_level, sharpen_amp);
    // sharpen_level = 2;
    // sharpen_amp = 2.0;
    // processor.addSharpeningLevel(sharpen_level, sharpen_amp);

    for (size_t i = 0; i < filepaths.size(); i++)
    {
        if (i < 2)
        {
            continue;
        }

        try
        {
            processor.processVideo(filepaths[i], output_path);
        }
        catch (const std::exception &e)
        {
            std::cout << "failed to process " << filepaths[i] << ": " << e.what() << std::endl;
        }
    }

    // // processor.getMaskAreas();

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
