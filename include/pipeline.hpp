#pragma once

#include <memory>

#include "config.hpp"
#include "types.hpp"

namespace rmcs_laser_guidance {

class Pipeline {
public:
    explicit Pipeline(const Config& config);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    auto operator=(const Pipeline&) -> Pipeline& = delete;

    Pipeline(Pipeline&&) noexcept;
    auto operator=(Pipeline&&) noexcept -> Pipeline&;

    auto process(const Frame& frame) const -> TargetObservation;
    auto draw_debug_overlay(cv::Mat& image, const TargetObservation& observation) const -> void;

private:
    struct Details;
    std::unique_ptr<Details> details_;
};

} // namespace rmcs_laser_guidance
