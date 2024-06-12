#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#ifndef COLOR_FUNCTIONS_H
#define COLOR_FUNCTIONS_H

void RGB_TO_HSL(const double& R_, const double& G_, const double& B_, double& H, double& S, double& L);
void HSL_TO_RGB(double& R_, double& G_, double& B_, const double& H, const double& S, const double& L);

#endif