#include "rmcs_laser_guidance/config.hpp"

#include <stdexcept>

#include <yaml-cpp/yaml.h>

namespace rmcs_laser_guidance {

auto load_config(const std::filesystem::path& config_path) -> Config {
    YAML::Node yaml = YAML::LoadFile(config_path.string());

    Config config;

    if (const YAML::Node camera = yaml["camera"]) {
        if (camera["timeout_ms"])
            config.camera.timeout_ms = camera["timeout_ms"].as<unsigned int>();
        if (camera["exposure_us"])
            config.camera.exposure_us = camera["exposure_us"].as<float>();
        if (camera["framerate"])
            config.camera.framerate = camera["framerate"].as<float>();
        if (camera["gain"])
            config.camera.gain = camera["gain"].as<float>();
        if (camera["invert_image"])
            config.camera.invert_image = camera["invert_image"].as<bool>();
    }

    if (const YAML::Node debug = yaml["debug"]) {
        if (debug["show_window"])
            config.debug.show_window = debug["show_window"].as<bool>();
        if (debug["draw_overlay"])
            config.debug.draw_overlay = debug["draw_overlay"].as<bool>();
    }

    if (config.camera.framerate <= 0.0F)
        throw std::runtime_error("camera.framerate must be positive");
    if (config.camera.exposure_us <= 0.0F)
        throw std::runtime_error("camera.exposure_us must be positive");

    return config;
}

} // namespace rmcs_laser_guidance

