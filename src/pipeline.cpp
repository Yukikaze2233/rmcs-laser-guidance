#include "pipeline.hpp"

#include <memory>

#include "internal/debug_renderer.hpp"
#include "internal/detector.hpp"

namespace rmcs_laser_guidance {

struct Pipeline::Details {
    explicit Details(const Config& config)
        : detector(config)
        , debug_renderer(config.debug) {}

    Detector detector;
    DebugRenderer debug_renderer;
};

Pipeline::Pipeline(const Config& config)
    : details_(std::make_unique<Details>(config)) {}

Pipeline::~Pipeline() = default;

Pipeline::Pipeline(Pipeline&&) noexcept = default;

auto Pipeline::operator=(Pipeline&&) noexcept -> Pipeline& = default;

auto Pipeline::process(const Frame& frame) const -> TargetObservation {
    return details_->detector.detect(frame);
}

auto Pipeline::draw_debug_overlay(cv::Mat& image, const TargetObservation& observation) const -> void {
    details_->debug_renderer.draw(image, observation);
}

} // namespace rmcs_laser_guidance
