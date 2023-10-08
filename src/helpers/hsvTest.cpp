#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int main() {
    // VideoCapture cap("output_video.avi");
    VideoCapture cap(0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 368);
    cap.set(cv::CAP_PROP_FPS, 15);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera" << std::endl;
        return -1;
    }

    namedWindow("Video", WINDOW_AUTOSIZE);

    int minHue = 0, maxHue = 30;
    int minSat = 100, maxSat = 255;
    int minValue = 100, maxValue = 255;

    createTrackbar("Min Hue", "Video", &minHue, 179);
    createTrackbar("Max Hue", "Video", &maxHue, 23);
    createTrackbar("Min Saturation", "Video", &minSat, 255);
    createTrackbar("Max Saturation", "Video", &maxSat, 255);
    createTrackbar("Min Value", "Video", &minValue, 40);
    createTrackbar("Max Value", "Video", &maxValue, 255);

    Mat frame, hsvFrame, mask;

    while (true) {
        cap >> frame; 

        cvtColor(frame, hsvFrame, COLOR_BGR2HSV);
        inRange(hsvFrame, Scalar(minHue, minSat, minValue), Scalar(maxHue, maxSat, maxValue), mask);
        Mat result;
        bitwise_and(frame, frame, result, mask);
        imshow("Video", result);
        if (waitKey(0) == 'q') {
            break;
        }
    }
    cap.release();
    destroyAllWindows();

    return 0;
}
