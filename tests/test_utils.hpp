#pragma once

#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace rmcs_laser_guidance::tests {

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

inline auto make_temp_path(std::string_view stem) -> std::filesystem::path {
    const auto unique =
        std::to_string(std::filesystem::file_time_type::clock::now().time_since_epoch().count());
    return std::filesystem::temp_directory_path() /
           (std::string(stem) + "_" + unique + ".yaml");
}

inline auto write_text_file(const std::filesystem::path& path, std::string_view content) -> void {
    if (path.has_parent_path())
        std::filesystem::create_directories(path.parent_path());

    std::ofstream out(path);
    if (!out)
        throw std::runtime_error("failed to open temp file for writing");
    out << content;
}

inline auto require(const bool condition, std::string_view message) -> void {
    if (!condition)
        throw std::runtime_error(std::string(message));
}

inline auto require_near(
    const float actual, const float expected, const float tolerance, std::string_view label)
    -> void {
    if (std::fabs(actual - expected) <= tolerance)
        return;

    std::ostringstream oss;
    oss << label << " expected " << expected << " +/- " << tolerance << ", got " << actual;
    throw std::runtime_error(oss.str());
}

} // namespace rmcs_laser_guidance::tests
