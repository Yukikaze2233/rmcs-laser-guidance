#pragma once

#include "rmcs_laser_guidance/config.hpp"
#include "rmcs_laser_guidance/types.hpp"

namespace rmcs_laser_guidance {

class Pipeline {
public:
    explicit Pipeline(const Config& config);

    auto process(const Frame& frame) const -> TargetObservation;
    auto draw_debug_overlay(cv::Mat& image, const TargetObservation& observation) const -> void;

private:
    DebugConfig debug_;
};

} // namespace rmcs_laser_guidance

