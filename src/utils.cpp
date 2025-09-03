#include "utils.hpp"
#include <fstream>

void drawTextWithOutline(cv::Mat &frame, const std::string &text,
                         cv::Point org, double scale, int thickness)
{
    cv::putText(frame, text, org, cv::FONT_HERSHEY_SIMPLEX, scale,
                cv::Scalar(0, 0, 0), thickness + 2);
    cv::putText(frame, text, org, cv::FONT_HERSHEY_SIMPLEX, scale,
                cv::Scalar(0, 255, 0), thickness);
}

void drawFPS(cv::Mat &frame, double fps)
{
    std::string label = "FPS: " + std::to_string((int)fps);
    drawTextWithOutline(frame, label, cv::Point(10, 30), 1.0, 2);
}
