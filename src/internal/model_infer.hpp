#pragma once

#include <string>

#include "config.hpp"
#include "types.hpp"

namespace rmcs_laser_guidance {

struct ModelInferResult {
    bool enabled = false;
    bool success = false;
    TargetObservation observation { };
    std::string message { };
};

class ModelInfer {
public:
    explicit ModelInfer(InferenceConfig config = { });

    auto infer(const Frame& frame) const -> ModelInferResult;

private:
    InferenceConfig config_;
};

} // namespace rmcs_laser_guidance
