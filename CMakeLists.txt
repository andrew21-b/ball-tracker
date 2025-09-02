cmake_minimum_required(VERSION 4.0.0)
project(BallTracker)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} include)

add_executable(ball_tracker 
    src/main.cpp
    src/tracker.cpp
    src/utils.cpp
)

target_link_libraries(ball_tracker ${OpenCV_LIBS})