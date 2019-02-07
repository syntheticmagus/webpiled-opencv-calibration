#include <CalibrationSolver.h>

#include <memory>

extern "C"
{
    void initialize();
    void uninitialize();
    void reset();
    bool add_image(int width, int height, void* data);
    CalibrationSolver::Calibration* solve_for_calibration();
}

// Global calibration solver variable
std::unique_ptr<CalibrationSolver> calibrationSolver{};
std::unique_ptr<CalibrationSolver::Calibration> calibration{};

void initialize()
{
    calibrationSolver = std::make_unique<CalibrationSolver>();
}

void uninitialize()
{
    calibrationSolver.reset();
    calibration.reset();
}

void reset()
{
    uninitialize();
    initialize();
}

bool add_image(int width, int height, void* data)
{
    return calibrationSolver->AddImage(static_cast<size_t>(width), static_cast<size_t>(height), data);
}

CalibrationSolver::Calibration* solve_for_calibration()
{
    calibration = std::make_unique<CalibrationSolver::Calibration>(calibrationSolver->SolveForCalibration());
    return calibration.get();
}