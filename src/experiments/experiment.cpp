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

void run_experiments(std::vector<SharpeningMap> experiments)
{
    std::vector<std::string> filters = {"OI", "CH4", "NIR", "BLU", "HIA", "GRN", "NH3"};
    // std::vector<std::string> filters = {"OI", "CH4"};

    fs::path input_dir = "data/20260318UT";
    std::vector<std::string> filepaths = get_avi_files(input_dir);
    std::string filename;
    for (const auto &filter : filters)
    {
        for (size_t i = 0; i < filepaths.size(); i++)
        {
            if (filepaths[i].find(filter) != std::string::npos)
            {
                filename = filepaths[i];
                break;
            }
        }
        std::string output_path = "src/experiments/sharpen_exp/" + filter + "/";

        for (const auto &experiment : experiments)
        {
            VideoProcessor processor;

            for (const auto &[level, amp] : experiment)
            {
                processor.addSharpeningLevel(level, amp);
            }
            std::cout << output_path << std::endl;
            processor.processVideo(filename, output_path);
            std::cout << filename << std::endl;
        }
    }
}

int main()
{
    // std::vector<SharpeningMap> experiments = {
    //     {{2, 2.0}, {3, 4.0}, {4, 2.0}},
    //     {{2, 2.0}, {3, 4.0}, {4, 3.0}},
    //     {{3, 4.0}, {4, 2.0}},
    //     {{3, 4.0}, {4, 3.0}},
    //     {{3, 5.0}, {4, 2.0}},
    //     {{3, 5.0}, {4, 3.0}},
    //     {{3, 5.0}, {4, 4.0}},
    //     {{3, 6.0}, {4, 3.0}}

    // };
    std::vector<SharpeningMap> experiments = {

        {{2, 6.0}, {3, 5.0}, {4, 2.0}},

    };
    run_experiments(experiments);
    return 0;
}