#include <cstdio>
#include <print>

#include "config.hpp"
#include "internal/replay.hpp"
#include "pipeline.hpp"
#include "test_utils.hpp"

int main() {
    try {
        using namespace rmcs_laser_guidance::tests;

        const auto dataset = rmcs_laser_guidance::load_replay_dataset(default_sample_replay_path());
        require(!dataset.frames.empty(), "sample dataset should not be empty");

        const auto frame = rmcs_laser_guidance::load_replay_frame(dataset, dataset.frames.front());
        auto config      = rmcs_laser_guidance::load_config(default_config_path());
        config.inference.backend    = rmcs_laser_guidance::InferenceBackendKind::model;
        config.inference.model_path = "models/mock_detector.onnx";

        rmcs_laser_guidance::Pipeline pipeline(config);
        const auto observation = pipeline.process(frame);

        require(!observation.detected, "model placeholder pipeline should not detect");
        require_near(observation.center.x, -1.0F, 1e-3F, "model placeholder center.x");
        require_near(observation.center.y, -1.0F, 1e-3F, "model placeholder center.y");
        require_near(observation.brightness, 0.0F, 1e-3F, "model placeholder brightness");

        return 0;
    } catch (const std::exception& e) {
        std::println(stderr, "model_infer_test failed: {}", e.what());
        return 1;
    }
}
