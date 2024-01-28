#include "steering/auto_pilot.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <tuple>

#include "common/config_parser.h"

class MockDistanceClient : public IDistanceClient {
   public:
    MOCK_METHOD(double, getDistance, (), (override));
};

class MockSteeringClient : public ISteeringClient {
   public:
    MOCK_METHOD(bool, start, (), (override));
    MOCK_METHOD(bool, stop, (), (override));
    MOCK_METHOD(bool, center, (), (override));
    MOCK_METHOD(bool, turnLeft, (int), (override));
    MOCK_METHOD(bool, turnRight, (int), (override));
    MOCK_METHOD(bool, driveForward, (int), (override));
    MOCK_METHOD(bool, driveBackward, (int), (override));
};

class AutoPilotParameterizedTestFixture : public ::testing::TestWithParam<std::tuple<std::string, std::string>> {
   protected:
    Logger logger;
    MockDistanceClient mockDistanceClient;
    MockSteeringClient mockSteeringClient;
    RoadLaneDetectorCanny roadLaneDetectorCanny;
    AutoPilot autoPilot;
    TrafficLightDetector trafficLightDetector;
    MandatorySignDetector mandatorySignDetector;
    SpeedLimitDetector speedLimitDetector;

    AutoPilotParameterizedTestFixture() : logger(),
                                          mockDistanceClient(),
                                          mockSteeringClient(),
                                          roadLaneDetectorCanny(),
                                          trafficLightDetector(),
                                          speedLimitDetector(),
                                          mandatorySignDetector(),
                                          autoPilot(trafficLightDetector, speedLimitDetector, mandatorySignDetector, roadLaneDetectorCanny, mockSteeringClient, mockDistanceClient, logger) {}

    void SetUp() override {
        ON_CALL(mockDistanceClient, getDistance()).WillByDefault(::testing::Return(100.0));
        ON_CALL(mockSteeringClient, start()).WillByDefault(::testing::Return(true));
        ON_CALL(mockSteeringClient, stop()).WillByDefault(::testing::Return(true));
        ON_CALL(mockSteeringClient, center()).WillByDefault(::testing::Return(true));
        ON_CALL(mockSteeringClient, turnLeft(::testing::_)).WillByDefault(::testing::Return(true));
        ON_CALL(mockSteeringClient, turnRight(::testing::_)).WillByDefault(::testing::Return(true));
        ON_CALL(mockSteeringClient, driveForward(::testing::_)).WillByDefault(::testing::Return(true));
        ON_CALL(mockSteeringClient, driveBackward(::testing::_)).WillByDefault(::testing::Return(true));
    }

    void TearDown() override {
    }
};

TEST_P(AutoPilotParameterizedTestFixture, testAllActions) {
    auto [frameName, frameNameDescription] = GetParam();
    cv::Mat frame = cv::imread(frameName);
    ConfigParser configParser(frameNameDescription);
    std::string correctAction = configParser.getValue<std::string>("ACTION", "");
    roadLaneDetectorCanny.processFrame(frame);
    autoPilot.controlSteering();
    std::string action = autoPilot.getCurrentAction();
    EXPECT_EQ(action, correctAction);
}

TEST_F(AutoPilotParameterizedTestFixture, testTest) {
    cv::Mat frame = cv::imread("../frames/frame113.jpg");
    roadLaneDetectorCanny.processFrame(frame);
    autoPilot.controlSteering();
    EXPECT_EQ(5, 3 + 2);
}

