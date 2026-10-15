
#include <opencv2/opencv.hpp>

/**
 * @brief computes ECC correlation score between two frame
 *
 * @param ref_frame matrix object
 * @param aligned_frame matrix object
 * @return double
 */

double compute_ecc(const cv::Mat ref_frame, const cv::Mat aligned_frame)
{
    double corr = cv::computeECC(ref_frame, aligned_frame);
    return corr;
}

/**
 * @brief applies translation to maximize ECC correlation between two frames
 *
 * outputs translated aligned_frame into new_aligned matrix
 *
 * @param ref_frame reference frame for alignment
 * @param aligned_frame input frame for translation
 * @param new_aligned translation output
 * @return double magnitude of tranlsation shift
 */

double transform_ecc(const cv::Mat ref_frame, const cv::Mat aligned_frame, cv::Mat &new_aligned)
{

    cv::Mat warp_matrix = cv::Mat::eye(2, 3, CV_32F);
    // cv::imshow("refernce", ref_frame);
    // cv::waitKey(0);

    // cv::imshow("aligned", aligned_frame);
    // cv::waitKey(0);
    // std::cout << compute_ecc(ref_frame, aligned_frame);
    double ecc = cv::findTransformECC(ref_frame, aligned_frame, warp_matrix, cv::MOTION_TRANSLATION);
    // std::cout << "ecc: " << ecc << std::endl;
    cv::warpAffine(aligned_frame, new_aligned, warp_matrix, cv::Size(ref_frame.cols, ref_frame.rows), cv::INTER_LINEAR | cv::WARP_INVERSE_MAP);
    // std::cout << warp_matrix << std::endl;
    // std::cout << compute_ecc(ref_frame, new_aligned) << std::endl;

    double t_x = warp_matrix.at<float>(0, 2);
    double t_y = warp_matrix.at<float>(1, 2);
    double shift_mag = std::sqrt(t_x * t_x + t_y * t_y);

    return shift_mag;
}

static void get_row_max(cv::Mat frame, int row, cv::Point cm)
{
    cv::Mat left_mag, right_mag;
    cv::Mat frame_row = frame.row(row);
    cv::Rect roi_left(0, 0, cm.x, 1);
    cv::Rect roi_right(cm.x, 0, frame_row.cols, 1);
    cv::Mat frame_left = frame_row(roi_left);
    cv::Mat frame_right = frame_row(roi_right);

    cv::Sobel(frame_left, left_mag, CV_32FC1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::Sobel(frame_right, right_mag, CV_32FC1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);

    double minVal;
    double maxVal;
    cv::Point minLoc;
    cv::Point maxLoc;

    minMaxLoc(left_mag, &minVal, &maxVal, &minLoc, &maxLoc);
    std::cout << "edge left: " << maxLoc.x << std::endl;
    minMaxLoc(left_mag, &minVal, &maxVal, &minLoc, &maxLoc);
    std::cout << "edge right: " << maxLoc.y << std::endl;
}

static void detect_limb_shift(cv::Mat ref_frame, const cv::Mat frame, cv::Point cm)
{
    cv::imshow("ref_frame", ref_frame);
    int row_start = 150;
    int row_end = 190;
    int r = cm.x;
    int c = cm.y;
    get_row_max(ref_frame, r, cm);
    get_row_max(frame, r, cm);
    // for (int r = row_start; r < row_end; r++)
    // {
    // }
    cv::waitKey(0);
}

static void template_match(const cv::Mat ref_frame, const cv::Mat aligned_frame, cv::Rect roi)
{

    // cv::Mat template_mask = cv::Mat::zeros(ref_frame.rows, ref_frame.cols, CV_8UC1);
    // cv::rectangle(template_mask, roi, cv::Scalar(255), cv::FILLED);
    cv::Mat template_frame = ref_frame(roi);
    // cv::imshow("rect", ref_frame(roi));
    // cv::waitKey(0);
    cv::Mat result;
    cv::matchTemplate(aligned_frame, template_frame, result, cv::TM_CCOEFF_NORMED);
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
    std::cout << "fit: " << maxVal << std::endl;
    std::cout << "maxLoc x: " << maxLoc.x << " maxLoc y: " << maxLoc.y << std::endl;
    std::cout << "x shift: " << maxLoc.x - roi.x << std::endl;
    std::cout << "y shift: " << maxLoc.y - roi.y << std::endl;
}

//  if (i == select_amount - 1)
//         {
//             detect_limb_shift(ref_frame, aligned, cm);
//             //     cv::Rect roi1 = cv::Rect(130, 126, 40, 80);
//             //     // cv::Rect roi2 = cv::Rect(50)
//             //     cv::Rect roi2 = cv::Rect(156, 145, 30, 80);
//             //     cv::Rect roi3 = cv::Rect(130, 126, 100, 80);
//             //     cv::rectangle(ref_frame, roi2, cv::Scalar(255), 1);
//             //     cv::rectangle(ref_frame, roi1, cv::Scalar(255), 1);
//             //     cv::rectangle(ref_frame, roi3, cv::Scalar(255), 1);
//             //     cv::imshow("rois", ref_frame);
//             //     cv::waitKey(0);
//             //     template_match(aligned, ref_frame, roi1);
//             //     template_match(aligned, ref_frame, roi2);
//             //     template_match(aligned, ref_frame, roi3);
//         }