#pragma once

#include <array>
#include <memory>

class CalibrationSolver
{
public:
    // OpenCV calibration uses OpenCV naming conventions: https://docs.opencv.org/2.4/doc/tutorials/calib3d/camera_calibration/camera_calibration.html
    struct Calibration
    {
        int FrameWidth;
        int FrameHeight;

        double AverageReprojectionError;

        double Fx;
        double Fy;
        double Cx;
        double Cy;
        double K1;
        double K2;
        double P1;
        double P2;
        double K3;
    };

    CalibrationSolver();
    ~CalibrationSolver();

    bool AddImage(size_t width, size_t height, void *data);
    Calibration SolveForCalibration() const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl{};
};