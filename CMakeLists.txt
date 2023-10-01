cmake_minimum_required(VERSION 3.0)
project(autonomus_driver)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(spdlog REQUIRED)
find_package(CURL REQUIRED)

file(COPY "src/camera/camera_config.txt" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "src/steering_control/curl_config.txt" DESTINATION ${CMAKE_BINARY_DIR})

add_executable(main
    src/main.cpp
    src/common/logger.cpp
    src/common/config_parser.cpp
    src/camera/camera.cpp
    src/steering_control/steering_control.cpp
)

target_include_directories(main PRIVATE
    include     # Include the "include" directory
    src         # Include the "src" directory (if needed)
)

target_link_libraries(main ${OpenCV_LIBS} spdlog::spdlog curl)
