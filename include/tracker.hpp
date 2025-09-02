#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

enum class DetectionMode
{
    HSV,
    GRAYSCALE
};

class BallTracker
{
public:
    BallTracker(DetectionMode mode = DetectionMode::HSV);

    cv::Mat preprocessFrame(const cv::Mat &frame);
    cv::Point2f detectBall(const cv::Mat &mask, cv::Mat &outputFrame);
    void drawTrajectory(cv::Mat &frame, cv::Point2f center);
    double computeFPS(int64 startTick, int64 endTick);

private:
    DetectionMode mode;
    std::vector<cv::Point2f> trajectory;

    cv::Point2f detectContours(const cv::Mat& mask, cv::Mat& outputFrame);
    cv::Point2f detectCircles(const cv::Mat& gray, cv::Mat& outputFrame);
};
