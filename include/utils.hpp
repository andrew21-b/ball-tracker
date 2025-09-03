#pragma once
#include <opencv2/opencv.hpp>
#include <string>

void drawFPS(cv::Mat& frame, double fps);

void drawTextWithOutline(cv::Mat& frame, const std::string& text,
                         cv::Point org, double scale = 1.0, int thickness = 2);
