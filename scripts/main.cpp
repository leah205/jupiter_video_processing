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

/**
 * @brief
 *
 * @param frame
 */
cv::Mat preprocess(cv::Mat frame)
{
    cv::Mat processed;
    cv::normalize(frame, processed, 0, 255, cv::NORM_MINMAX);
    return processed;
}

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

cv::Mat stack_images(cv::Mat frames[], int num_frames)
{
    cv::Mat stacked;

    int num_rows = frames[0].rows;
    int num_cols = frames[0].cols;
    cv::Mat sum_mat = cv::Mat::zeros(num_rows, num_cols, CV_32SC1);
    int *sum_ptr = sum_mat.ptr<int>(0);
    for (int i = 0; i < num_frames - 1; i++)
    {
        if (i == 0)
        {
            get_array_info(frames[0]);
            cv::imshow("frame", frames[i]);
            cv::waitKey(0);
        }
        cv::Mat cur = frames[i];
        for (int row = 0; row < num_rows; row++)
        {
            // why not just use one pointer here?
            uchar *p = cur.ptr(row);
            int *sum_ptr = sum_mat.ptr<int>(row);
            for (int col = 0; col < num_cols; col++)
            {
                *sum_ptr += *p;
                sum_ptr++;
                p++;
            }
        }
    }
    cv::normalize(sum_mat, stacked, 0, 1 << 16, cv::NORM_MINMAX, CV_16UC1);
    return stacked;
}

int main()
{
    cv::VideoCapture cap("../data/2026-03-18-0236_9-Jupiter_656HIA.avi");
    int frame_num = cap.get(cv::CAP_PROP_FRAME_COUNT);

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