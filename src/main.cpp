#include "tracker.hpp"
#include "utils.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

int lowH = 30, highH = 80;
int lowS = 50, highS = 255;
int lowV = 50, highV = 255;

int circularityThresh = 70;

cv::Mat makeDebugView(const cv::Mat& raw, const cv::Mat& processed, 
                      const cv::Mat& edges, const cv::Mat& trackerFrame) {
    cv::Mat rawResized, procResized, edgeResized, trackResized;
    cv::resize(raw, rawResized, cv::Size(320,240));
    cv::resize(processed, procResized, cv::Size(320,240));
    cv::resize(edges, edgeResized, cv::Size(320,240));
    cv::resize(trackerFrame, trackResized, cv::Size(320,240));

    if (procResized.channels() == 1) cv::cvtColor(procResized, procResized, cv::COLOR_GRAY2BGR);
    if (edgeResized.channels() == 1) cv::cvtColor(edgeResized, edgeResized, cv::COLOR_GRAY2BGR);

    cv::Mat top, bottom, combined;
    cv::hconcat(rawResized, procResized, top);
    cv::hconcat(edgeResized, trackResized, bottom);
    cv::vconcat(top, bottom, combined);

    return combined;
}

int main()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video stream!" << std::endl;
        return -1;
    }

    DetectionMode mode = DetectionMode::GRAYSCALE;

    BallTracker tracker(mode);

    if (mode == DetectionMode::HSV)
    {
        cv::namedWindow("Tuning");
        cv::createTrackbar("LowH", "Tuning", &lowH, 179);
        cv::createTrackbar("HighH", "Tuning", &highH, 179);
        cv::createTrackbar("LowS", "Tuning", &lowS, 255);
        cv::createTrackbar("HighS", "Tuning", &highS, 255);
        cv::createTrackbar("LowV", "Tuning", &lowV, 255);
        cv::createTrackbar("HighV", "Tuning", &highV, 255);
    }
    else
    {
        cv::namedWindow("Tuning");
        cv::createTrackbar("Circularity %", "Tuning", &circularityThresh, 100);
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

        cv::Mat processed = tracker.preprocessFrame(flippedFrame);

        cv::Mat edges;
        cv::Canny(processed, edges, 100, 200);

        if (mode == DetectionMode::HSV)
        {
            tracker.setHSVThresholds(cv::Scalar(lowH, lowS, lowV),
                                     cv::Scalar(highH, highS, highV));
        }
        else
        {
            tracker.setCircularityThreshold(circularityThresh / 100.0);
        }

        cv::Mat trackerFrame = flippedFrame.clone();

        cv::Point2f center = tracker.detectBall(processed, flippedFrame);

        if (center.x > 0 && center.y > 0)
        {
            tracker.drawTrajectory(flippedFrame, center);
            std::cout << "Ball detected at: (" << center.x << ", " << center.y << ")" << std::endl;
        }

        int64_t endTime = cv::getTickCount();
        double fps = tracker.computeFPS(startTime, endTime);
        drawFPS(flippedFrame, fps);

        cv::Mat debugView = makeDebugView(flippedFrame, processed, edges, trackerFrame);
        cv::imshow("Debug View (Raw | Processed | Edges | Tracker)", debugView);

        int key = cv::waitKey(10);
        if (key == 27)
            break;
    }

    return 0;
}