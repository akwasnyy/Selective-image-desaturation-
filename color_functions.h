#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#ifndef COLOR_FUNCTIONS_H
#define COLOR_FUNCTIONS_H

cv::Vec3b RGB_TO_HSL(int& r, int& b, int& g, double& H, double& S, double& L);
void HSL_TO_RGB(double& R_, double& B_, double& G_, double& H, double& S, double& L);

#endif