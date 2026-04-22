#pragma once

#include <string>

#include "types.hpp"

namespace rmcs_laser_guidance {

struct ModelInferResult {
    bool enabled = false;
    bool success = false;
    std::string message{};
};

class ModelInfer {
public:
    ModelInfer() = default;

    auto infer(const Frame& frame) const -> ModelInferResult;
};

} // namespace rmcs_laser_guidance
