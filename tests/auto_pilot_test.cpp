#include "steering/auto_pilot.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

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

class MockFrameDispatcherClient : public IFrameDispatcherClient {
   public:
    MOCK_METHOD(void, sendFrame, (const cv::Mat&, const std::string&), (override));
};

class AutoPilotTest : public ::testing::Test {
   protected:
    Logger logger;
    MockDistanceClient mockDistanceClient;
    MockSteeringClient mockSteeringClient;
    MockFrameDispatcherClient mockFrameDispatcherClient;
    RoadLaneDetectorCanny roadLaneDetectorCanny;
    AutoPilot autoPilot;

    AutoPilotTest() : logger("/dev/null"),
                      mockDistanceClient(MockDistanceClient()),
                      mockSteeringClient(),
                      mockFrameDispatcherClient(),
                      roadLaneDetectorCanny(mockFrameDispatcherClient),
                      autoPilot(roadLaneDetectorCanny, mockSteeringClient, mockDistanceClient, logger) {}

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

    void runAutoPilotTest(const std::string& frameName, const std::string& frameNameDescription) {
        cv::Mat frame = cv::imread(frameName);
        ConfigParser configParser(frameNameDescription);
        std::string correctAction = configParser.getValue<std::string>("ACTION", "");
        roadLaneDetectorCanny.processFrame(frame);
        autoPilot.controlSteering();
        std::string action = autoPilot.getCurrentAction();
        EXPECT_EQ(action, correctAction);
    }
};

