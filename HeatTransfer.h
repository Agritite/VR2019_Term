//VR2019 Term Project
#pragma once

#include <opencv2/opencv.hpp>

cv::Mat HeatTransfer(const cv::Mat&, const size_t n = 10, const float timestep = 0.1, const float alpha = 10);