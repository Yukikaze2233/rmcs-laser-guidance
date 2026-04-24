#include <cstdio>
#include <print>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "internal/model_adapter.hpp"
#include "internal/model_runtime.hpp"
#include "test_utils.hpp"

namespace {

auto make_frame() -> rmcs_laser_guidance::Frame {
    cv::Mat image = cv::Mat::zeros(100, 100, CV_8UC3);
    cv::rectangle(image, { 40, 40 }, { 60, 60 }, { 255, 255, 255 }, -1);
    return rmcs_laser_guidance::Frame {
        .image     = image,
        .timestamp = rmcs_laser_guidance::Clock::now(),
    };
}

auto identity_transform() -> rmcs_laser_guidance::ModelImageTransform {
    return rmcs_laser_guidance::ModelImageTransform {
        .original_width  = 100,
        .original_height = 100,
        .input_width     = 100,
        .input_height    = 100,
        .scale           = 1.0F,
        .pad_x           = 0.0F,
        .pad_y           = 0.0F,
    };
}

} // namespace

int main() {
    try {
        using namespace rmcs_laser_guidance::tests;

        const auto frame = make_frame();

        const auto raw_result = rmcs_laser_guidance::adapt_yolov5_outputs(frame,
            rmcs_laser_guidance::ModelRunResult{
                .success = true,
                .message = {},
                .transform = identity_transform(),
                .outputs = {
                    rmcs_laser_guidance::ModelTensorData{
                        .name = "output0",
                        .shape = {1, 3, 6},
                        .element_type = "float32",
                        .values = {
                            50.0F, 50.0F, 20.0F, 20.0F, 0.90F, 0.90F,
                            51.0F, 51.0F, 20.0F, 20.0F, 0.80F, 0.90F,
                            20.0F, 20.0F, 10.0F, 10.0F, 0.10F, 0.90F,
                        },
                    },
                },
            });
        require(raw_result.success, "raw adapter result should succeed");
        require(raw_result.contract_supported, "raw adapter contract should be supported");
        require(raw_result.observation.detected, "raw adapter should detect target");
        require(
            raw_result.candidates.size() == 1, "raw adapter should keep one candidate after NMS");
        require_near(raw_result.observation.center.x, 50.0F, 2.0F, "raw adapter center.x");
        require_near(raw_result.observation.center.y, 50.0F, 2.0F, "raw adapter center.y");
        require(raw_result.observation.contour.size() == 4, "raw adapter contour size mismatch");
        require(raw_result.observation.brightness > 200.0F, "raw adapter brightness mismatch");

        const auto nms_result = rmcs_laser_guidance::adapt_yolov5_outputs(frame,
            rmcs_laser_guidance::ModelRunResult{
                .success = true,
                .message = {},
                .transform = identity_transform(),
                .outputs = {
                    rmcs_laser_guidance::ModelTensorData{
                        .name = "detections",
                        .shape = {1, 2, 6},
                        .element_type = "float32",
                        .values = {
                            10.0F, 10.0F, 30.0F, 30.0F, 0.85F, 0.0F,
                            60.0F, 60.0F, 90.0F, 90.0F, 0.60F, 0.0F,
                        },
                    },
                },
            });
        require(nms_result.success, "nms adapter result should succeed");
        require(nms_result.contract_supported, "nms adapter contract should be supported");
        require(nms_result.candidates.size() == 2, "nms adapter candidate count mismatch");
        require_near(
            nms_result.candidates.front().center.x, 20.0F, 2.0F, "nms adapter top1 center.x");

        const auto split_nms_result = rmcs_laser_guidance::adapt_yolov5_outputs(frame,
            rmcs_laser_guidance::ModelRunResult{
                .success = true,
                .message = {},
                .transform = identity_transform(),
                .outputs = {
                    rmcs_laser_guidance::ModelTensorData{
                        .name = "num_dets",
                        .shape = {1},
                        .element_type = "float32",
                        .values = {1.0F},
                    },
                    rmcs_laser_guidance::ModelTensorData{
                        .name = "boxes",
                        .shape = {1, 2, 4},
                        .element_type = "float32",
                        .values = {
                            40.0F, 40.0F, 60.0F, 60.0F,
                            10.0F, 10.0F, 20.0F, 20.0F,
                        },
                    },
                    rmcs_laser_guidance::ModelTensorData{
                        .name = "scores",
                        .shape = {1, 2},
                        .element_type = "float32",
                        .values = {0.95F, 0.10F},
                    },
                    rmcs_laser_guidance::ModelTensorData{
                        .name = "classes",
                        .shape = {1, 2},
                        .element_type = "float32",
                        .values = {0.0F, 0.0F},
                    },
                },
            });
        require(split_nms_result.success, "split nms adapter result should succeed");
        require(split_nms_result.contract_supported, "split nms contract should be supported");
        require(split_nms_result.candidates.size() == 1, "split nms candidate count mismatch");
        require_near(split_nms_result.observation.center.x, 50.0F, 2.0F, "split nms center.x");
        require_near(split_nms_result.observation.center.y, 50.0F, 2.0F, "split nms center.y");

        const auto unsupported_result = rmcs_laser_guidance::adapt_yolov5_outputs(frame,
            rmcs_laser_guidance::ModelRunResult{
                .success = true,
                .message = {},
                .transform = identity_transform(),
                .outputs = {
                    rmcs_laser_guidance::ModelTensorData{
                        .name = "bad",
                        .shape = {1, 4, 5},
                        .element_type = "float32",
                        .values = std::vector<float>(20, 0.0F),
                    },
                },
            });
        require(!unsupported_result.success, "unsupported adapter contract should fail");
        require(!unsupported_result.contract_supported,
            "unsupported adapter contract should not be supported");
        require_contains(unsupported_result.message, "unsupported", "unsupported adapter message");

        const auto empty_detection_result = rmcs_laser_guidance::adapt_yolov5_outputs(frame,
            rmcs_laser_guidance::ModelRunResult{
                .success = true,
                .message = {},
                .transform = identity_transform(),
                .outputs = {
                    rmcs_laser_guidance::ModelTensorData{
                        .name = "output0",
                        .shape = {1, 1, 6},
                        .element_type = "float32",
                        .values = {20.0F, 20.0F, 10.0F, 10.0F, 0.05F, 0.90F},
                    },
                },
            });
        require(empty_detection_result.success, "empty detection result should still succeed");
        require(empty_detection_result.contract_supported,
            "empty detection result should still support the contract");
        require(!empty_detection_result.observation.detected,
            "empty detection result should not detect");

        return 0;
    } catch (const std::exception& e) {
        std::println(stderr, "model_adapter_test failed: {}", e.what());
        return 1;
    }
}
