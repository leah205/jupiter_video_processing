
#include <opencv2/opencv.hpp>

/**
 * @brief Get the mean intensity object
 *
 * @param frame single channel matrix with data type CV_8UC1
 * @return mean intensity of pixels in frame
 */

double get_mean_intensity(cv::Mat frame)
{
    int nr = frame.rows;
    int nc = frame.cols;
    double sum;

    if (frame.isContinuous())
    {
        nc = nc * nr;
        nr = 1;
    }

    for (int r = 0; r < nr; r++)
    {
        uchar *ptr = frame.ptr(r);
        for (int c = 0; c < nc; c++)
        {

            sum += (*ptr);
            ptr++;
        }
    }
    std::cout << "mean intensity: " << sum / (nr * nc) << std::endl;
    return sum / (nr * nc);
}

/**
 * @brief normalize intensity to reference
 *
 * Shifts all pixels by difference in means with reference frame
 * so that all frames have approximately the same mean intensity
 *
 * @param frame
 * @param ref_mean
 */

void normalize_to_mean_intensity(cv::Mat &frame, double ref_mean)
{
    double mean_intensity = get_mean_intensity(frame);
    double shift = round(ref_mean - mean_intensity);

    int nr = frame.rows;
    int nc = frame.cols;
    double sum;

    if (frame.isContinuous())
    {
        nc = nc * nr;
        nr = 1;
    }

    for (int r = 0; r < nr; r++)
    {
        uchar *ptr = frame.ptr(r);
        for (int c = 0; c < nc; c++)
        {
            int new_val = (*ptr + shift);
            if (new_val < 0)
            {
                *ptr = 0;
            }
            else if (new_val > 255)
            {
                *ptr = 255;
            }
            *ptr = (uchar)new_val;
        }
    }
    cv::imshow("normalized frame", frame);
    cv::waitKey(0);
}
