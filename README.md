# rmcs_laser_guidance

`rmcs_laser_guidance` 是一个最小激光视觉引导骨架，当前阶段只覆盖：

- `V4L2/UVC` 采集卡取图
- YAML 配置加载
- `Pipeline` 统一视觉入口与可切换后端骨架
- 最小亮点检测 `Detector`
- 调试 overlay 与回放样本
- 自动测试与人工运行示例

当前阶段**不包含**：

- RMCS 控制接入
- `/gimbal/*` 接口
- `HardSyncSnapshot`
- 发弹时机或 `fire_control`

## Build

仓库根目录直接构建：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

配套测试：

```bash
ctest --test-dir build --output-on-failure
```

如果你在带 Jazzy 的 ROS 工作区里，也可以继续使用：

```bash
build-rmcs --packages-select rmcs_laser_guidance
```

或：

```bash
cd rmcs_ws
colcon build --packages-select rmcs_laser_guidance --symlink-install --merge-install
```

说明：

- 当机器上没有 `/opt/ros/jazzy` 和 `ament_cmake` 时，ROS/`colcon` 路径不可用，但根目录直接 `cmake` 仍应可用。
- 当机器上有完整 Jazzy 环境时，两种构建方式都支持。

## Auto Tests

自动验证目标位于 `tests/*_test.cpp`，可通过：

```bash
ctest --output-on-failure
```

在对应构建目录中执行全部自动测试。Standalone 构建对应 `build/`，ROS 工作区构建对应 `build/rmcs_laser_guidance/`。

## Examples

离线冒烟：

```bash
ros2 run rmcs_laser_guidance example_offline_smoke
```

V4L2 预览：

```bash
ros2 run rmcs_laser_guidance example_v4l2_preview
```

也可以显式传配置路径：

```bash
ros2 run rmcs_laser_guidance example_v4l2_preview /abs/path/to/default.yaml
```

V4L2 采图录帧：

```bash
ros2 run rmcs_laser_guidance example_v4l2_capture
```

回放预览：

```bash
ros2 run rmcs_laser_guidance example_replay_preview
```

检测 benchmark：

```bash
ros2 run rmcs_laser_guidance example_detector_benchmark
```

模型推理占位入口：

```bash
ros2 run rmcs_laser_guidance example_model_infer
```

## Notes

- 默认配置文件位于 `config/default.yaml`
- 默认样本回放位于 `test_data/sample_images`
- `models/` 用于放置 `.onnx` 模型文件
- public 头文件平铺在 `include/`，实现细节头收束在 `src/internal/`
- 默认 live 输入为 UVC 采集卡 `v4l2.device_path=/dev/video0`
- 默认 live 模式为 `1920x1080 @ 60 FPS`，优先 `mjpeg`
- 当前检测逻辑仍然是极简亮点检测实现，用于把工程链路跑通
- `Pipeline` 通过 `inference.backend` 在 `bright_spot` 和 `model` 占位后端间切换
- `inference.model_path` 当前只用于预留模型接入配置，不会真正加载 ONNX
- `model_infer_*` 当前只是占位接口，不接真实推理后端
- 后续如果要接 RMCS，再单独增加 bridge 和控制接口

## Docs

- `plan.md`
- `docs/README.md`
- `docs/architecture.md`
- `docs/development.md`
- `docs/future_rmcs_integration.md`
