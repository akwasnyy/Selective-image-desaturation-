#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Global variables for the trackbar position
int brightnessThreshold = 128;
Mat colorImage, displayImage;

void applyThreshold(int, void*) {
    colorImage.copyTo(displayImage);  // Start with a copy of the original image

    // Convert only the pixels below the brightness threshold to grayscale
    for (int i = 0; i < colorImage.rows; i++) {
        for (int j = 0; j < colorImage.cols; j++) {
            Vec3b pixel = colorImage.at<Vec3b>(i, j);
            int brightness = (pixel[0] + pixel[1] + pixel[2]) / 3;
            if (brightness <= brightnessThreshold) {
                uchar grayScaleValue = static_cast<uchar>(brightness);
                displayImage.at<Vec3b>(i, j) = Vec3b(grayScaleValue, grayScaleValue, grayScaleValue);
            }
        }
    }

    imshow("Processed Image", displayImage);
}

int main() {
    // Load an image from file
    colorImage = imread("test.jpg", IMREAD_COLOR);
    if (colorImage.empty()) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    // Display the original image
    namedWindow("Original Image", WINDOW_AUTOSIZE);
    imshow("Original Image", colorImage);

    // Setup display image for showing the changes
    displayImage = colorImage.clone();

    // Create a window and trackbar
    namedWindow("Processed Image", WINDOW_AUTOSIZE);
    createTrackbar("Brightness Threshold", "Processed Image", &brightnessThreshold, 255, applyThreshold);

    // Initial application of the threshold
    applyThreshold(brightnessThreshold, nullptr);

    // Wait for a keystroke in the window
    waitKey(0);
    return 0;
}
