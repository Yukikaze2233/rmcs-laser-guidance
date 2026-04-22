#include "internal/v4l2_capture.hpp"

#include <cerrno>
#include <charconv>
#include <cmath>
#include <cstring>
#include <optional>
#include <string>
#include <utility>

#include <opencv2/core.hpp>

namespace rmcs_laser_guidance {
namespace {

auto requested_fourcc(const V4l2PixelFormat pixel_format) -> int {
    switch (pixel_format) {
    case V4l2PixelFormat::mjpeg:
        return cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    case V4l2PixelFormat::yuyv:
        return cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V');
    default:
        return 0;
    }
}

auto maybe_device_index(const std::filesystem::path& device_path) -> std::optional<int> {
    const std::string filename = device_path.filename().string();
    constexpr std::string_view prefix = "video";
    if (!filename.starts_with(prefix))
        return std::nullopt;

    const std::string_view suffix = std::string_view(filename).substr(prefix.size());
    if (suffix.empty())
        return std::nullopt;

    int index = -1;
    const auto [ptr, ec] = std::from_chars(suffix.data(), suffix.data() + suffix.size(), index);
    if (ec != std::errc{} || ptr != suffix.data() + suffix.size() || index < 0)
        return std::nullopt;
    return index;
}

auto open_capture(cv::VideoCapture& capture, const std::filesystem::path& device_path) -> bool {
    if (const auto index = maybe_device_index(device_path))
        return capture.open(*index, cv::CAP_V4L2);
    return capture.open(device_path.string(), cv::CAP_V4L2);
}

} // namespace

V4l2Capture::V4l2Capture(V4l2Config config)
    : config_(std::move(config)) {}

V4l2Capture::~V4l2Capture() noexcept {
    close();
}

auto V4l2Capture::open() -> std::expected<V4l2NegotiatedFormat, std::string> {
    close();

    if (!open_capture(capture_, config_.device_path)) {
        return std::unexpected("failed to open V4L2 device " + config_.device_path.string());
    }

    (void)capture_.set(cv::CAP_PROP_FOURCC, static_cast<double>(requested_fourcc(config_.pixel_format)));
    (void)capture_.set(cv::CAP_PROP_FRAME_WIDTH, static_cast<double>(config_.width));
    (void)capture_.set(cv::CAP_PROP_FRAME_HEIGHT, static_cast<double>(config_.height));
    (void)capture_.set(cv::CAP_PROP_FPS, static_cast<double>(config_.framerate));

    negotiated_ = {
        .device_path = config_.device_path,
        .width = static_cast<int>(std::lround(capture_.get(cv::CAP_PROP_FRAME_WIDTH))),
        .height = static_cast<int>(std::lround(capture_.get(cv::CAP_PROP_FRAME_HEIGHT))),
        .framerate = capture_.get(cv::CAP_PROP_FPS),
        .fourcc = fourcc_string_from_int(static_cast<int>(capture_.get(cv::CAP_PROP_FOURCC))),
    };

    return negotiated_;
}

auto V4l2Capture::read_frame() -> std::expected<Frame, std::string> {
    if (!capture_.isOpened())
        return std::unexpected("V4L2 device is not open");

    cv::Mat image;
    if (!capture_.read(image) || image.empty()) {
        return std::unexpected(
            "failed to read frame from V4L2 device " + config_.device_path.string());
    }

    if (config_.invert_image)
        cv::bitwise_not(image, image);

    return Frame{
        .image = image,
        .timestamp = Clock::now(),
    };
}

auto V4l2Capture::close() noexcept -> void {
    if (capture_.isOpened())
        capture_.release();
}

auto V4l2Capture::is_open() const noexcept -> bool {
    return capture_.isOpened();
}

auto V4l2Capture::negotiated_format() const noexcept -> const V4l2NegotiatedFormat& {
    return negotiated_;
}

auto fourcc_string_from_int(const int fourcc) -> std::string {
    if (fourcc == 0)
        return "unknown";

    std::string value(4, '\0');
    value[0] = static_cast<char>(fourcc & 0xFF);
    value[1] = static_cast<char>((fourcc >> 8) & 0xFF);
    value[2] = static_cast<char>((fourcc >> 16) & 0xFF);
    value[3] = static_cast<char>((fourcc >> 24) & 0xFF);

    for (char& ch : value) {
        if (ch == '\0')
            ch = ' ';
    }

    return value;
}

} // namespace rmcs_laser_guidance
