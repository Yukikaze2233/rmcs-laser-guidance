#include "internal/model_infer.hpp"

#include <utility>

namespace rmcs_laser_guidance {

ModelInfer::ModelInfer(InferenceConfig config)
    : config_(std::move(config)) { }

auto ModelInfer::infer(const Frame& frame) const -> ModelInferResult {
    (void)frame;

    std::string message = "model backend is not enabled in this build";
    if (!config_.model_path.empty()) message += ": " + config_.model_path.string();

    return {
        .enabled     = false,
        .success     = false,
        .observation = { },
        .message     = std::move(message),
    };
}

} // namespace rmcs_laser_guidance
