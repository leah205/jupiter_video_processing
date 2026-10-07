#include <opencv2/opencv.hpp>
#include "sharpen.h"

extern "C"
{
#include "wavelib.h"
}

// subtract laplacian from image
// sharpened_pizel = 5 * current-left-right-up-down

/**
 * @brief apply kernel to sharpen frame
 *
 * @param image
 * @param result
 */

void sharpen(const cv::Mat &image, cv::Mat &result)
{
    // input must be 8-bit

    cv::Mat kernel(3, 3, CV_32F, cv::Scalar(0));

    kernel.at<float>(1, 1) = 5.0;
    kernel.at<float>(0, 1) = -1.0;
    kernel.at<float>(2, 1) = -1.0;
    kernel.at<float>(1, 0) = -1.0;
    kernel.at<float>(1, 2) = -1.0;

    cv::filter2D(image, result, image.depth(), kernel);
};

void WaveletSharpener::sharpenImage(const cv::Mat &image, cv::Mat &result)
{
    cv::Mat input;
    // double *input_ptr, *output_ptr, *diff_ptr;
    std::vector<double> input_v, diff_v;
    double *output_p, diff_p;
    double *cLL;
    int ir, ic;
    int size = input.rows * input.cols;
    image.convertTo(input, CV_64F);
    // input_v = input.data;
    input_v.assign(input.datastart, input.dataend);

    // input_ptr = reinterpret_cast<double *>(input.data);
    obj = wave_init("wavelet");
    wt = wt2_init(obj, "swt", input.rows, input.cols, 2);
    // setWTConv(wt, "direct");
    double *wavecoeffs = swt2(wt, input_v.data());
    cLL = getWT2Coeffs(wt, wavecoeffs, 2, "A", &ir, &ic);
    dispWT2Coeffs(cLL, ir, ic);
    iswt2(wt, wavecoeffs, output_p);
    std::vector<double> output_v(output_p, output_p + size);

    for (int i = 0; i < size; i++)
    {
        diff_v[i] = output_v[i] - input_v[i];
    };

    result.create(input.rows, input.cols, CV_64F);
    // is this actually what i want here?

    std::memcpy(result.data, (void *)&output_v, size * sizeof(double));

    wt2_summary(wt);

    wave_free(obj);
    wt2_free(wt);
}
