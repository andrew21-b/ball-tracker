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
    BallTracker(DetectionMode mode);

    cv::Mat preprocessFrame(const cv::Mat& frame);
    cv::Point2f detectBall(const cv::Mat& processed, cv::Mat& outputFrame);

    void drawTrajectory(cv::Mat& frame, cv::Point2f center);
    double computeFPS(int64 startTick, int64 endTick);

    void setHSVThresholds(const cv::Scalar& lower, const cv::Scalar& upper)
    {
        hsvLower = lower;
        hsvUpper = upper;
    }

    void setCircularityThreshold(double thresh)
    {
        circularityThreshold = thresh;
    }

    std::vector<cv::Point2f> getTrajectory() const { return trajectory; }

private:
    DetectionMode mode;
    cv::Scalar hsvLower, hsvUpper;
    double circularityThreshold;

    std::vector<cv::Point2f> trajectory;

    cv::Point2f detectContours(const cv::Mat& mask, cv::Mat& outputFrame);
    cv::Point2f detectCircles(const cv::Mat &edges, cv::Mat &outputFrame);
};
