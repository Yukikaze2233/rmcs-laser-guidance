#pragma once

#include <filesystem>

namespace rmcs_laser_guidance {

struct CameraConfig {
    unsigned int timeout_ms = 2000;
    float exposure_us = 1500.0F;
    float framerate = 80.0F;
    float gain = 16.0F;
    bool invert_image = false;
};

struct DebugConfig {
    bool show_window = true;
    bool draw_overlay = true;
};

struct Config {
    CameraConfig camera{};
    DebugConfig debug{};
};

auto load_config(const std::filesystem::path& config_path) -> Config;

} // namespace rmcs_laser_guidance

