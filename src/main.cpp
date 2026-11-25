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
#include <optional>
#include <fstream>
#include <nlohmann/json.hpp>
using nlohmann::json;

std::optional<SharpenPreset> get_sharpen_preset(std::string_view string_preset)
{
    if (string_preset == "")
    {
        return std::nullopt;
    }
    if (string_preset == "default")
    {
        return SharpenPreset::DEFAULT;
    }
    else if (string_preset == "coarse")
    {
        return SharpenPreset::COARSE;
    }
    else
    {
        PLOG_ERROR << std::string{string_preset} << " is not a valid sharpening configuration. Using default instead";
        return SharpenPreset::DEFAULT;
    }
}

void configure_processor(json config_data, VideoProcessor &processor)
{
    std::vector<std::string> filters = config_data["filters"].get<std::vector<std::string>>();
    std::cout << "config data " << config_data << std::endl;
    bool run_sharpening = config_data.contains("run_sharpening") ? config_data["run_sharpening"].get<bool>() : false;
    processor.setFilters(filters);
    processor.setRunSharpening(run_sharpening);
    for (auto &[filter, filter_config] : config_data["video_config"].items())
    {
        if (filter != "default" && (std::find(filters.begin(), filters.end(), filter) == filters.end()))
        {
            continue;
        }

        float stack_percentage = (filter_config.contains("stack_percentage")) ? filter_config["stack_percentage"].get<float>() : 0;
        std::optional<SharpenPreset> preset = (filter_config.contains("sharpening")) ? get_sharpen_preset(filter_config["sharpening"].get<std::string>()) : std::nullopt;
        if (filter == "default")
        {
            if (preset.has_value() && run_sharpening)
            {
                processor.setSharpening(preset.value());
            }
            if (stack_percentage)
            {
                processor.setStackPercentage(stack_percentage);
            }
        }
        else
        {
            if (preset.has_value() && run_sharpening)
            {
                processor.setSharpening(preset.value(), std::vector<std::string>{filter});
            }
            if (stack_percentage)
            {
                processor.setStackPercentage(stack_percentage, std::vector<std::string>{filter});
            }
        }
    };
}

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
    std::vector<std::string> filters{config_data["filters"].get<std::vector<std::string>>()};
    fs::create_directories(output_dir);
    fs::create_directories(fits_dir);

    if (!std::filesystem::exists(log_file))
    {
        std::ofstream file(log_file);
        file.close();
    }

    VideoProcessor processor;

    plog::init(plog::info, log_file.c_str());
    PLOG_INFO << "Running video processing on : " << input_dir;

    // PLOG_INFO << "Processing all .avi files from " << input_dir;
    try
    {
        configure_processor(config_data, processor);
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR << "sharpening configuration: " << e.what();
    }
    // array?

    // processor.setStackPercentage(0.25);
    // processor.setStackPercentage(0.5, {"889H4"});
    // // processor.setSharpening({{2, 6.0}, {3, 5.0}, {4, 2.0}});
    // processor.setSharpening(SharpenPreset::DEFAULT);
    // processor.setSharpening(SharpenPreset::COARSE, {"889CH4"});
    // processor.setSharpening({{2, 15.0}}, {"647"});

    int failed_count = 0;
    // std::vector<std::string> filepaths;
    std::vector<std::string> filepaths = get_avi_files(fs::path(input_dir));

    for (size_t i = 0; i < filepaths.size(); i++)
    {

        std::cout << "processing " << filepaths[i] << std::endl;

        try
        {
            std::string filter = get_filter(filepaths[i]);
            if ((std::find(filters.begin(), filters.end(), filter) != filters.end()))
            {
                processor.processVideo(filepaths[i], output_dir);
            }
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
