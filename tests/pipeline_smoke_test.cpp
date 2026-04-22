#include <filesystem>
#include <iostream>

#include <opencv2/core/mat.hpp>

#include "rmcs_laser_guidance/config.hpp"
#include "rmcs_laser_guidance/pipeline.hpp"

namespace {

auto resolve_config_path() -> std::filesystem::path {
#ifdef RMCS_LASER_GUIDANCE_DEFAULT_CONFIG_PATH
    return RMCS_LASER_GUIDANCE_DEFAULT_CONFIG_PATH;
#else
    return "configs/default.yaml";
#endif
}

} // namespace

int main() {
    try {
        const auto config = rmcs_laser_guidance::load_config(resolve_config_path());
        rmcs_laser_guidance::Pipeline pipeline(config);

        rmcs_laser_guidance::Frame empty_frame;
        if (pipeline.process(empty_frame).detected) {
            std::cerr << "Empty frame should not be detected.\n";
            return 1;
        }

        rmcs_laser_guidance::Frame blank_frame{
            .image = cv::Mat::zeros(240, 320, CV_8UC3),
            .timestamp = rmcs_laser_guidance::Clock::now(),
        };
        if (pipeline.process(blank_frame).detected) {
            std::cerr << "Blank frame should not be detected.\n";
            return 1;
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "pipeline_smoke_test failed: " << e.what() << '\n';
        return 1;
    }
}
