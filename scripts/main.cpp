#include <iostream>
#include <opencv2/opencv.hpp>

// compile with g++ -o main main.cpp
// g++ main.cpp -o my_program $(pkg-config --cflags --libs opencv4) - turn into make file

void get_planet_mask(cv::Mat frame, cv::Mat mask)
{
    cv::Mat blurred;
    // frame.copyTo(blurred);
    // should i blur here?
    cv::GaussianBlur(frame, blurred, cv::Size(3, 3), 0);
    cv::threshold(blurred, mask, 10, 255, cv::THRESH_BINARY);
};

void compress(cv::Mat frame)
{
    cv::extractChannel(frame, frame, 0);
}

void align_frame(cv::Mat frame, cv::Point cm, cv::Point ref)
{
    double offset_x, offset_y;
    offset_x = cm.x - ref.x;
    offset_y = cm.y - ref.y;

    cv::Mat translation_matrix = (cv::Mat_<double>(2, 3) << 1, 0, -1 * offset_x, 0, 1, -1 * offset_y);
    int height = frame.cols;
    int width = frame.rows;
    std::cout << "cm x: " << cm.x << " ref x: " << ref.x << std::endl;

    cv::warpAffine(frame, frame, translation_matrix, cv::Size(width, height));
    cv::imshow("aligned", frame);
    cv::waitKey(0);
}

cv::Point get_center_of_mass(cv::Mat frame)
{
    cv::Mat mask = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC3);

    get_planet_mask(frame, mask);

    cv::extractChannel(mask, mask, 0);

    std::cout << mask.channels() << std::endl;
    cv::Moments m = cv::moments(mask, true);

    if (std::abs(m.m00) < 1e-8)
    {
        throw std::runtime_error("Zero-area blob");
    }

    cv::Point p(m.m10 / m.m00, m.m01 / m.m00);
    return p;

    // cv::circle(frame, p, 5, cv::Scalar(128, 0, 0), -1);
    // cv::imshow("image with center", frame);
    // cv::waitKey(0);
    // std::cout
    //     << "x center " << m.m10 << std::endl;
    // std::cout << "y center " << m.m01 << std::endl;
}

// void stack_images(cv::Mat frames[], cv::Mat stacked[], int num_frames)
// {
//     int num_rows = frames[0].rows;
//     int num_cols = frames[0].cols;
//     // what data type to pout here?
//     cv::Mat sum_mat = cv::Mat::zeros(num_rows, num_cols, CV_16F);
//     uchar *sum_ptr = sum_mat.data;
//     for (int i = 0; i < num_frames; i++)
//     {
//         cv::Mat cur = frames[i];
//         for (int row = 0; row < num_rows; ++row)
//         {
//             // why not just use one pointer here?
//             uchar *p = cur.ptr(row);
//             for (int col = 0; col < num_cols; ++col)
//             {
//                 *sum_ptr += *p;
//                 p++;
//             }
//         }
//     }

//     cv::Mat stacked = sum_mat / num_frames;
// }

void get_array_info(cv::Mat frame)
{
    // 320
    std::cout << "rows: " << frame.rows << std::endl;
    // 320
    std::cout << "cols: " << frame.cols << std::endl;
    std::cout << "channels: " << frame.channels() << std::endl;
    // CV_8U
    std::cout << "data type: " << frame.type() << std::endl;
    std::cout << "depth: " << frame.depth() << std::endl;
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

    // cv::Mat frames[frame_num];
    // uchar *cur_frame = frames.ptr(0);
    cv::Mat frame;
    bool ret = cap.read(frame);
    compress(frame);

    cv::Point ref_cm = get_center_of_mass(frame);

    while (true)
    {

        ret = cap.read(frame);

        if (!ret)
        {
            break;
        }

        cv::Point cm = get_center_of_mass(frame);
        align_frame(frame, cm, ref_cm);
        // frames[]
    }

    cap.release();
    return 0;
}