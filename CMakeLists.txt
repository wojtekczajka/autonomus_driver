cmake_minimum_required(VERSION 3.10)
project(autonomus_driver)


set(CMAKE_CXX_STANDARD 17)
message("C++ Standard: ${CMAKE_CXX_STANDARD}")

find_package(OpenCV REQUIRED)
find_package(spdlog REQUIRED)

file(COPY "src/camera/camera_config.txt" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "src/steering/curl_config.txt" DESTINATION ${CMAKE_BINARY_DIR})

add_executable(autonomus_driver
    src/main.cpp
    src/common/drawer.cpp
    src/common/config_parser.cpp
    src/common/logger.cpp
    src/camera/camera.cpp
    src/distance/distance_client.cpp
    src/steering/steering_client.cpp
    src/steering/auto_pilot.cpp
    src/road_lane_detector/road_lane_detector_canny.cpp
    src/road_sign_detector/speed_limit_detector.cpp
    src/road_sign_detector/turn_sign_detector.cpp
)

add_executable(hsvTest
    src/helpers/hsvTest.cpp
)

add_executable(photographer
    src/helpers/photographer.cpp
    src/camera/camera.cpp
    src/common/logger.cpp
    src/common/config_parser.cpp
)

target_link_libraries(photographer ${OpenCV_LIBS})

target_link_libraries(hsvTest ${OpenCV_LIBS})

target_link_libraries(autonomus_driver ${OpenCV_LIBS} cpr::cpr -llept -ltesseract)

target_include_directories(autonomus_driver PRIVATE
    include     
)

target_include_directories(photographer PRIVATE
    include     
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8) # The commit hash for 1.10.x. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

enable_testing()

add_executable(
    auto_pilot_test
    tests/road_lane_detector_canny_test.cpp
    tests/auto_pilot_test.cpp
    src/steering/auto_pilot.cpp
    src/common/config_parser.cpp
    src/common/logger.cpp
    src/distance/distance_client.cpp
    src/steering/steering_client.cpp
    src/road_lane_detector/road_lane_detector_canny.cpp
)

target_link_libraries(
    auto_pilot_test
    GTest::gtest_main
    GTest::gmock_main
    cpr::cpr
    ${OpenCV_LIBS}
)

target_include_directories(auto_pilot_test PRIVATE
    include     
)

include(GoogleTest)
gtest_discover_tests(auto_pilot_test)

