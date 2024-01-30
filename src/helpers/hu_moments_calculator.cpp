#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <ImagePath>" << endl;
        return -1;
    }

    Mat binary = imread(argv[1], IMREAD_GRAYSCALE);
    if (binary.empty()) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    Moments moments = cv::moments(binary, true); 

    double huMoments[7];
    cv::HuMoments(moments, huMoments);

    for (int i = 0; i < 7; i++) 
        cout << huMoments[i]  << " ";
    cout << std::endl;
    return 0;
}
