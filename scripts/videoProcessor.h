#include <opencv2/opencv.hpp>
#include "align.h"
#include "limit.h"
#include "frameInfo.h"
#include "quality.h"
#include "sharpen.h"

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
    WaveletSharpener sharpener;
    // std::vector<size_t> quality_sorted_indices;

public:
    VideoProcessor(const FrameQuality &q_s, int wavelet_levels = 5) : limiter(q_s),
                                                                      aligner(),
                                                                      sharpener(wavelet_levels)
    {
        stacked_frames_num = 600;
    }

    void addFrame(cv::Mat &frame);

    void setFrameStackNum(int frame_num);

    // void alignSelectedFramesByCentroid();

    // void alignSelectedCentroidEcc();

    // void getMaskAreas();

    // void compareStacks();

    std::vector<frameInfo> getSelected();

    cv::Mat getSharpenedOutput();

    void generateSharpenedOutput();

    void setSharpeningLevel(int level, double amplification);

    void selectFrames();

    void stackAlignedFrames();

    void alignFrames();

    cv::Mat getOutput();
};

#endif