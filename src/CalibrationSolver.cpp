#include "CalibrationSolver.h"

#include <opencv2/opencv.hpp>
#include <vector>

namespace
{
    struct ChessboardMeasurements
    {
        float SquareWidthMeters{ 0.005f };
        cv::Size BoardDimensionsSquares{ 6, 9 };
    };

    std::vector<cv::Point3f> getCornerPositions(const ChessboardMeasurements& measurements)
    {
        std::vector<cv::Point3f> corners{};
        corners.reserve(measurements.BoardDimensionsSquares.area());

        for (int j = 0; j < measurements.BoardDimensionsSquares.height; j++)
        {
            for (int i = 0; i < measurements.BoardDimensionsSquares.width; i++)
            {
                corners.emplace_back(i * measurements.SquareWidthMeters, j * measurements.SquareWidthMeters, 0.f);
            }
        }

        return std::move(corners);
    }
}

struct CalibrationSolver::Impl
{
    bool AddImage(size_t width, size_t height, void *data);
    CalibrationSolver::Calibration SolveForCalibration() const;

private:
    ChessboardMeasurements m_measurements{};
    std::vector<std::vector<cv::Point2f>> m_foundPoints{};
    cv::Size m_frameSize;
};

bool CalibrationSolver::Impl::AddImage(size_t width, size_t height, void *data)
{
    cv::Mat image{};
    cv::Mat(height, width, CV_8U, data).copyTo(image);

    m_foundPoints.emplace_back();
    auto& points = m_foundPoints.back();
    if (cv::findChessboardCorners(image, m_measurements.BoardDimensionsSquares, points))
    {
        m_frameSize = image.size();
        return true;
    }
    else
    {
        m_foundPoints.pop_back();
        return false;
    }
}

CalibrationSolver::Calibration CalibrationSolver::Impl::SolveForCalibration() const
{
    cv::Mat cameraMatrix{ cv::Mat::eye(3, 3, CV_64F) };
    cv::Mat distortionCoefficients{};

    std::vector<std::vector<cv::Point3f>> worldSpaceCornerPoints;
    worldSpaceCornerPoints.resize(m_foundPoints.size(), getCornerPositions(m_measurements));

    std::vector<cv::Mat> rVectors;
    std::vector<cv::Mat> tVectors;
    distortionCoefficients = cv::Mat::zeros(8, 1, CV_64F);

    Calibration calibration{};

    calibration.FrameWidth = m_frameSize.width;
    calibration.FrameHeight = m_frameSize.height;
    calibration.AverageReprojectionError = cv::calibrateCamera(worldSpaceCornerPoints, m_foundPoints, m_frameSize, cameraMatrix, distortionCoefficients, rVectors, tVectors);

    calibration.Fx = cameraMatrix.at<double>(0, 0);
    calibration.Fy = cameraMatrix.at<double>(1, 1);
    calibration.Cx = cameraMatrix.at<double>(0, 2);
    calibration.Cy = cameraMatrix.at<double>(1, 2);
    calibration.K1 = distortionCoefficients.at<double>(0, 0);
    calibration.K2 = distortionCoefficients.at<double>(1, 0);
    calibration.P1 = distortionCoefficients.at<double>(2, 0);
    calibration.P2 = distortionCoefficients.at<double>(3, 0);
    calibration.K3 = distortionCoefficients.at<double>(4, 0);

    return calibration;
}

CalibrationSolver::CalibrationSolver()
    : m_impl{ std::make_unique<Impl>() }
{}

CalibrationSolver::~CalibrationSolver()
{}

bool CalibrationSolver::AddImage(size_t width, size_t height, void *data)
{
    return m_impl->AddImage(width, height, data);
}

CalibrationSolver::Calibration CalibrationSolver::SolveForCalibration() const
{
    return m_impl->SolveForCalibration();
}