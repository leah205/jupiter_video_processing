#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

#include "videoProcessor.h"
#include "optimisation.h"
#include "helpers.h"
#include "test_helpers.h"
#include "helpers.h"
#include "align.h"
#include "stack.h"
#include "quality.h"

// launch registax with gio launch R..

int main()
{

    double duration = static_cast<double>(cv::getTickCount());
    VideoProcessor processor;
    plog::init(plog::info, "LogFile.txt");

    fs::path input_dir = "data/20260318UT";
    PLOG_INFO << "Processing all .avi files from " << input_dir.string();

    std::string output_path = "sharpen_exp/oi/";
    std::string filter = "OI";
    int sharpen_level = 4;
    double sharpen_amp = 2.0;
    processor.addSharpeningLevel(sharpen_level, sharpen_amp);
    sharpen_level = 3;
    sharpen_amp = 5.0;
    processor.addSharpeningLevel(sharpen_level, sharpen_amp);
    // sharpen_level = 2;
    // sharpen_amp = 2.0;
    // processor.addSharpeningLevel(sharpen_level, sharpen_amp);
    int failed_count = 0;
    std::vector<std::string> filepaths = get_avi_files(input_dir);

    for (size_t i = 0; i < filepaths.size(); i++)
    {

        std::cout << "processing " << filepaths[i] << std::endl;

        try
        {
            processor.processVideo(filepaths[i], output_path);
            PLOG_INFO << "processed " << filepaths[i];
        }
        catch (const std::exception &e)
        {
            std::cout << "failed to process " << filepaths[i] << ": " << e.what() << std::endl;
            PLOG_ERROR << e.what() << " " << filepaths[i];
            failed_count++;
        }
    }

    duration = static_cast<double>(cv::getTickCount()) - duration;
    duration /= cv::getTickFrequency();
    std::cout << "duration of program: " << duration << " s" << std::endl;
    PLOG_INFO << "files processed: " << filepaths.size() - failed_count;
    PLOG_INFO << "files failed: " << failed_count;
    PLOG_INFO << "duration of run: " << duration << "s";
}
