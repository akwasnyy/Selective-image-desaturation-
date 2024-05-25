#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void applyDesaturation(Mat& image, int mode, int brightnessThreshold, int rThreshold, int gThreshold, int bThreshold) {
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            Vec3b intensity = image.at<Vec3b>(y, x);
            int b = intensity[0];
            int g = intensity[1];
            int r = intensity[2];

            if (mode == 1) {  // RGB mode
                if (r <= rThreshold && g <= gThreshold && b <= bThreshold) {
                    int gray = (r + g + b) / 3;
                    image.at<Vec3b>(y, x) = Vec3b(gray, gray, gray);
                }
            }
            else {  // Brightness mode
                int brightness = (r + g + b) / 3;
                if (brightness <= brightnessThreshold) {
                    int gray = (r + g + b) / 3;
                    image.at<Vec3b>(y, x) = Vec3b(gray, gray, gray);
                }
            }
        }
    }
}

int main() {
    Mat inputImage = imread("test.jpg");
    if (inputImage.empty()) {
        cerr << "Failed to load image!" << endl;
        return -1;
    }

    namedWindow("Original Image", WINDOW_NORMAL);
    namedWindow("Partial Desaturation", WINDOW_NORMAL);
    namedWindow("Controls", WINDOW_NORMAL);
    resizeWindow("Original Image", 800, 600);
    resizeWindow("Partial Desaturation", 800, 600);
    resizeWindow("Controls", 800, 200);

    int modeRGB = 0;  // Button for RGB mode
    int modeBrightness = 0;  // Button for Brightness mode
    int brightnessThreshold = 128;
    int rThreshold = 128;
    int gThreshold = 128;
    int bThreshold = 128;

    // Control "Buttons"
    createTrackbar("RGB Mode", "Controls", &modeRGB, 1);
    createTrackbar("Brightness Mode", "Controls", &modeBrightness, 1);

    // Trackbars for RGB and Brightness
    createTrackbar("Brightness", "Controls", &brightnessThreshold, 255);
    createTrackbar("R", "Controls", &rThreshold, 255);
    createTrackbar("G", "Controls", &gThreshold, 255);
    createTrackbar("B", "Controls", &bThreshold, 255);

    Mat outputImage = inputImage.clone();

    while (true) {
        if (modeRGB == 1) {
            applyDesaturation(outputImage, 1, brightnessThreshold, rThreshold, gThreshold, bThreshold);
            setTrackbarPos("Brightness", "Controls", 0);  // Reset Brightness control
        }
        else if (modeBrightness == 1) {
            applyDesaturation(outputImage, 0, brightnessThreshold, 0, 0, 0);
            setTrackbarPos("R", "Controls", 0);  // Reset RGB controls
            setTrackbarPos("G", "Controls", 0);
            setTrackbarPos("B", "Controls", 0);
        }

        imshow("Original Image", inputImage);
        imshow("Partial Desaturation", outputImage);

        if (getWindowProperty("Original Image", WND_PROP_VISIBLE) == 0 ||
            getWindowProperty("Partial Desaturation", WND_PROP_VISIBLE) == 0 ||
            getWindowProperty("Controls", WND_PROP_VISIBLE) == 0) {
            break; // Exit loop if any window is closed
        }

        char key = waitKey(10);
        if (key == 's' || key == 'S') {
            imwrite("output.jpg", outputImage);
            cout << "Saved modified image to file: output.jpg" << endl;
        }
        else if (key == 27) { // ESC key
            break;
        }
    }

    destroyAllWindows();
    return 0;
}
