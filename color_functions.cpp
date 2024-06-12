#include "color_functions.h"
using namespace cv;

Vec3b RGB_TO_HSL(const double& R_, const double& G_, const double& B_)   {
    double max_rgb = std::max({ R_, G_, B_ });
    double min_rgb = std::min({ R_, G_, B_ });
    double L = (max_rgb + min_rgb) / 2.0;

    double delta = max_rgb - min_rgb;
    double S = 0;

    if (L <= 0.5)
    {
        S = delta / (max_rgb + min_rgb);
    }
    else
    {
        S = delta / (2.0 - max_rgb - min_rgb);
    }
    double H = 0;
    if (delta != 0)
    {
        if (max_rgb == R_)
            H = (G_ - B_) / delta;
        else if (max_rgb == G_)
            H = 2.0 + (B_ - R_) / delta;
        else
            H = 4.0 + (R_ - G_) / delta;
    }
    H = H * 60;
    if (H < 0)
        H += 360;

    return Vec3b(H, S, L);
}


void HSL_TO_RGB(double& R_, double& G_, double& B_, const double& H, const double& S, const double& L) {
    double q = 0;
    double p = 0; //jakies fancy parameters do hsl->rgb conversion idk
    if (L <= 0.5)
        q = L * (1 + S);
    else
        q = L + S - L * S;

    p = 2 * L - q;

    auto h = [](double t, double p_, double q_) {
        if (t < 0) t += 1;
        if (t > 1) t -= 1;
        if (6 * t < 1)
            return p_ + (q_ - p_) * 6 * t;
        if (2 * t < 1)
            return q_;
        if (3 * t < 2)
            return p_ + (q_ - p_) * (2 / 3.0 - t) * 6;
        return p_;
        };//nazwalem se t¹ lambde h bo mi tak pasowalo ~_~
    //tu obliczamy nowe rgb:
    R_ = h(H / 360.0 + 1 / 3.0, p, q);
    G_ = h(H / 360.0, p, q);
    B_ = h(H / 360.0 - 1 / 3.0, p, q);
    R_ = R_ * 255.0;
    G_ = G_ * 255.0;
    B_ = B_ * 255.0; //przeskalowanie, bo wczesniej normalizowalismy 

}

