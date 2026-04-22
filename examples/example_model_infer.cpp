#include <cstdio>
#include <print>

#include "example_support.hpp"
#include "internal/model_infer.hpp"
#include "internal/replay.hpp"

int main() {
    try {
        const auto dataset = rmcs_laser_guidance::load_replay_dataset(
            rmcs_laser_guidance::examples::default_sample_replay_path());
        if (dataset.frames.empty()) {
            std::println(stderr, "No sample frames available for model infer example.");
            return 1;
        }

        const auto frame = rmcs_laser_guidance::load_replay_frame(dataset, dataset.frames.front());
        const rmcs_laser_guidance::ModelInfer model_infer;
        const auto result = model_infer.infer(frame);

        std::println(
            "enabled={} success={} message=\"{}\"",
            result.enabled,
            result.success,
            result.message);
        return 0;
    } catch (const std::exception& e) {
        std::println(stderr, "example_model_infer failed: {}", e.what());
        return 1;
    }
}
