# autonomus_driver #

## Overview ##
The Autonomous Driver project is a computer vision-based program developed in C++ using OpenCV. The program processes input from a camera and distance sensor to make steering decisions, enabling autonomous navigation.

## Build Instructions ##
To build the project, follow these steps:
1. Clone reposiotory (https://github.com/wojtekczajka/autonomus_driver)
2. Run CMake to generate the build files:
```bash
cmake -S . -B build
```
3. Change into the build directory:
```bash
cd build
```
4. Build the project using make. You can specify the number of parallel jobs with the -j option (replace 4 with the desired number):
```bash
make -j 4
```

## Running the Program ##
After successfully building the project, execute the main program using the following command:
```bash
./autonomous_driver
```
This will launch the autonomous driver program

## Running Tests ##
To run tests, use the following command in the build directory:
```bash
ctest
```
This will execute the tests and provide feedback on the program's functionality.

## Demo Video ##
[Download Demo Video](./videos/raw_make_u_turn_left_and_brake_to_obstacle_2.avi)

## Note for Raspberry Pi 4 ##
This program is specifically designed for the Raspberry Pi 4, inspired by the Donkey Car project. Make sure your Raspberry Pi environment is set up accordingly.