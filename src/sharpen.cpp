#include <opencv2/opencv.hpp>
#include "sharpen.h"

extern "C"
{
#include "wavelib.h"
}

static void showDetailMap(const cv::Mat &input, const cv::Mat &output);

/**
 * @brief performs wavelet sharpening
 *
 * Performs swt wavelet decomposition and uses sharpening config to sharpen at each decomposition level
 * by specified amplitude factor. Performs inverse transform to reconstruct image and store in result.
 *
 * @param image
 * @param result
 */
void WaveletSharpener::runSharpening(const cv::Mat &image, cv::Mat &result) const
{
    std::cout << "sharpening..." << std::endl;
    std::cout << config.decomp_levels << std::endl;
    wave_object obj = wave_init(config.method.c_str());
    cv::Mat input;

    int size = image.rows * image.cols;
    std::vector<double> input_v(size), diff_v(size), output_v(size);
    double *cLH, *cHL, *cHH;
    int ir, ic;
    image.convertTo(input, CV_64F, 255.0 / 65535.0);

    input_v.assign(input.datastart, input.dataend);
    wt2_object wt = wt2_init(obj, "swt", input.rows, input.cols, config.decomp_levels);
    // setWTConv(wt, "direct");
    double *wavecoeffs = swt2(wt, input.ptr<double>(0));

    for (const auto &[level, amp] : config.level_sharpenings)
    {
        char H[] = "H";
        char V[] = "V";
        char D[] = "D";

        cLH = getWT2Coeffs(wt, wavecoeffs, level, H, &ir, &ic);
        cHL = getWT2Coeffs(wt, wavecoeffs, level, V, &ir, &ic);
        cHH = getWT2Coeffs(wt, wavecoeffs, level, D, &ir, &ic);
        std::cout << "sharpening layer: " << level << std::endl;
        std::cout << "sharpening factr: " << amp << std::endl;

        for (int i = 0; i < ir; i++)
        {
            for (int j = 0; j < ic; j++)
            {
                *(cLH++) *= amp;
                *(cHL++) *= amp;
                *(cHH++) *= amp;
            }
        }
    }

    // dispWT2Coeffs(cLL, ir, ic);
    iswt2(wt, wavecoeffs, output_v.data());
    result = cv::Mat(input.rows, input.cols, CV_64F, &output_v.front());

    // showDetailMap(input, result);

    result.convertTo(result, CV_16UC1, 65535.0 / 255.0);

    // wt2_summary(wt);
    free(wavecoeffs);
    wave_free(obj);
    wt2_free(wt);
    // free wavelet coefficients
}

/**
 * @brief adds wavelet layer and corresponding amplification factor for sharpening
 *
 *
 *
 * @param level layer for wavelet decomposition, specifies spatial scale for sharpening
 * @param amplification amplification factor, specifies factor to multiply decomposed wavelet coefficients
 */

void WaveletSharpener::addLevelSharpening(const int level, const double amplification)
{
    if (level > 5 || level < 1)
    {
        throw std::runtime_error("wavelet sharpening layer must be between 1 and 5");
    }

    if (amplification <= 0)
    {
        throw std::runtime_error("amplification must be positive");
    }
    config.level_sharpenings[level] = amplification;
};

const SharpeningConfig &WaveletSharpener::getSharpenConfig() const
{
    return config;
};

void showDetailMap(const cv::Mat &input, const cv::Mat &output)
{
    cv::Mat effect, effectColor;
    cv::subtract(output, input, effect);
    double maxAbs;
    cv::minMaxLoc(effect, nullptr, &maxAbs);
    maxAbs = std::max(maxAbs, 1e-9);

    // Map [-maxAbs, +maxAbs] to [0, 255], with zero at 127/128.
    effect.convertTo(effect, CV_8U, 127.5 / maxAbs, 127.5);

    cv::applyColorMap(effect, effectColor, cv::COLORMAP_JET);

    cv::imshow("Sharpening effect", effectColor);
    cv::waitKey(0);
}