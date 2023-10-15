#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

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

    int minHue = 0, maxHue = 0;
    int minSat = 0, maxSat = 0;
    int minValue = 0, maxValue = 255;

    createTrackbar("Min Hue", "Video", &minHue, 255);
    createTrackbar("Max Hue", "Video", &maxHue, 255);
    createTrackbar("Min Saturation", "Video", &minSat, 255);
    createTrackbar("Max Saturation", "Video", &maxSat, 255);
    createTrackbar("Min Value", "Video", &minValue, 255);
    createTrackbar("Max Value", "Video", &maxValue, 255);

    Mat frame, hsvFrame, mask;

    while (true) {
        cap >> frame;
        std::vector<cv::Mat> channels;
        cv::split(frame, channels);
        for (int i = 0; i < 3; i++) {
            cv::equalizeHist(channels[i], channels[i]);
        }
        
        cv::merge(channels, frame);
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