INSTANTIATE_TEST_SUITE_P(
    shouldCenterTests,
    AutoPilotParameterizedTestFixture,
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
    shouldTurnLeftTests,
    AutoPilotParameterizedTestFixture,
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

INSTANTIATE_TEST_SUITE_P(
    shouldMakeCorrectionRight,
    AutoPilotParameterizedTestFixture,
    ::testing::Values(
        std::make_tuple("../frames/frame57.jpg", "../frames/frame57_description.txt"),
        std::make_tuple("../frames/frame58.jpg", "../frames/frame58_description.txt"),
        std::make_tuple("../frames/frame59.jpg", "../frames/frame59_description.txt"),
        std::make_tuple("../frames/frame60.jpg", "../frames/frame60_description.txt"),
        std::make_tuple("../frames/frame61.jpg", "../frames/frame61_description.txt"),
        std::make_tuple("../frames/frame62.jpg", "../frames/frame62_description.txt"),
        std::make_tuple("../frames/frame63.jpg", "../frames/frame63_description.txt"),
        std::make_tuple("../frames/frame64.jpg", "../frames/frame64_description.txt"),
        std::make_tuple("../frames/frame65.jpg", "../frames/frame65_description.txt"),
        std::make_tuple("../frames/frame66.jpg", "../frames/frame66_description.txt"),
        std::make_tuple("../frames/frame67.jpg", "../frames/frame67_description.txt"),
        std::make_tuple("../frames/frame68.jpg", "../frames/frame68_description.txt"),
        std::make_tuple("../frames/frame69.jpg", "../frames/frame69_description.txt"),
        std::make_tuple("../frames/frame70.jpg", "../frames/frame70_description.txt"),
        std::make_tuple("../frames/frame71.jpg", "../frames/frame71_description.txt"),
        std::make_tuple("../frames/frame72.jpg", "../frames/frame72_description.txt"),
        std::make_tuple("../frames/frame73.jpg", "../frames/frame73_description.txt"),
        std::make_tuple("../frames/frame74.jpg", "../frames/frame74_description.txt"),
        std::make_tuple("../frames/frame75.jpg", "../frames/frame75_description.txt"),
        std::make_tuple("../frames/frame76.jpg", "../frames/frame76_description.txt"),
        std::make_tuple("../frames/frame77.jpg", "../frames/frame77_description.txt"),
        std::make_tuple("../frames/frame78.jpg", "../frames/frame78_description.txt")));

INSTANTIATE_TEST_SUITE_P(
    shouldMakeCorrectionLeft,
    AutoPilotParameterizedTestFixture,
    ::testing::Values(
        std::make_tuple("../frames/frame79.jpg", "../frames/frame79_description.txt"),
        std::make_tuple("../frames/frame80.jpg", "../frames/frame80_description.txt"),
        std::make_tuple("../frames/frame81.jpg", "../frames/frame81_description.txt"),
        std::make_tuple("../frames/frame82.jpg", "../frames/frame82_description.txt"),
        std::make_tuple("../frames/frame83.jpg", "../frames/frame83_description.txt"),
        std::make_tuple("../frames/frame84.jpg", "../frames/frame84_description.txt"),
        std::make_tuple("../frames/frame85.jpg", "../frames/frame85_description.txt"),
        std::make_tuple("../frames/frame86.jpg", "../frames/frame86_description.txt"),
        std::make_tuple("../frames/frame87.jpg", "../frames/frame87_description.txt"),
        std::make_tuple("../frames/frame88.jpg", "../frames/frame88_description.txt"),
        std::make_tuple("../frames/frame89.jpg", "../frames/frame89_description.txt"),
        std::make_tuple("../frames/frame90.jpg", "../frames/frame90_description.txt"),
        std::make_tuple("../frames/frame91.jpg", "../frames/frame91_description.txt"),
        std::make_tuple("../frames/frame92.jpg", "../frames/frame92_description.txt"),
        std::make_tuple("../frames/frame93.jpg", "../frames/frame93_description.txt"),
        std::make_tuple("../frames/frame94.jpg", "../frames/frame94_description.txt"),
        std::make_tuple("../frames/frame95.jpg", "../frames/frame95_description.txt"),
        std::make_tuple("../frames/frame96.jpg", "../frames/frame96_description.txt"),
        std::make_tuple("../frames/frame97.jpg", "../frames/frame97_description.txt"),
        std::make_tuple("../frames/frame98.jpg", "../frames/frame98_description.txt"),
        std::make_tuple("../frames/frame99.jpg", "../frames/frame99_description.txt"),
        std::make_tuple("../frames/frame100.jpg", "../frames/frame100_description.txt"),
        std::make_tuple("../frames/frame101.jpg", "../frames/frame101_description.txt"),
        std::make_tuple("../frames/frame102.jpg", "../frames/frame102_description.txt"),
        std::make_tuple("../frames/frame103.jpg", "../frames/frame103_description.txt"),
        std::make_tuple("../frames/frame104.jpg", "../frames/frame104_description.txt"),
        std::make_tuple("../frames/frame105.jpg", "../frames/frame105_description.txt"),
        std::make_tuple("../frames/frame106.jpg", "../frames/frame106_description.txt"),
        std::make_tuple("../frames/frame107.jpg", "../frames/frame107_description.txt")));

