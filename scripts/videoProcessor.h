#include <opencv2/opencv.hpp>

#ifndef VIDEO_H
#define VIDEO_H

typedef struct
{
    cv::Mat frame;
    double quality_score;
    cv::Point cm;
} frameInfo;

class VideoProcessor
{
private:
    std::vector<frameInfo> frames;
    std::vector<cv::Mat> aligned_frames;
    int stacked_frames_num;
    cv::Mat output;
    std::vector<size_t> selected_frames;
    std::vector<size_t> quality_sorted_indices;

public:
    VideoProcessor()
    {
        stacked_frames_num = 600;
    }

    void addFrame(cv::Mat &frame);

    void setFrameStackNum(int frame_num);

    void selectFramesByGradient();

    void alignSelectedFramesByCentroid();

    void alignSelectedCentroidEcc();

    void stackAlignedFrames();

    cv::Mat getOutput();
};

#endif