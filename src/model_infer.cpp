#include "internal/model_infer.hpp"

namespace rmcs_laser_guidance {

auto ModelInfer::infer(const Frame& frame) const -> ModelInferResult {
    (void)frame;
    return {
        .enabled = false,
        .success = false,
        .message = "model infer is not enabled in this build",
    };
}

} // namespace rmcs_laser_guidance
