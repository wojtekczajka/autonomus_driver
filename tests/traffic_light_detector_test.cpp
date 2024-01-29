#include "traffic_light_detector/traffic_light_detector.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "common/config_parser.h"

class TrafficLightDetectorParameterizedTestFixture : public ::testing::TestWithParam<std::tuple<std::string, std::string>> {
   protected:
    TrafficLightDetector trafficLightDetector;

    TrafficLightDetectorParameterizedTestFixture() : trafficLightDetector() {}
};

TEST_P(TrafficLightDetectorParameterizedTestFixture, testTrafficLightDetector) {
    auto [frameName, frameNameDescription] = GetParam();
    cv::Mat frame = cv::imread(frameName);
    ConfigParser configParser(frameNameDescription);
    trafficLightDetector.detectTrafficLight(frame);
    int trafficLight = configParser.getValue<int>("TRAFFIC_LIGHT_COLOR", -1);
    EXPECT_TRUE(trafficLightDetector.isTrafficLightDetected());
    EXPECT_EQ(trafficLight, trafficLightDetector.getTrafficLight());
}

INSTANTIATE_TEST_SUITE_P(
    testTrafficLightDetector,
    TrafficLightDetectorParameterizedTestFixture,
    ::testing::Values(
        std::make_tuple("../frames/test_data0.jpg", "../frames/test_data0_description.txt"),
        std::make_tuple("../frames/test_data1.jpg", "../frames/test_data1_description.txt"),
        std::make_tuple("../frames/test_data2.jpg", "../frames/test_data2_description.txt"),
        std::make_tuple("../frames/test_data3.jpg", "../frames/test_data3_description.txt"),
        std::make_tuple("../frames/test_data4.jpg", "../frames/test_data4_description.txt"),
        std::make_tuple("../frames/test_data5.jpg", "../frames/test_data5_description.txt"),
        std::make_tuple("../frames/test_data6.jpg", "../frames/test_data6_description.txt"),
        std::make_tuple("../frames/test_data7.jpg", "../frames/test_data7_description.txt"),
        std::make_tuple("../frames/test_data8.jpg", "../frames/test_data8_description.txt"),
        std::make_tuple("../frames/test_data9.jpg", "../frames/test_data9_description.txt"),
        std::make_tuple("../frames/test_data10.jpg", "../frames/test_data10_description.txt"),
        std::make_tuple("../frames/test_data11.jpg", "../frames/test_data11_description.txt"),
        std::make_tuple("../frames/test_data12.jpg", "../frames/test_data12_description.txt"),
        std::make_tuple("../frames/test_data13.jpg", "../frames/test_data13_description.txt"),
        std::make_tuple("../frames/test_data14.jpg", "../frames/test_data14_description.txt"),
        std::make_tuple("../frames/test_data15.jpg", "../frames/test_data15_description.txt"),
        std::make_tuple("../frames/test_data16.jpg", "../frames/test_data16_description.txt"),
        std::make_tuple("../frames/test_data17.jpg", "../frames/test_data17_description.txt"),
        std::make_tuple("../frames/test_data18.jpg", "../frames/test_data18_description.txt"),
        std::make_tuple("../frames/test_data19.jpg", "../frames/test_data19_description.txt"),
        std::make_tuple("../frames/test_data20.jpg", "../frames/test_data20_description.txt")));