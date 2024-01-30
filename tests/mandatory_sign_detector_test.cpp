#include "road_sign_detector/mandatory_sign_detector.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "common/config_parser.h"

class MandatorySignDetectorParameterizedTestFixture : public ::testing::TestWithParam<std::tuple<std::string, std::string>> {
   protected:
    MandatorySignDetector mandatorySignDetector;

    MandatorySignDetectorParameterizedTestFixture() : mandatorySignDetector() {}
};

TEST_P(MandatorySignDetectorParameterizedTestFixture, testMandatorySignDetector) {
    auto [frameName, frameNameDescription] = GetParam();
    cv::Mat frame = cv::imread(frameName);
    ConfigParser configParser(frameNameDescription);
    mandatorySignDetector.detectMandatorySign(frame);
    int signType = configParser.getValue<int>("MANDATORY_SIGN_TYPE", -1);
    EXPECT_TRUE(mandatorySignDetector.isSignDetected());
    EXPECT_EQ(signType, mandatorySignDetector.getSignType());
}

INSTANTIATE_TEST_SUITE_P(
    testMandatorySignDetector,
    MandatorySignDetectorParameterizedTestFixture,
    ::testing::Values(
        std::make_tuple("../frames/test_data21.jpg", "../frames/test_data21_description.txt"),
        std::make_tuple("../frames/test_data22.jpg", "../frames/test_data22_description.txt"),
        std::make_tuple("../frames/test_data23.jpg", "../frames/test_data23_description.txt"),
        std::make_tuple("../frames/test_data24.jpg", "../frames/test_data24_description.txt"),
        std::make_tuple("../frames/test_data25.jpg", "../frames/test_data25_description.txt"),
        std::make_tuple("../frames/test_data26.jpg", "../frames/test_data26_description.txt"),
        std::make_tuple("../frames/test_data27.jpg", "../frames/test_data27_description.txt"),
        std::make_tuple("../frames/test_data28.jpg", "../frames/test_data28_description.txt"),
        std::make_tuple("../frames/test_data29.jpg", "../frames/test_data29_description.txt"),
        std::make_tuple("../frames/test_data30.jpg", "../frames/test_data30_description.txt"),
        std::make_tuple("../frames/test_data31.jpg", "../frames/test_data31_description.txt"),
        std::make_tuple("../frames/test_data32.jpg", "../frames/test_data32_description.txt"),
        std::make_tuple("../frames/test_data33.jpg", "../frames/test_data33_description.txt"),
        std::make_tuple("../frames/test_data34.jpg", "../frames/test_data34_description.txt"),
        std::make_tuple("../frames/test_data35.jpg", "../frames/test_data35_description.txt"),
        std::make_tuple("../frames/test_data36.jpg", "../frames/test_data36_description.txt"),
        std::make_tuple("../frames/test_data37.jpg", "../frames/test_data37_description.txt"),
        std::make_tuple("../frames/test_data38.jpg", "../frames/test_data38_description.txt"),
        std::make_tuple("../frames/test_data39.jpg", "../frames/test_data39_description.txt"),
        std::make_tuple("../frames/test_data40.jpg", "../frames/test_data40_description.txt"),
        std::make_tuple("../frames/test_data41.jpg", "../frames/test_data41_description.txt"),
        std::make_tuple("../frames/test_data42.jpg", "../frames/test_data42_description.txt"),
        std::make_tuple("../frames/test_data43.jpg", "../frames/test_data43_description.txt"),
        std::make_tuple("../frames/test_data44.jpg", "../frames/test_data44_description.txt"),
        std::make_tuple("../frames/test_data45.jpg", "../frames/test_data45_description.txt"),
        std::make_tuple("../frames/test_data46.jpg", "../frames/test_data46_description.txt"),
        std::make_tuple("../frames/test_data47.jpg", "../frames/test_data47_description.txt"),
        std::make_tuple("../frames/test_data48.jpg", "../frames/test_data48_description.txt"),
        std::make_tuple("../frames/test_data49.jpg", "../frames/test_data49_description.txt"),
        std::make_tuple("../frames/test_data50.jpg", "../frames/test_data50_description.txt"),
        std::make_tuple("../frames/test_data51.jpg", "../frames/test_data51_description.txt"),
        std::make_tuple("../frames/test_data52.jpg", "../frames/test_data52_description.txt"),
        std::make_tuple("../frames/test_data53.jpg", "../frames/test_data53_description.txt"),
        std::make_tuple("../frames/test_data54.jpg", "../frames/test_data54_description.txt"),
        std::make_tuple("../frames/test_data55.jpg", "../frames/test_data55_description.txt"),
        std::make_tuple("../frames/test_data74.jpg", "../frames/test_data74_description.txt"),
        std::make_tuple("../frames/test_data75.jpg", "../frames/test_data75_description.txt"),
        std::make_tuple("../frames/test_data76.jpg", "../frames/test_data76_description.txt"),
        std::make_tuple("../frames/test_data77.jpg", "../frames/test_data77_description.txt"),
        std::make_tuple("../frames/test_data78.jpg", "../frames/test_data78_description.txt"),
        std::make_tuple("../frames/test_data79.jpg", "../frames/test_data79_description.txt"),
        std::make_tuple("../frames/test_data80.jpg", "../frames/test_data80_description.txt"),
        std::make_tuple("../frames/test_data81.jpg", "../frames/test_data81_description.txt"),
        std::make_tuple("../frames/test_data82.jpg", "../frames/test_data82_description.txt"),
        std::make_tuple("../frames/test_data83.jpg", "../frames/test_data83_description.txt"),
        std::make_tuple("../frames/test_data84.jpg", "../frames/test_data84_description.txt"),
        std::make_tuple("../frames/test_data85.jpg", "../frames/test_data85_description.txt"),
        std::make_tuple("../frames/test_data86.jpg", "../frames/test_data86_description.txt"),
        std::make_tuple("../frames/test_data87.jpg", "../frames/test_data87_description.txt"),
        std::make_tuple("../frames/test_data88.jpg", "../frames/test_data88_description.txt")));