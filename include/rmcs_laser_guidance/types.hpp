#pragma once

#include <chrono>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

namespace rmcs_laser_guidance {

using Clock = std::chrono::steady_clock;

struct Frame {
    cv::Mat image;
    Clock::time_point timestamp{};
};

struct TargetObservation {
    bool detected = false;
    cv::Point2f center{-1.0F, -1.0F};
    std::vector<cv::Point2f> contour{};
    float brightness = 0.0F;
};

} // namespace rmcs_laser_guidance

