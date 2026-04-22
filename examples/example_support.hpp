#pragma once

#include <filesystem>

#include "config.hpp"

namespace rmcs_laser_guidance::examples {

inline auto default_config_path() -> std::filesystem::path {
#ifdef RMCS_LASER_GUIDANCE_DEFAULT_CONFIG_PATH
    return RMCS_LASER_GUIDANCE_DEFAULT_CONFIG_PATH;
#else
    return "config/default.yaml";
#endif
}

inline auto default_sample_replay_path() -> std::filesystem::path {
#ifdef RMCS_LASER_GUIDANCE_DEFAULT_SAMPLE_REPLAY_PATH
    return RMCS_LASER_GUIDANCE_DEFAULT_SAMPLE_REPLAY_PATH;
#else
    return "test_data/sample_images";
#endif
}

inline auto should_exit_from_key(const int key) -> bool {
    return key == 27 || key == 'q' || key == 'Q';
}

inline auto pixel_format_name(const rmcs_laser_guidance::V4l2PixelFormat pixel_format) noexcept
    -> const char* {
    switch (pixel_format) {
    case rmcs_laser_guidance::V4l2PixelFormat::mjpeg:
        return "mjpeg";
    case rmcs_laser_guidance::V4l2PixelFormat::yuyv:
        return "yuyv";
    default:
        return "unknown";
    }
}

} // namespace rmcs_laser_guidance::examples
