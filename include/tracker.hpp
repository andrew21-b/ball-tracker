#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

class BallTracker
{
public:
    BallTracker();

    cv::Mat preprocessFrame(const cv::Mat &frame);
    cv::Point2f detectBall(const cv::Mat &mask, cv::Mat &outputFrame);
    void drawTrajectory(cv::Mat &frame, cv::Point2f center);
    double computeFPS(int64 startTick, int64 endTick);

private:
    std::vector<cv::Point2f> trajectory;
};
