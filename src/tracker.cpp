#include "tracker.hpp"

const int MIN_BALL_AREA = 500;

BallTracker::BallTracker(DetectionMode mode) : mode(mode)
{
    hsvLower = cv::Scalar(30, 50, 50);
    hsvUpper = cv::Scalar(80, 255, 255);
    circularityThreshold = 0.7;
}

cv::Mat BallTracker::preprocessFrame(const cv::Mat &frame)
{
    cv::Mat output;

    if (mode == DetectionMode::HSV)
    {
        cv::Mat hsv;
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
        cv::inRange(hsv, hsvLower, hsvUpper, output);

        cv::erode(output, output, cv::Mat(), cv::Point(-1, -1), 2);
        cv::dilate(output, output, cv::Mat(), cv::Point(-1, -1), 2);
    }
    else if (mode == DetectionMode::GRAYSCALE)
    {
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        cv::GaussianBlur(gray, gray, cv::Size(9, 9), 0);

        cv::Mat edges;
        cv::Canny(gray, edges, 8, 35);

        output = edges.clone();
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

    cv::Point2f bestCenter(-1, -1);
    float bestRadius = -1;
    double bestScore = 1e9;

    for (const auto &contour : contours)
    {
        double area = cv::contourArea(contour);
        if (area < MIN_BALL_AREA)
            continue;

        cv::Point2f center;
        float radius;
        cv::minEnclosingCircle(contour, center, radius);

        double perimeter = cv::arcLength(contour, true);
        if (perimeter == 0)
            continue;

        double circularity = 4 * CV_PI * area / (perimeter * perimeter);
        if (circularity < 0.5 * circularityThreshold)
            continue;

        cv::Mat maskImg = cv::Mat::zeros(mask.size(), CV_8U);
        std::vector<std::vector<cv::Point>> singleContour = {contour};
        cv::drawContours(maskImg, singleContour, 0, 255, -1);
        cv::Scalar meanVal = cv::mean(mask, maskImg);
        if (meanVal[0] > 200)
            continue;

        if (radius > bestRadius || meanVal[0] < bestScore)
        {
            bestRadius = radius;
            bestCenter = center;
            bestScore = meanVal[0];
        }
        cv::circle(outputFrame, center, (int)radius, cv::Scalar(0, 255, 0), 2);
        cv::circle(outputFrame, center, 2, cv::Scalar(0, 0, 255), -1);
    }

    return bestCenter;
}

cv::Point2f BallTracker::detectCircles(const cv::Mat &edges, cv::Mat &outputFrame)
{
    cv::Mat dilatedEdges;
    cv::dilate(edges, dilatedEdges, cv::Mat(), cv::Point(-1, -1), 4);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(dilatedEdges, circles, cv::HOUGH_GRADIENT, 1,
                     dilatedEdges.rows / 8,
                     80, 20,
                     60, 140);

    cv::Point2f center(-1, -1);
    float bestRadius = -1;

    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Vec3f c = circles[i];
        cv::Point2f candidateCenter(c[0], c[1]);
        float radius = c[2];
        if (radius < 60 || radius > 140)
            continue;
        if (radius > bestRadius)
        {
            bestRadius = radius;
            center = candidateCenter;
        }
    }

    if (center.x >= 0 && center.y >= 0 && bestRadius > 0)
    {
        cv::circle(outputFrame, center, (int)bestRadius, cv::Scalar(0, 255, 0), 2);
        cv::circle(outputFrame, center, 2, cv::Scalar(0, 0, 255), 3);
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