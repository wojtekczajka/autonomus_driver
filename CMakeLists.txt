cmake_minimum_required(VERSION 3.0)
project(autonomus_driver)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(spdlog REQUIRED)
find_package(CURL REQUIRED)

file(COPY "src/camera/camera_config.txt" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "src/steering_control/curl_config.txt" DESTINATION ${CMAKE_BINARY_DIR})

add_executable(autonomus_driver
    src/main.cpp
    src/common/logger.cpp
    src/common/config_parser.cpp
    src/camera/camera.cpp
    src/steering_control/steering_control.cpp
    src/road_line_detector/road_line_detector.cpp
)

add_executable(linesTest
    src/linesTest.cpp
)
target_link_libraries(linesTest ${OpenCV_LIBS})

target_include_directories(autonomus_driver PRIVATE
    include     # Include the "include" directory
    src         # Include the "src" directory (if needed)
)

target_link_libraries(autonomus_driver ${OpenCV_LIBS} spdlog::spdlog curl)
