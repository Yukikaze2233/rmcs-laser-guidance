#include <cstdio>
#include <print>

#include "test_utils.hpp"
#include "config.hpp"

int main() {
    try {
        using namespace rmcs_laser_guidance::tests;

        const auto default_config = rmcs_laser_guidance::load_config(default_config_path());
        require(
            default_config.v4l2.device_path == std::filesystem::path("/dev/video0"),
            "default device path mismatch");
        require(default_config.v4l2.width == 1920, "default width mismatch");
        require(default_config.v4l2.height == 1080, "default height mismatch");
        require_near(default_config.v4l2.framerate, 60.0F, 1e-3F, "default framerate");
        require(
            default_config.v4l2.pixel_format == rmcs_laser_guidance::V4l2PixelFormat::mjpeg,
            "default pixel format mismatch");
        require(!default_config.v4l2.invert_image, "default invert_image mismatch");
        require(default_config.debug.show_window, "default show_window mismatch");
        require(default_config.debug.draw_overlay, "default draw_overlay mismatch");

        const auto override_path = make_temp_path("rmcs_laser_guidance_config_override");
        write_text_file(
            override_path,
            "v4l2:\n"
            "  device_path: /dev/video7\n"
            "  width: 1280\n"
            "  height: 720\n"
            "  framerate: 59.94\n"
            "  pixel_format: yuyv\n"
            "  invert_image: true\n"
            "debug:\n"
            "  show_window: false\n"
            "  draw_overlay: false\n");
        const auto override_config = rmcs_laser_guidance::load_config(override_path);
        require(
            override_config.v4l2.device_path == std::filesystem::path("/dev/video7"),
            "override device path mismatch");
        require(override_config.v4l2.width == 1280, "override width mismatch");
        require(override_config.v4l2.height == 720, "override height mismatch");
        require_near(override_config.v4l2.framerate, 59.94F, 1e-3F, "override framerate");
        require(
            override_config.v4l2.pixel_format == rmcs_laser_guidance::V4l2PixelFormat::yuyv,
            "override pixel format mismatch");
        require(override_config.v4l2.invert_image, "override invert_image mismatch");
        require(!override_config.debug.show_window, "override show_window mismatch");
        require(!override_config.debug.draw_overlay, "override draw_overlay mismatch");

        const auto bad_framerate_path = make_temp_path("rmcs_laser_guidance_bad_framerate");
        write_text_file(
            bad_framerate_path,
            "v4l2:\n"
            "  framerate: 0.0\n"
            "  width: 1920\n"
            "  height: 1080\n"
            "  device_path: /dev/video0\n");
        bool bad_framerate_threw = false;
        try {
            (void)rmcs_laser_guidance::load_config(bad_framerate_path);
        } catch (const std::exception&) {
            bad_framerate_threw = true;
        }
        require(bad_framerate_threw, "bad framerate should throw");

        const auto bad_width_path = make_temp_path("rmcs_laser_guidance_bad_width");
        write_text_file(
            bad_width_path,
            "v4l2:\n"
            "  device_path: /dev/video0\n"
            "  width: 0\n"
            "  height: 1080\n"
            "  framerate: 10.0\n");
        bool bad_width_threw = false;
        try {
            (void)rmcs_laser_guidance::load_config(bad_width_path);
        } catch (const std::exception&) {
            bad_width_threw = true;
        }
        require(bad_width_threw, "bad width should throw");

        const auto bad_device_path = make_temp_path("rmcs_laser_guidance_bad_device_path");
        write_text_file(
            bad_device_path,
            "v4l2:\n"
            "  device_path: ''\n"
            "  width: 1920\n"
            "  height: 1080\n"
            "  framerate: 10.0\n");
        bool bad_device_path_threw = false;
        try {
            (void)rmcs_laser_guidance::load_config(bad_device_path);
        } catch (const std::exception&) {
            bad_device_path_threw = true;
        }
        require(bad_device_path_threw, "empty device path should throw");

        const auto bad_pixel_format = make_temp_path("rmcs_laser_guidance_bad_pixel_format");
        write_text_file(
            bad_pixel_format,
            "v4l2:\n"
            "  device_path: /dev/video0\n"
            "  width: 1920\n"
            "  height: 1080\n"
            "  framerate: 10.0\n"
            "  pixel_format: rgb24\n");
        bool bad_pixel_format_threw = false;
        try {
            (void)rmcs_laser_guidance::load_config(bad_pixel_format);
        } catch (const std::exception&) {
            bad_pixel_format_threw = true;
        }
        require(bad_pixel_format_threw, "bad pixel format should throw");

        bool missing_path_threw = false;
        try {
            (void)rmcs_laser_guidance::load_config("/tmp/does_not_exist_rmcs_laser_guidance.yaml");
        } catch (const std::exception&) {
            missing_path_threw = true;
        }
        require(missing_path_threw, "missing config path should throw");

        return 0;
    } catch (const std::exception& e) {
        std::println(stderr, "config_test failed: {}", e.what());
        return 1;
    }
}
