#include <cstdio>
#include <filesystem>
#include <print>

#include "config.hpp"
#include "example_support.hpp"
#include "internal/replay.hpp"
#include "pipeline.hpp"

namespace {

auto resolve_config_path(int argc, char** argv) -> std::filesystem::path {
    if (argc > 1) return argv[1];
    return rmcs_laser_guidance::examples::default_config_path();
}

auto resolve_replay_dir(int argc, char** argv) -> std::filesystem::path {
    if (argc > 2) return argv[2];
    return rmcs_laser_guidance::examples::default_sample_replay_path();
}

} // namespace

int main(int argc, char** argv) {
    try {
        const auto config_path   = resolve_config_path(argc, argv);
        const auto replay_dir    = resolve_replay_dir(argc, argv);
        auto config              = rmcs_laser_guidance::load_config(config_path);
        config.inference.backend = rmcs_laser_guidance::InferenceBackendKind::model;

        const auto dataset = rmcs_laser_guidance::load_replay_dataset(replay_dir);
        if (dataset.frames.empty()) {
            std::println(stderr, "No sample frames available for model infer example.");
            return 1;
        }

        const auto frame = rmcs_laser_guidance::load_replay_frame(dataset, dataset.frames.front());
        rmcs_laser_guidance::Pipeline pipeline(config);
        const auto observation = pipeline.process(frame);

        std::println("backend={} model_path={} detected={} brightness={}",
            rmcs_laser_guidance::examples::inference_backend_name(config.inference.backend),
            config.inference.model_path.empty() ? "<unset>" : config.inference.model_path.string(),
            observation.detected, observation.brightness);
        return 0;
    } catch (const std::exception& e) {
        std::println(stderr, "example_model_infer failed: {}", e.what());
        return 1;
    }
}
