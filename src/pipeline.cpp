#include "rmcs_laser_guidance/pipeline.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include <opencv2/imgproc.hpp>

namespace rmcs_laser_guidance {
namespace {

constexpr double kMinDetectedBrightness = 250.0;

auto to_gray(const cv::Mat& image) -> cv::Mat {
    cv::Mat gray;
    if (image.empty())
        return gray;

    if (image.channels() == 1) {
        gray = image.clone();
    } else if (image.channels() == 3) {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    } else if (image.channels() == 4) {
        cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
    }

    return gray;
}

auto contour_center(const std::vector<cv::Point>& contour) -> cv::Point2f {
    const cv::Moments moments = cv::moments(contour);
    if (std::abs(moments.m00) > 1e-6)
        return {
            static_cast<float>(moments.m10 / moments.m00),
            static_cast<float>(moments.m01 / moments.m00),
        };

    const cv::Rect bounds = cv::boundingRect(contour);
    return {
        static_cast<float>(bounds.x) + static_cast<float>(bounds.width) / 2.0F,
        static_cast<float>(bounds.y) + static_cast<float>(bounds.height) / 2.0F,
    };
}

} // namespace

Pipeline::Pipeline(const Config& config)
    : debug_(config.debug) {}

auto Pipeline::process(const Frame& frame) const -> TargetObservation {
    if (frame.image.empty())
        return {};

    cv::Mat gray = to_gray(frame.image);
    if (gray.empty())
        return {};

    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, {5, 5}, 0.0);

    double max_value = 0.0;
    cv::Point max_location;
    cv::minMaxLoc(blurred, nullptr, &max_value, nullptr, &max_location);
    if (max_value < kMinDetectedBrightness)
        return {};

    cv::Mat mask;
    cv::threshold(blurred, mask, std::max(245.0, max_value - 10.0), 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    if (contours.empty())
        return {};

    const auto largest = std::max_element(
        contours.begin(), contours.end(), [](const auto& lhs, const auto& rhs) {
            return cv::contourArea(lhs) < cv::contourArea(rhs);
        });

    TargetObservation observation;
    observation.detected = true;
    observation.center = contour_center(*largest);
    observation.brightness = static_cast<float>(max_value);
    observation.contour.reserve(largest->size());
    for (const cv::Point& point : *largest)
        observation.contour.emplace_back(static_cast<float>(point.x), static_cast<float>(point.y));

    (void)max_location;
    return observation;
}

auto Pipeline::draw_debug_overlay(cv::Mat& image, const TargetObservation& observation) const -> void {
    if (image.empty() || !debug_.draw_overlay)
        return;

    if (!observation.detected) {
        cv::putText(
            image, "no target", {10, 30}, cv::FONT_HERSHEY_SIMPLEX, 0.8, {0, 255, 255}, 2);
        return;
    }

    if (!observation.contour.empty()) {
        std::vector<std::vector<cv::Point>> contour_set(1);
        contour_set.front().reserve(observation.contour.size());
        for (const cv::Point2f& point : observation.contour) {
            contour_set.front().emplace_back(
                static_cast<int>(std::lround(point.x)), static_cast<int>(std::lround(point.y)));
        }
        cv::polylines(image, contour_set, true, {0, 255, 0}, 2);
    }

    cv::circle(image, observation.center, 6, {0, 0, 255}, 2);
    cv::putText(
        image,
        "laser " + std::to_string(static_cast<int>(observation.brightness)),
        {
            static_cast<int>(std::lround(observation.center.x)) + 10,
            static_cast<int>(std::lround(observation.center.y)) - 10,
        },
        cv::FONT_HERSHEY_SIMPLEX,
        0.6,
        {0, 255, 0},
        2);
}

} // namespace rmcs_laser_guidance

