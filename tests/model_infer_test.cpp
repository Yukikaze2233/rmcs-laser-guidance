#include <cstdio>
#include <print>

#include "test_utils.hpp"
#include "internal/model_infer.hpp"
#include "internal/replay.hpp"

int main() {
    try {
        using namespace rmcs_laser_guidance::tests;

        const auto dataset =
            rmcs_laser_guidance::load_replay_dataset(default_sample_replay_path());
        require(!dataset.frames.empty(), "sample dataset should not be empty");

        const auto frame = rmcs_laser_guidance::load_replay_frame(dataset, dataset.frames.front());
        const rmcs_laser_guidance::ModelInfer model_infer;
        const auto result = model_infer.infer(frame);

        require(!result.enabled, "model infer should be disabled");
        require(!result.success, "model infer should not succeed");
        require(
            result.message.find("not enabled") != std::string::npos,
            "model infer message should mention disabled state");

        return 0;
    } catch (const std::exception& e) {
        std::println(stderr, "model_infer_test failed: {}", e.what());
        return 1;
    }
}
