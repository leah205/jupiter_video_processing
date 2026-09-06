#include <iostream>
#include <opencv2/opencv.hpp>

// g++ main.cpp  -g -o my_program $(pkg-config --cflags --libs opencv4) - turn into make file

void get_array_info(cv::Mat frame)
{
    // 320
    std::cout << "rows: " << frame.rows << std::endl;
    // 320
    std::cout << "cols: " << frame.cols << std::endl;
    std::cout << "channels: " << frame.channels() << std::endl;
    // CV_8U (char)
    std::cout << "data type: " << frame.type() << std::endl;
    std::cout << "depth: " << frame.depth() << std::endl;
}

/**
 * @brief Get the planet mask object
 *
 * Creates planet mask based on hard coded intensity threshold
 *
 * @param frame
 * @param mask
 */

void get_planet_mask(cv::Mat frame, cv::Mat &mask)
{

    cv::Mat blurred;
    // should i blur here?
    cv::GaussianBlur(frame, blurred, cv::Size(3, 3), 0);
    cv::threshold(blurred, mask, 10, 255, cv::THRESH_BINARY);
};

double get_avg_gradient_mag(cv::Mat frame)
{
    int rows = frame.rows;
    int cols = frame.cols;

    double total_mag;

    cv::Mat magx = cv::Mat::zeros(rows, cols, CV_32FC1);
    cv::Mat mag_mat = cv::Mat::zeros(rows, cols, CV_32FC1);
    cv::Mat magy = cv::Mat::zeros(rows, cols, CV_32FC1);

    cv::GaussianBlur(frame, frame, cv::Size(3, 3), 0);

    cv::Sobel(frame, magx, CV_16SC1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::Sobel(frame, magy, CV_16SC1, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);
    // optimize later
    int *mag_x_ptr = magx.ptr<int>(0);
    int *mag_y_ptr = magy.ptr<int>(0);
    for (int r = 0; r < magx.rows; r++)
    {
        for (int c = 0; c < magx.cols; c++)
        {
            total_mag = std::sqrt(pow(*(mag_x_ptr++), 2) + pow(*(mag_y_ptr++), 2));
        }
    }
    double avg_mag = total_mag / (rows * cols);
    return avg_mag;
}

/**
 * @brief returns frame with values normalized between 0 and 255
 *
 * @param frame
 */
cv::Mat preprocess(cv::Mat frame)
{
    cv::Mat processed;
    cv::normalize(frame, processed, 0, 255, cv::NORM_MINMAX);
    return processed;
}

/**
 * @brief aligns frame so that centroid matches with planet centroid of reference
 *
 * @param frame
 * @param cm
 * @param ref
 * @return cv::Mat
 */
cv::Mat align_frame(cv::Mat frame, cv::Point cm, cv::Point ref)
{
    cv::Mat aligned_frame;

    cv::extractChannel(frame, frame, 0);
    double offset_x, offset_y;
    offset_x = cm.x - ref.x;
    offset_y = cm.y - ref.y;

    cv::Mat translation_matrix = (cv::Mat_<double>(2, 3) << 1, 0, -1 * offset_x, 0, 1, -1 * offset_y);
    int height = frame.cols;
    int width = frame.rows;

    cv::warpAffine(frame, aligned_frame, translation_matrix, cv::Size(width, height));
    return aligned_frame;
}

/**
 * @brief Get the center of mass object
 *
 * @param frame
 * @return cv::Point
 */

cv::Point get_center_of_mass(cv::Mat frame)
{
    cv::Mat mask = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC3);

    get_planet_mask(frame, mask);

    cv::extractChannel(mask, mask, 0);
    cv::Moments m = cv::moments(mask, true);

    if (std::abs(m.m00) < 1e-8)
    {
        throw std::runtime_error("Zero-area blob");
    }

    cv::Point p(m.m10 / m.m00, m.m01 / m.m00);
    return p;
}

/**
 * @brief stacks frames and normalizes 16-bit result
 *
 * @param frames
 * @param num_frames
 * @return cv::Mat
 */

cv::Mat stack_images(cv::Mat frames[], int num_frames)
{
    cv::Mat stacked;

    int num_rows = frames[0].rows;
    int num_cols = frames[0].cols;
    cv::Mat sum_mat = cv::Mat::zeros(num_rows, num_cols, CV_32SC1);
    int *sum_ptr = sum_mat.ptr<int>(0);
    for (int i = 0; i < num_frames; i++)
    {
        int nc = num_cols;
        int nl = num_rows;
        cv::Mat cur = frames[i];
        if (cur.isContinuous() && sum_mat.isContinuous())
        {
            nc = num_cols * num_rows;
            nl = 1;
        }
        for (int row = 0; row < nl; row++)
        {
            uchar *p = cur.ptr(row);
            int *sum_ptr = sum_mat.ptr<int>(row);
            for (int col = 0; col < nc; col++)
            {
                *sum_ptr += *p;
                sum_ptr++;
                p++;
            }
        }
    }
    cv::normalize(sum_mat, sum_mat, 0, 1 << 16, cv::NORM_MINMAX, CV_16UC1);

    cv::imshow("sum mat", sum_mat);
    cv::waitKey(0);
    cv::normalize(sum_mat, stacked, 0, 1 << 16, cv::NORM_MINMAX, CV_16UC1);
    return stacked;
}

int main()
{
    cv::VideoCapture cap("../data/2026-03-18-0236_9-Jupiter_656HIA.avi");
    int frame_num = cap.get(cv::CAP_PROP_FRAME_COUNT);
    int fps = cap.get(cv::CAP_PROP_FPS);

    std::cout << "number of frames: " << frame_num << std::endl;
    std::cout << "frames per second: " << fps << std::endl;

    if (!cap.isOpened())
    {
        std::cout << "Error: video object was not opened successfully" << std::endl;
        return -1;
    }
    else
    {
        std::cout << "Video opened successfully" << std::endl;
    }

    cv::Mat translated_frames[frame_num];
    cv::Mat *cur_frame = translated_frames;

    cv::Mat frame;
    bool ret = cap.read(frame);
    frame = preprocess(frame);
    *cur_frame = frame;
    cur_frame++;
    // get_array_info(frame);

    cv::Point ref_cm = get_center_of_mass(frame);

    while (true)
    {

        ret = cap.read(frame);

        if (!ret)
        {
            break;
        }

        preprocess(frame);
        cv::Point cm = get_center_of_mass(frame);
        frame = align_frame(frame, cm, ref_cm);

        *cur_frame = frame;
        cur_frame++;
    }
    cv::Mat stacked = stack_images(translated_frames, frame_num);
    cv::imshow("stacked", stacked);
    cv::waitKey(0);
    cap.release();
    return 0;
}