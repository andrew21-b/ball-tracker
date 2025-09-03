# Ball Tracker (C++ / OpenCV)

A **real-time ball tracking system** in C++ using OpenCV.  
Designed as a stepping stone project toward **sports technology applications** (player/ball tracking, broadcast analysis, etc.).

## Features
- Real-time video capture (webcam or file)
- HSV-based color segmentation for ball detection
- Noise reduction with morphological operations
- Contour-based ball detection with bounding circle
- Trajectory visualization
- FPS counter

## Build
```bash
mkdir build
cd build
cmake ..
cd ..
cmake --build .\build\ --config Debug
```


## Run
```bash
.\build\Debug\ball_tracker.exe
```