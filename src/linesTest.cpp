#include <iostream>

#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;
using namespace std;

cv::Mat cropRoiFromLineDetection(const cv::Mat& inputFrame) {
    int width = inputFrame.cols;
    int height = inputFrame.rows;

    cv::Mat mask = cv::Mat::zeros(inputFrame.size(), CV_8UC1);
    std::vector<cv::Point> triangleVertices;
    triangleVertices.push_back(cv::Point(0, height));      
    triangleVertices.push_back(cv::Point(width / 2, 0));   
    triangleVertices.push_back(cv::Point(width, height));  

    cv::fillPoly(mask, std::vector<std::vector<cv::Point>>(1, triangleVertices), cv::Scalar(255));
    int rectHeight = height / 2;
    cv::Rect topRect(0, 0, width, rectHeight);

    // Fill the rectangular region in the mask
    cv::rectangle(mask, topRect, cv::Scalar(0), -1);  // -1 indicates filled rectangle

    rectHeight = height;
    int rectWidth = width / 4;            // 1/4 of the frame width
    int rectX = (width - rectWidth) / 2;  // Centered horizontally
    topRect = cv::Rect(rectX, 0, rectWidth, rectHeight);
    cv::rectangle(mask, topRect, cv::Scalar(0), -1);

    // Create a masked image by bitwise AND operation
    cv::Mat masked_image;
    cv::bitwise_and(inputFrame, inputFrame, masked_image, mask);

    return masked_image;
}

int main(int argc, char** argv) {
    // Declare the output variables
    Mat dst, cdst, cdstP;
    const char* default_file = "frame.png";
    const char* filename = argc >= 2 ? argv[1] : default_file;
    Mat src = imread(filename, IMREAD_GRAYSCALE);
    if (src.empty()) {
        printf(" Error opening image\n");
        printf(" Program Arguments: [image_name -- default %s] \n", default_file);
        return -1;
    }
    Canny(src, dst, 30, 90, 3);
    dst = cropRoiFromLineDetection(dst);
    // Copy edges to the images that will display the results in BGR
    cvtColor(dst, cdst, COLOR_GRAY2BGR);
    cdstP = cdst.clone();
    vector<Vec2f> lines;                               // will hold the results of the detection
    // Probabilistic Line Transform
    vector<Vec4i> linesP;                                  // will hold the results of the detection
    HoughLinesP(dst, linesP, 6, CV_PI / 60, 110, 30, 10);  // runs the actual detection
    // Draw the lines
    for (size_t i = 0; i < linesP.size(); i++) {
        Vec4i l = linesP[i];
        line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
    }

    std::vector<cv::Point> all_point_pairs_left;
    std::vector<cv::Point> all_point_pairs_right;
    for (Vec4i line : linesP) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
        double slope = static_cast<double>(y2 - y1) / (x2 - x1);
        if (std::abs(slope) < 0.5) { // check if abs not return int :)
            continue;
        }
        if (slope <= 0) {
            all_point_pairs_left.push_back(cv::Point(x1, y1));
            all_point_pairs_left.push_back(cv::Point(x2, y2));
        } else {
            all_point_pairs_right.push_back(cv::Point(x1, y1));
            all_point_pairs_right.push_back(cv::Point(x2, y2));
        }
    }

    int image_height = src.rows;
    double min_y = image_height * 0.6;
    double max_y = image_height;

    // Calculate line parameters using fitLine
    Vec4f left_line_params, right_line_params;
    fitLine(all_point_pairs_left, left_line_params, DIST_L2, 0, 0.01, 0.01);
    fitLine(all_point_pairs_right, right_line_params, DIST_L2, 0, 0.01, 0.01);

    // Calculate coordinates for min_y and max_y using the line parameters
    int left_x_start = static_cast<int>(left_line_params[2] - (left_line_params[0] / left_line_params[1]) * max_y);
    int left_x_end = static_cast<int>(left_line_params[2] - (left_line_params[0] / left_line_params[1]) * min_y);
    int right_x_start = static_cast<int>(right_line_params[2] - (right_line_params[0] / right_line_params[1]) * max_y);
    int right_x_end = static_cast<int>(right_line_params[2] - (right_line_params[0] / right_line_params[1]) * min_y);

    std::cout << "Left Line: (" << left_x_start << ", " << max_y << ") to (" << left_x_end << ", " << min_y << ")\n";
    std::cout << "Right Line: (" << right_x_start << ", " << max_y << ") to (" << right_x_end << ", " << min_y << ")\n";

    // Show results
    imshow("Source", src);
    imshow("Detected Lines (in red) - Standard Hough Line Transform", cdst);
    imshow("Detected Lines (in red) - Probabilistic Line Transform", cdstP);
    
    // Draw the left line in blue
    cv::line(src, cv::Point(left_x_start, max_y), cv::Point(left_x_end, min_y), cv::Scalar(255, 0, 0), 2);

    // Draw the right line in green
    cv::line(src, cv::Point(right_x_start, max_y), cv::Point(right_x_end, min_y), cv::Scalar(0, 255, 0), 2);
    cv::imshow("detected left and right lines", src);
    // Wait and Exit
    waitKey();
    return 0;
}