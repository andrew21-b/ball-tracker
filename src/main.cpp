#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video stream!" << std::endl;
        return -1;
    }

    cv::Mat frame, flippedFrame;
    while (true)
    {
        int64_t startTime = cv::getTickCount();
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error: Could not read frame from video stream!" << std::endl;
            break;
        }
        cv::flip(frame, flippedFrame, 1);
        cv::imshow("Display Video", flippedFrame);
        if (cv::waitKey(10) == 27)
            break;
    }

    return 0;
}