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
    src/common/logger.cpp
    src/common/config_parser.cpp
    src/camera/camera.cpp
    src/camera/frame_dispatcher_client.cpp
    src/distance/distance_client.cpp
    src/steering/steering_client.cpp
    src/steering/auto_pilot.cpp
    src/easywsclient/easywsclient.cpp
    src/road_lane_detector/road_lane_detector_canny.cpp
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

target_link_libraries(autonomus_driver ${OpenCV_LIBS})

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

enable_testing()

add_executable(
    auto_pilot_test
    tests/road_lane_detector_canny_test.cpp
    tests/auto_pilot_test.cpp
    src/steering/auto_pilot.cpp
    src/common/config_parser.cpp
    src/common/logger.cpp
    src/camera/frame_dispatcher_client.cpp
    src/distance/distance_client.cpp
    src/steering/steering_client.cpp
    src/easywsclient/easywsclient.cpp
    src/road_lane_detector/road_lane_detector_canny.cpp
)
target_link_libraries(
    auto_pilot_test
    GTest::gtest_main
    GTest::gmock_main
    ${OpenCV_LIBS}
)

target_include_directories(auto_pilot_test PRIVATE
    include     
)

include(GoogleTest)
gtest_discover_tests(auto_pilot_test)

