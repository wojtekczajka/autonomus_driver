#include "road_lane_detector/road_lane_detector_canny.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "common/config_parser.h"

class MockFrameDispatcherClient : public IFrameDispatcherClient {
   public:
    MOCK_METHOD(void, sendFrame, (const cv::Mat&, const std::string&), (override));
};

class RoadLaneDetectorCannyParameterizedTestFixture : public ::testing::TestWithParam<std::tuple<std::string, std::string>> {
   protected:
    MockFrameDispatcherClient mockFrameDispatcherClient;
    RoadLaneDetectorCanny roadLaneDetectorCanny;

    RoadLaneDetectorCannyParameterizedTestFixture() : roadLaneDetectorCanny(mockFrameDispatcherClient) {}
};

TEST_P(RoadLaneDetectorCannyParameterizedTestFixture, testAllDetectedLines) {
    auto [frameName, frameNameDescription] = GetParam();
    cv::Mat frame = cv::imread(frameName);
    ConfigParser configParser(frameNameDescription);
    roadLaneDetectorCanny.processFrame(frame);
    std::cout << configParser.getValue<int>("LEFT_LANE_DETECTED", 0) << " " << configParser.getValue<int>("RIGHT_LANE_DETECTED", 0) << std::endl;
    bool shoouldLeftLaneBeDetected = configParser.getValue<int>("LEFT_LANE_DETECTED", 0);
    bool shoouldRightLaneBeDetected = configParser.getValue<int>("RIGHT_LANE_DETECTED", 0);
    EXPECT_EQ(shoouldLeftLaneBeDetected, roadLaneDetectorCanny.isLeftVerticalLaneDetected());
    EXPECT_EQ(shoouldRightLaneBeDetected, roadLaneDetectorCanny.isRightVerticalLaneDetected());
}

INSTANTIATE_TEST_SUITE_P(
    shouldDetectRightAndLeftLane,
    RoadLaneDetectorCannyParameterizedTestFixture,
    ::testing::Values(
        std::make_tuple("../frames/frame0.jpg", "../frames/frame0_description.txt"),
        std::make_tuple("../frames/frame1.jpg", "../frames/frame1_description.txt"),
        std::make_tuple("../frames/frame2.jpg", "../frames/frame2_description.txt"),
        std::make_tuple("../frames/frame3.jpg", "../frames/frame3_description.txt"),
        std::make_tuple("../frames/frame4.jpg", "../frames/frame4_description.txt"),
        std::make_tuple("../frames/frame5.jpg", "../frames/frame5_description.txt"),
        std::make_tuple("../frames/frame6.jpg", "../frames/frame6_description.txt"),
        std::make_tuple("../frames/frame7.jpg", "../frames/frame7_description.txt"),
        std::make_tuple("../frames/frame8.jpg", "../frames/frame8_description.txt"),
        std::make_tuple("../frames/frame9.jpg", "../frames/frame9_description.txt"),
        std::make_tuple("../frames/frame10.jpg", "../frames/frame10_description.txt"),
        std::make_tuple("../frames/frame11.jpg", "../frames/frame11_description.txt"),
        std::make_tuple("../frames/frame12.jpg", "../frames/frame12_description.txt"),
        std::make_tuple("../frames/frame13.jpg", "../frames/frame13_description.txt"),
        std::make_tuple("../frames/frame14.jpg", "../frames/frame14_description.txt"),
        std::make_tuple("../frames/frame15.jpg", "../frames/frame15_description.txt"),
        std::make_tuple("../frames/frame16.jpg", "../frames/frame16_description.txt"),
        std::make_tuple("../frames/frame17.jpg", "../frames/frame17_description.txt"),
        std::make_tuple("../frames/frame18.jpg", "../frames/frame18_description.txt"),
        std::make_tuple("../frames/frame19.jpg", "../frames/frame19_description.txt"),
        std::make_tuple("../frames/frame20.jpg", "../frames/frame20_description.txt"),
        std::make_tuple("../frames/frame21.jpg", "../frames/frame21_description.txt"),
        std::make_tuple("../frames/frame22.jpg", "../frames/frame22_description.txt"),
        std::make_tuple("../frames/frame23.jpg", "../frames/frame23_description.txt"),
        std::make_tuple("../frames/frame24.jpg", "../frames/frame24_description.txt"),
        std::make_tuple("../frames/frame25.jpg", "../frames/frame25_description.txt")));

INSTANTIATE_TEST_SUITE_P(
    shouldDetectLeftLane,
    RoadLaneDetectorCannyParameterizedTestFixture,
    ::testing::Values(
        std::make_tuple("../frames/frame26.jpg", "../frames/frame26_description.txt"),
        std::make_tuple("../frames/frame27.jpg", "../frames/frame27_description.txt"),
        std::make_tuple("../frames/frame28.jpg", "../frames/frame28_description.txt"),
        std::make_tuple("../frames/frame29.jpg", "../frames/frame29_description.txt"),
        std::make_tuple("../frames/frame30.jpg", "../frames/frame30_description.txt"),
        std::make_tuple("../frames/frame31.jpg", "../frames/frame31_description.txt"),
        std::make_tuple("../frames/frame32.jpg", "../frames/frame32_description.txt"),
        std::make_tuple("../frames/frame33.jpg", "../frames/frame33_description.txt"),
        std::make_tuple("../frames/frame34.jpg", "../frames/frame34_description.txt"),
        std::make_tuple("../frames/frame35.jpg", "../frames/frame35_description.txt"),
        std::make_tuple("../frames/frame36.jpg", "../frames/frame36_description.txt"),
        std::make_tuple("../frames/frame37.jpg", "../frames/frame37_description.txt"),
        std::make_tuple("../frames/frame38.jpg", "../frames/frame38_description.txt"),
        std::make_tuple("../frames/frame39.jpg", "../frames/frame39_description.txt"),
        std::make_tuple("../frames/frame40.jpg", "../frames/frame40_description.txt"),
        std::make_tuple("../frames/frame41.jpg", "../frames/frame41_description.txt"),
        std::make_tuple("../frames/frame42.jpg", "../frames/frame42_description.txt"),
        std::make_tuple("../frames/frame43.jpg", "../frames/frame43_description.txt"),
        std::make_tuple("../frames/frame44.jpg", "../frames/frame44_description.txt"),
        std::make_tuple("../frames/frame45.jpg", "../frames/frame45_description.txt"),
        std::make_tuple("../frames/frame46.jpg", "../frames/frame46_description.txt"),
        std::make_tuple("../frames/frame47.jpg", "../frames/frame47_description.txt"),
        std::make_tuple("../frames/frame48.jpg", "../frames/frame48_description.txt"),
        std::make_tuple("../frames/frame49.jpg", "../frames/frame49_description.txt"),
        std::make_tuple("../frames/frame50.jpg", "../frames/frame50_description.txt"),
        std::make_tuple("../frames/frame51.jpg", "../frames/frame51_description.txt"),
        std::make_tuple("../frames/frame52.jpg", "../frames/frame52_description.txt")));