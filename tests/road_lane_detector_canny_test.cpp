#include "road_lane_detector/road_lane_detector_canny.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "common/config_parser.h"

class MockFrameDispatcherClient : public IFrameDispatcherClient {
   public:
    MOCK_METHOD(void, sendFrame, (const cv::Mat&, const std::string&), (override));
};

class RoadLaneDetectorCannyTest : public ::testing::Test {
   protected:
    MockFrameDispatcherClient mockFrameDispatcherClient;
    RoadLaneDetectorCanny roadLaneDetectorCanny;

    RoadLaneDetectorCannyTest() : roadLaneDetectorCanny(mockFrameDispatcherClient) {}

    void SetUp() override {
    }

    void TearDown() override {
    }

    void shouldDetectRightAndLeftLane(const std::string& frameName, const std::string& frameNameDescription) {
        cv::Mat frame = cv::imread(frameName);
        ConfigParser configParser(frameNameDescription);
        std::string correctAction = configParser.getValue<std::string>("RIGHT_LINE_DETECTED", "");
        roadLaneDetectorCanny.processFrame(frame);
        EXPECT_TRUE(roadLaneDetectorCanny.isRightVerticalLaneDetected());
        EXPECT_TRUE(roadLaneDetectorCanny.isLeftVerticalLaneDetected());
    }
};

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane0) {
    shouldDetectRightAndLeftLane("../frames/frame0.jpg", "../frames/frame0_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane1) {
    shouldDetectRightAndLeftLane("../frames/frame1.jpg", "../frames/frame1_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane2) {
    shouldDetectRightAndLeftLane("../frames/frame2.jpg", "../frames/frame2_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane3) {
    shouldDetectRightAndLeftLane("../frames/frame3.jpg", "../frames/frame3_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane4) {
    shouldDetectRightAndLeftLane("../frames/frame4.jpg", "../frames/frame4_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane5) {
    shouldDetectRightAndLeftLane("../frames/frame5.jpg", "../frames/frame5_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane6) {
    shouldDetectRightAndLeftLane("../frames/frame6.jpg", "../frames/frame6_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane7) {
    shouldDetectRightAndLeftLane("../frames/frame7.jpg", "../frames/frame7_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane8) {
    shouldDetectRightAndLeftLane("../frames/frame8.jpg", "../frames/frame8_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane9) {
    shouldDetectRightAndLeftLane("../frames/frame9.jpg", "../frames/frame9_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane10) {
    shouldDetectRightAndLeftLane("../frames/frame10.jpg", "../frames/frame10_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane11) {
    shouldDetectRightAndLeftLane("../frames/frame11.jpg", "../frames/frame11_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane12) {
    shouldDetectRightAndLeftLane("../frames/frame12.jpg", "../frames/frame12_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane13) {
    shouldDetectRightAndLeftLane("../frames/frame13.jpg", "../frames/frame13_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane14) {
    shouldDetectRightAndLeftLane("../frames/frame14.jpg", "../frames/frame14_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane15) {
    shouldDetectRightAndLeftLane("../frames/frame15.jpg", "../frames/frame15_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane16) {
    shouldDetectRightAndLeftLane("../frames/frame16.jpg", "../frames/frame16_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane17) {
    shouldDetectRightAndLeftLane("../frames/frame17.jpg", "../frames/frame17_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane18) {
    shouldDetectRightAndLeftLane("../frames/frame18.jpg", "../frames/frame18_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane19) {
    shouldDetectRightAndLeftLane("../frames/frame19.jpg", "../frames/frame19_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane20) {
    shouldDetectRightAndLeftLane("../frames/frame20.jpg", "../frames/frame20_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane21) {
    shouldDetectRightAndLeftLane("../frames/frame21.jpg", "../frames/frame21_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane22) {
    shouldDetectRightAndLeftLane("../frames/frame22.jpg", "../frames/frame22_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane23) {
    shouldDetectRightAndLeftLane("../frames/frame23.jpg", "../frames/frame23_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane24) {
    shouldDetectRightAndLeftLane("../frames/frame24.jpg", "../frames/frame24_description.txt");
}

TEST_F(RoadLaneDetectorCannyTest, shouldDetectRightAndLeftLane25) {
    shouldDetectRightAndLeftLane("../frames/frame25.jpg", "../frames/frame25_description.txt");
}

