#include "steering/auto_pilot.h"

#include <gtest/gtest.h>

#include "common/config_parser.h"

class AutoPilotTest : public ::testing::Test {
   protected:
    Logger logger;
    DistanceClient distanceClient;
    SteeringClient steeringClient;
    FrameDispatcherClient frameDispatcherClient;
    RoadLaneDetectorCanny roadLaneDetectorCanny;
    AutoPilot autoPilot;

    AutoPilotTest() : logger("/dev/null"),
                      distanceClient(logger, "ws://127.0.0.1:8000/distance"),
                      steeringClient(logger, "ws://127.0.0.1:8000/control"),
                      frameDispatcherClient(logger, "ws://localhost:8000/frame_dispatcher"),
                      roadLaneDetectorCanny(frameDispatcherClient),
                      autoPilot(roadLaneDetectorCanny, steeringClient, distanceClient, logger) {}

    void SetUp() override {
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

