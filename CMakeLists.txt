cmake_minimum_required(VERSION 3.0)
project(autonomus_driver)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(spdlog REQUIRED)
find_package(CURL REQUIRED)

file(COPY "src/camera/camera_config.txt" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "src/steering/curl_config.txt" DESTINATION ${CMAKE_BINARY_DIR})

add_executable(autonomus_driver
    src/main.cpp
    src/common/logger.cpp
    src/common/config_parser.cpp
    src/camera/camera.cpp
    src/distance/distance_client.cpp
    src/steering/steering_client.cpp
    # src/road_lane_detector/road_lane_detector_contours.cpp
    src/road_lane_detector/road_lane_detector_canny.cpp
)

add_executable(hsvTest
    src/helpers/hsvTest.cpp
)

target_link_libraries(hsvTest ${OpenCV_LIBS})

target_link_libraries(autonomus_driver ${OpenCV_LIBS} CURL::libcurl)

target_include_directories(autonomus_driver PRIVATE
    include     
)
