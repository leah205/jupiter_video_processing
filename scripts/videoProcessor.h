#include <opencv2/opencv.hpp>
#include "align.h"
#include "limit.h"
#include "frameInfo.h"

#ifndef VIDEO_H
#define VIDEO_H

class VideoProcessor
{
private:
    Aligner aligner;
    Limiter limiter;
    std::vector<frameInfo> frames;
    std::vector<frameInfo> aligned_frames;
    int stacked_frames_num;
    cv::Mat output;
    cv::Mat sharpened_output;
    std::vector<frameInfo> selected_frames;
    // std::vector<size_t> quality_sorted_indices;
    enum Quality_metric
    {
        GRADIENT,
        LAPLACIAN
    };

    enum Quality_metric quality_metric;

public:
    VideoProcessor()
    {
        stacked_frames_num = 600;
        quality_metric = GRADIENT;
        // Limiter limiter = new Limiter();
        // Aligner aligner = new Aligner();
    }

    void addFrame(cv::Mat &frame);

    void processFrames(cv::Mat frame);

    void setFrameStackNum(int frame_num);

    // void alignSelectedFramesByCentroid();

    // void alignSelectedCentroidEcc();

    // void getMaskAreas();

    // void compareStacks();

    std::vector<frameInfo> getSelected();

    cv::Mat getSharpenedOutput();

    void generateSharpenedOutput();

    void assessFramesQuality();

    void selectFrames();

    void setQualityMetricToGradient();

    void setQualityMetricToLaplacian();

    void stackAlignedFrames();

    void alignFrames();

    // void alignSelectedFramesByCentroidCircle();

    cv::Mat getOutput();
};

#endif