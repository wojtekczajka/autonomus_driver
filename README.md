# autonomus_driver #

## Overview ##
The Autonomous Driver project is a computer vision-based program developed in C++ using OpenCV. The program processes input from a camera and distance sensor to make steering decisions, enabling autonomous steering. It incorporates functionalities to recognize road signs indicating mandatory turns and speed limits. Additionally, it can detect road lines and react to obstacles on the road. Project is deployed on Pi-Racer Standard platform.

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

## Demo Videos ##
1. Steering


https://github.com/wojtekczajka/autonomus_driver/assets/78093105/8c0d245e-23c6-42f3-b277-46252ec9a420


2. Road signs and traffic light recognition, f.e can be seen that the algorithm does not classify false positives.


https://github.com/wojtekczajka/autonomus_driver/assets/78093105/623deb28-1bd5-4662-a3f7-c3a1855e0029


3. Example ride (the car starts driving only when there is no obstacle ahead, reacts to traffic lights at the end of ride).


https://github.com/wojtekczajka/autonomus_driver/assets/78093105/8f32fe3f-7633-4a8b-a020-4011214e1507


## Note for Raspberry Pi 4 ##
This program is specifically designed for the Raspberry Pi 4, inspired by the Donkey Car project. Make sure your Raspberry Pi environment is set up accordingly.
