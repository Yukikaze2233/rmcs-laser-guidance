#include "internal/debug_renderer.hpp"

#include <cmath>
#include <string>
#include <vector>

#include <opencv2/imgproc.hpp>

namespace rmcs_laser_guidance {

DebugRenderer::DebugRenderer(const DebugConfig& debug_config)
    : debug_(debug_config) {}

auto DebugRenderer::draw(cv::Mat& image, const TargetObservation& observation) const -> void {
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
}

} // namespace rmcs_laser_guidance
