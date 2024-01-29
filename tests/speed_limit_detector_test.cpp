#include "road_sign_detector/speed_limit_detector.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "common/config_parser.h"

class SpeedLimitDetectorParameterizedTestFixture : public ::testing::TestWithParam<std::tuple<std::string, std::string>> {
   protected:
    SpeedLimitDetector speedLimitDetector;

    SpeedLimitDetectorParameterizedTestFixture() : speedLimitDetector() {}
};

TEST_P(SpeedLimitDetectorParameterizedTestFixture, testSpeedLimitDetector) {
    auto [frameName, frameNameDescription] = GetParam();
    cv::Mat frame = cv::imread(frameName);
    ConfigParser configParser(frameNameDescription);
    speedLimitDetector.detectSpeedLimit(frame);
    int speedLimit = configParser.getValue<int>("SPEED_LIMIT", -1);
    EXPECT_TRUE(speedLimitDetector.isSignDetected());
    EXPECT_EQ(speedLimit, speedLimitDetector.getSpeedLimitValue());
}

INSTANTIATE_TEST_SUITE_P(
    testSpeedLimitDetector,
    SpeedLimitDetectorParameterizedTestFixture,
    ::testing::Values(
        std::make_tuple("../frames/test_data56.jpg", "../frames/test_data56_description.txt"),
        std::make_tuple("../frames/test_data57.jpg", "../frames/test_data57_description.txt"),
        std::make_tuple("../frames/test_data58.jpg", "../frames/test_data58_description.txt"),
        std::make_tuple("../frames/test_data59.jpg", "../frames/test_data59_description.txt"),
        std::make_tuple("../frames/test_data60.jpg", "../frames/test_data60_description.txt"),
        std::make_tuple("../frames/test_data61.jpg", "../frames/test_data61_description.txt"),
        std::make_tuple("../frames/test_data62.jpg", "../frames/test_data62_description.txt"),
        std::make_tuple("../frames/test_data63.jpg", "../frames/test_data63_description.txt"),
        std::make_tuple("../frames/test_data64.jpg", "../frames/test_data64_description.txt"),
        std::make_tuple("../frames/test_data65.jpg", "../frames/test_data65_description.txt"),
        std::make_tuple("../frames/test_data66.jpg", "../frames/test_data66_description.txt"),
        std::make_tuple("../frames/test_data67.jpg", "../frames/test_data67_description.txt"),
        std::make_tuple("../frames/test_data68.jpg", "../frames/test_data68_description.txt"),
        std::make_tuple("../frames/test_data69.jpg", "../frames/test_data69_description.txt"),
        std::make_tuple("../frames/test_data70.jpg", "../frames/test_data70_description.txt"),
        std::make_tuple("../frames/test_data71.jpg", "../frames/test_data71_description.txt"),
        std::make_tuple("../frames/test_data72.jpg", "../frames/test_data72_description.txt")));