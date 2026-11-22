#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

#include "videoProcessor.h"
#include "helpers.h"
#include "align.h"
#include "stack.h"
#include "quality.h"

#include <fstream>
#include <nlohmann/json.hpp>
using nlohmann::json;

int main()
{
    double duration = static_cast<double>(cv::getTickCount());
    std::ifstream config_f("config/config.json");
    json config_data = json::parse(config_f);
    std::cout << config_data.dump() << std::endl;
    std::string date_string = config_data["date_string"].get<std::string>();
    std::string output_dir = config_data["png_output_dir"].get<std::string>() + "/" + date_string;
    std::string input_dir = config_data["video_input_dir"].get<std::string>() + "/" + date_string;
    std::string fits_dir = config_data["fits_output_dir"].get<std::string>() + "/" + date_string;
    std::string log_file = fits_dir + "/LogFile.txt";
    fs::create_directories(output_dir);
    fs::create_directories(fits_dir);

    if (!std::filesystem::exists(log_file))
    {
        std::ofstream file(log_file);
        file.close();
    }

    // auto output_dir = (config_data["png_output_dir"]).get<fs::path>();
    // auto input_dir = config_data["video_input_dir"].get<fs::path>();

    VideoProcessor processor;

    plog::init(plog::info, log_file.c_str());
    PLOG_INFO << "Running video processing on : " << input_dir;

    // PLOG_INFO << "Processing all .avi files from " << input_dir;

    processor.setStackPercentage(0.25);
    // processor.setStackPercentage(0.5, {"647"});
    // processor.setSharpening({{2, 6.0}, {3, 5.0}, {4, 2.0}});
    processor.setSharpening(SharpenPreset::DEFAULT);
    processor.setSharpening(SharpenPreset::COARSE, {"889CH4"});
    // processor.setSharpening({{2, 15.0}}, {"647"});

    int failed_count = 0;
    // std::vector<std::string> filepaths;
    std::vector<std::string> filepaths = get_avi_files(fs::path(input_dir));

    for (size_t i = 0; i < filepaths.size(); i++)
    {

        std::cout << "processing " << filepaths[i] << std::endl;

        try
        {
            processor.processVideo(filepaths[i], output_dir);
            // PLOG_INFO << "processed " << filepaths[i];
        }
        catch (const std::exception &e)
        {
            std::cout << "failed to process " << filepaths[i] << ": " << e.what() << std::endl;
            PLOG_ERROR << filepaths[i] << ": " << e.what() << " " << filepaths[i];
            failed_count++;
        }
    }

    duration = static_cast<double>(cv::getTickCount()) - duration;
    duration /= cv::getTickFrequency();
    std::cout << "duration of video processing: " << duration << " s" << std::endl;
    PLOG_INFO << "files processed: " << filepaths.size() - failed_count;
    PLOG_INFO << "files failed: " << failed_count;
    PLOG_INFO << "wrote video processing output to: " << output_dir;

    PLOG_INFO << "duration of video processing run: " << duration << "s";
}
