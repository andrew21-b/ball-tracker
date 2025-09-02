#include "tracker.hpp"

const cv::Scalar BALL_LOWER_HSV(30, 50, 50);
const cv::Scalar BALL_UPPER_HSV(80, 255, 255);
const int MIN_BALL_AREA = 500;

BallTracker::BallTracker(DetectionMode mode) : mode(mode) {}

cv::Mat BallTracker::preprocessFrame(const cv::Mat &frame)
{
    cv::Mat output;

    if (mode == DetectionMode::HSV)
    {
        cv::Mat hsv;
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
        cv::inRange(hsv, BALL_LOWER_HSV, BALL_UPPER_HSV, output);

        cv::erode(output, output, cv::Mat(), cv::Point(-1, -1), 2);
        cv::dilate(output, output, cv::Mat(), cv::Point(-1, -1), 2);
    }
    else if (mode == DetectionMode::GRAYSCALE)
    {
        cv::cvtColor(frame, output, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(output, output, cv::Size(9, 9), 2, 2);
    }
    return output;
}

cv::Point2f BallTracker::detectBall(const cv::Mat &processed, cv::Mat &outputFrame)
{
    if (mode == DetectionMode::HSV)
    {
        return detectContours(processed, outputFrame);
    }
    else
    {
        return detectCircles(processed, outputFrame);
    }
}

cv::Point2f BallTracker::detectContours(const cv::Mat &mask, cv::Mat &outputFrame)
{
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Point2f center(-1, -1);
    float radius = 0;

    for (const auto &contour : contours)
    {
        float area = cv::contourArea(contour);
        if (area > MIN_BALL_AREA)
        {
            cv::minEnclosingCircle(contour, center, radius);
            if (radius > 5)
            {
                cv::circle(outputFrame, center, (int)radius, cv::Scalar(0, 0, 255), 2);
                cv::circle(outputFrame, center, 2, cv::Scalar(255, 0, 0), -1);
            }
        }
    }
    return center;
}

cv::Point2f BallTracker::detectCircles(const cv::Mat &gray, cv::Mat &outputFrame)
{
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1,
                     gray.rows / 8,
                     100, 30,
                     10, 100);

    cv::Point2f center(-1, -1);

    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Vec3i c = circles[i];
        center = cv::Point2f(c[0], c[1]);
        int radius = c[2];
        cv::circle(outputFrame, center, radius, cv::Scalar(0, 0, 255), 2);
        cv::circle(outputFrame, center, 2, cv::Scalar(255, 0, 0), 3);
    }
    return center;
}

void BallTracker::drawTrajectory(cv::Mat &frame, cv::Point2f center)
{
    trajectory.push_back(center);
    for (size_t i = 1; i < trajectory.size(); i++)
    {
        cv::line(frame, trajectory[i - 1], trajectory[i],
                 cv::Scalar(255, 255, 0), 2);
    }
}

double BallTracker::computeFPS(int64 startTick, int64 endTick)
{
    double freq = cv::getTickFrequency();
    return freq / (endTick - startTick);
}