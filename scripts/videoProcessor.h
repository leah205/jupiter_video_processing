#include <opencv2/opencv.hpp>

#ifndef VIDEO_H
#define VIDEO_H

typedef struct
{
    cv::Mat frame;
    double quality_score;
    cv::Point cm;
    cv::Mat planet_mask;
} frameInfo;

class VideoProcessor
{
private:
    std::vector<frameInfo> frames;
    std::vector<cv::Mat> aligned_frames;
    int stacked_frames_num;
    cv::Mat output;
    cv::Mat sharpened_output;
    std::vector<size_t> selected_frames;
    std::vector<size_t> quality_sorted_indices;
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
    }

    void addFrame(cv::Mat &frame);

    void setFrameStackNum(int frame_num);

    void alignSelectedFramesByCentroid();

    void alignSelectedCentroidEcc();

    void getMaskAreas();

    cv::Mat getSharpenedOutput();

    // void alignEcc();

    void generateSharpenedOutput();

    void assessFramesQuality();

    void selectFrames();

    void setQualityMetricToGradient();

    void setQualityMetricToLaplacian();

    void stackAlignedFrames();

    void alignSelectedFramesByCentroidCircle();

    cv::Mat getOutput();

    void saveOutput(std::string output_dir);
};

#endif