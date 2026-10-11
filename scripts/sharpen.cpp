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
    int size = image.rows * image.cols;
    // double *input_ptr, *output_ptr, *diff_ptr;
    std::vector<double> input_v(size * sizeof(double)), diff_v(size * sizeof(double)), output_v(size * sizeof(double));
    double *cLH, *cHL, *cHH;
    int ir, ic;
    image.convertTo(input, CV_64F);

    input_v.assign(input.datastart, input.dataend);
    obj = wave_init("db4");
    wt = wt2_init(obj, "swt", input.rows, input.cols, 3);
    // setWTConv(wt, "direct");
    double *wavecoeffs = swt2(wt, input.ptr<double>(0));
    // double *wavecoeffs = swt2(wt, input_v.data());

    cLH = getWT2Coeffs(wt, wavecoeffs, 2, "H", &ir, &ic);
    cHL = getWT2Coeffs(wt, wavecoeffs, 2, "V", &ir, &ic);
    cHH = getWT2Coeffs(wt, wavecoeffs, 2, "D", &ir, &ic);
    // causing white specks
    for (int i = 0; i < ir; i++)
    {
        for (int j = 0; j < ic; j++)
        {
            *(cLH++) *= 20;
            *(cHL++) *= 20;
            *(cHH++) *= 20;
        }
    }

    // dispWT2Coeffs(cLL, ir, ic);
    iswt2(wt, wavecoeffs, output_v.data());
    result = cv::Mat(input.rows, input.cols, CV_64F, &output_v.front());
    result.convertTo(result, CV_8UC1);

    wt2_summary(wt);
    free(wavecoeffs);
    wave_free(obj);
    wt2_free(wt);
    // free wavelet coefficients
}