TEST_F(AutoPilotTest, ShouldCenter0) {
    runAutoPilotTest("../frames/frame0.jpg", "../frames/frame0_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter1) {
    runAutoPilotTest("../frames/frame1.jpg", "../frames/frame1_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter2) {
    runAutoPilotTest("../frames/frame2.jpg", "../frames/frame2_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter3) {
    runAutoPilotTest("../frames/frame3.jpg", "../frames/frame3_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter4) {
    runAutoPilotTest("../frames/frame4.jpg", "../frames/frame4_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter5) {
    runAutoPilotTest("../frames/frame5.jpg", "../frames/frame5_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter6) {
    runAutoPilotTest("../frames/frame6.jpg", "../frames/frame6_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter7) {
    runAutoPilotTest("../frames/frame7.jpg", "../frames/frame7_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter8) {
    runAutoPilotTest("../frames/frame8.jpg", "../frames/frame8_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter9) {
    runAutoPilotTest("../frames/frame9.jpg", "../frames/frame9_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter10) {
    runAutoPilotTest("../frames/frame10.jpg", "../frames/frame10_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter11) {
    runAutoPilotTest("../frames/frame11.jpg", "../frames/frame11_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter12) {
    runAutoPilotTest("../frames/frame12.jpg", "../frames/frame12_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter13) {
    runAutoPilotTest("../frames/frame13.jpg", "../frames/frame13_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter14) {
    runAutoPilotTest("../frames/frame14.jpg", "../frames/frame14_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter15) {
    runAutoPilotTest("../frames/frame15.jpg", "../frames/frame15_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter16) {
    runAutoPilotTest("../frames/frame16.jpg", "../frames/frame16_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter17) {
    runAutoPilotTest("../frames/frame17.jpg", "../frames/frame17_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter18) {
    runAutoPilotTest("../frames/frame18.jpg", "../frames/frame18_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter19) {
    runAutoPilotTest("../frames/frame19.jpg", "../frames/frame19_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter20) {
    runAutoPilotTest("../frames/frame20.jpg", "../frames/frame20_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter21) {
    runAutoPilotTest("../frames/frame21.jpg", "../frames/frame21_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter22) {
    runAutoPilotTest("../frames/frame22.jpg", "../frames/frame22_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter23) {
    runAutoPilotTest("../frames/frame23.jpg", "../frames/frame23_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter24) {
    runAutoPilotTest("../frames/frame24.jpg", "../frames/frame24_description.txt");
}

TEST_F(AutoPilotTest, ShouldCenter25) {
    runAutoPilotTest("../frames/frame25.jpg", "../frames/frame25_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft0) {
    runAutoPilotTest("../frames/frame26.jpg", "../frames/frame26_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft1) {
    runAutoPilotTest("../frames/frame27.jpg", "../frames/frame27_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft2) {
    runAutoPilotTest("../frames/frame28.jpg", "../frames/frame28_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft3) {
    runAutoPilotTest("../frames/frame29.jpg", "../frames/frame29_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft4) {
    runAutoPilotTest("../frames/frame30.jpg", "../frames/frame30_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft5) {
    runAutoPilotTest("../frames/frame31.jpg", "../frames/frame31_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft6) {
    runAutoPilotTest("../frames/frame32.jpg", "../frames/frame32_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft7) {
    runAutoPilotTest("../frames/frame33.jpg", "../frames/frame33_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft8) {
    runAutoPilotTest("../frames/frame34.jpg", "../frames/frame34_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft9) {
    runAutoPilotTest("../frames/frame35.jpg", "../frames/frame35_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft10) {
    runAutoPilotTest("../frames/frame36.jpg", "../frames/frame36_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft11) {
    runAutoPilotTest("../frames/frame37.jpg", "../frames/frame37_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft12) {
    runAutoPilotTest("../frames/frame38.jpg", "../frames/frame38_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft13) {
    runAutoPilotTest("../frames/frame39.jpg", "../frames/frame39_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft14) {
    runAutoPilotTest("../frames/frame40.jpg", "../frames/frame40_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft15) {
    runAutoPilotTest("../frames/frame41.jpg", "../frames/frame41_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft16) {
    runAutoPilotTest("../frames/frame42.jpg", "../frames/frame42_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft17) {
    runAutoPilotTest("../frames/frame43.jpg", "../frames/frame43_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft18) {
    runAutoPilotTest("../frames/frame44.jpg", "../frames/frame44_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft19) {
    runAutoPilotTest("../frames/frame45.jpg", "../frames/frame45_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft20) {
    runAutoPilotTest("../frames/frame46.jpg", "../frames/frame46_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft21) {
    runAutoPilotTest("../frames/frame47.jpg", "../frames/frame47_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft22) {
    runAutoPilotTest("../frames/frame48.jpg", "../frames/frame48_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft23) {
    runAutoPilotTest("../frames/frame49.jpg", "../frames/frame49_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft24) {
    runAutoPilotTest("../frames/frame50.jpg", "../frames/frame50_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft25) {
    runAutoPilotTest("../frames/frame51.jpg", "../frames/frame51_description.txt");
}

TEST_F(AutoPilotTest, ShouldTurnLeft26) {
    runAutoPilotTest("../frames/frame52.jpg", "../frames/frame52_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight0) {
    runAutoPilotTest("../frames/frame57.jpg", "../frames/frame57_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight1) {
    runAutoPilotTest("../frames/frame58.jpg", "../frames/frame58_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight2) {
    runAutoPilotTest("../frames/frame59.jpg", "../frames/frame59_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight3) {
    runAutoPilotTest("../frames/frame60.jpg", "../frames/frame60_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight4) {
    runAutoPilotTest("../frames/frame61.jpg", "../frames/frame61_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight5) {
    runAutoPilotTest("../frames/frame62.jpg", "../frames/frame62_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight6) {
    runAutoPilotTest("../frames/frame63.jpg", "../frames/frame63_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight7) {
    runAutoPilotTest("../frames/frame64.jpg", "../frames/frame64_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight8) {
    runAutoPilotTest("../frames/frame65.jpg", "../frames/frame65_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight9) {
    runAutoPilotTest("../frames/frame66.jpg", "../frames/frame66_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight10) {
    runAutoPilotTest("../frames/frame67.jpg", "../frames/frame67_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight11) {
    runAutoPilotTest("../frames/frame68.jpg", "../frames/frame68_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight12) {
    runAutoPilotTest("../frames/frame69.jpg", "../frames/frame69_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight13) {
    runAutoPilotTest("../frames/frame70.jpg", "../frames/frame70_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight14) {
    runAutoPilotTest("../frames/frame71.jpg", "../frames/frame71_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight15) {
    runAutoPilotTest("../frames/frame72.jpg", "../frames/frame72_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight16) {
    runAutoPilotTest("../frames/frame73.jpg", "../frames/frame73_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight17) {
    runAutoPilotTest("../frames/frame74.jpg", "../frames/frame74_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight18) {
    runAutoPilotTest("../frames/frame75.jpg", "../frames/frame75_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight19) {
    runAutoPilotTest("../frames/frame76.jpg", "../frames/frame76_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight20) {
    runAutoPilotTest("../frames/frame77.jpg", "../frames/frame77_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionRight21) {
    runAutoPilotTest("../frames/frame78.jpg", "../frames/frame78_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft0) {
    runAutoPilotTest("../frames/frame79.jpg", "../frames/frame79_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft1) {
    runAutoPilotTest("../frames/frame80.jpg", "../frames/frame80_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft2) {
    runAutoPilotTest("../frames/frame81.jpg", "../frames/frame81_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft3) {
    runAutoPilotTest("../frames/frame82.jpg", "../frames/frame82_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft4) {
    runAutoPilotTest("../frames/frame83.jpg", "../frames/frame83_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft5) {
    runAutoPilotTest("../frames/frame84.jpg", "../frames/frame84_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft6) {
    runAutoPilotTest("../frames/frame85.jpg", "../frames/frame85_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft7) {
    runAutoPilotTest("../frames/frame86.jpg", "../frames/frame86_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft8) {
    runAutoPilotTest("../frames/frame87.jpg", "../frames/frame87_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft9) {
    runAutoPilotTest("../frames/frame88.jpg", "../frames/frame88_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft10) {
    runAutoPilotTest("../frames/frame89.jpg", "../frames/frame89_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft11) {
    runAutoPilotTest("../frames/frame90.jpg", "../frames/frame90_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft12) {
    runAutoPilotTest("../frames/frame91.jpg", "../frames/frame91_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft13) {
    runAutoPilotTest("../frames/frame92.jpg", "../frames/frame92_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft14) {
    runAutoPilotTest("../frames/frame93.jpg", "../frames/frame93_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft15) {
    runAutoPilotTest("../frames/frame94.jpg", "../frames/frame94_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft16) {
    runAutoPilotTest("../frames/frame95.jpg", "../frames/frame95_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft17) {
    runAutoPilotTest("../frames/frame96.jpg", "../frames/frame96_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft18) {
    runAutoPilotTest("../frames/frame97.jpg", "../frames/frame97_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft19) {
    runAutoPilotTest("../frames/frame98.jpg", "../frames/frame98_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft20) {
    runAutoPilotTest("../frames/frame99.jpg", "../frames/frame99_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft21) {
    runAutoPilotTest("../frames/frame100.jpg", "../frames/frame100_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft22) {
    runAutoPilotTest("../frames/frame101.jpg", "../frames/frame101_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft23) {
    runAutoPilotTest("../frames/frame102.jpg", "../frames/frame102_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft24) {
    runAutoPilotTest("../frames/frame103.jpg", "../frames/frame103_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft25) {
    runAutoPilotTest("../frames/frame104.jpg", "../frames/frame104_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft26) {
    runAutoPilotTest("../frames/frame105.jpg", "../frames/frame105_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft27) {
    runAutoPilotTest("../frames/frame106.jpg", "../frames/frame106_description.txt");
}

TEST_F(AutoPilotTest, ShouldMakeCorrectionLeft28) {
    runAutoPilotTest("../frames/frame107.jpg", "../frames/frame107_description.txt");
}