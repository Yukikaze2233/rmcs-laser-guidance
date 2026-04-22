# 架构

## 当前目标

`rmcs_laser_guidance` 当前只解决视觉链路最小可运行问题：

- 相机是否能稳定出图
- 图像是否能被统一封装
- 检测入口是否已经固定
- 调试输出是否能独立于 RMCS 运行

它当前不是控制模块，因此不包含：

- 姿态同步
- 目标跟踪
- 弹道或开火时机
- `/gimbal/*` 输出

## 当前数据流

```text
V4l2Capture
-> read_frame()
-> Frame
-> Pipeline::process()
-> selected backend
-> Detector / ModelInfer
-> TargetObservation
-> draw_debug_overlay() / stdout
```

其中：

- `Frame` 是图像和时间戳的统一载体
- `TargetObservation` 是最小视觉结果
- `Detector` 负责最小亮点检测
- `ModelInfer` 负责未来模型推理接缝，当前仍是占位实现
- `DebugRenderer` 负责最小调试绘制
- `Pipeline` 组合“已选视觉后端”与 `DebugRenderer`
- `V4l2Capture` 负责从 `/dev/videoN` 读取 UVC 图像

## 模块职责

### Config

负责把 `default.yaml` 解析成强类型结构，避免入口程序散落 YAML 解析逻辑。

当前还承载视觉后端选择配置：

- `inference.backend`
  - 当前支持 `bright_spot` 与 `model`
- `inference.model_path`
  - 为后续 ONNX 接入预留模型路径，当前不会真正加载

### Frame

统一图像输入格式，后续不管是：

- `V4L2/UVC`
- 视频文件
- RMCS bridge

都先变成 `Frame` 再进入 pipeline。

### Pipeline

当前是最小视觉主入口，职责只有：

- 接受 `Frame`
- 返回 `TargetObservation`
- 在构造时选择视觉后端
- 转调 `Detector` 或 `ModelInfer`
- 转调 `DebugRenderer`

### Replay

- `ReplayRecorder`
  - 把 `Frame` 录成 `PNG + manifest.csv`
- `load_replay_dataset`
  - 从样本或录帧目录回放 `Frame`

### Examples

- `example_v4l2_preview`
  - 真机入口
- `example_v4l2_capture`
  - 录帧入口
- `example_offline_smoke`
  - 纯软件入口
- `example_replay_preview`
  - 回放入口
- `example_detector_benchmark`
  - 离线 benchmark
- `example_model_infer`
  - 通过 `Pipeline` 验证 `model` 占位后端接缝

Examples 只负责运行流程，不负责视觉算法本身。

## 当前输出

当前输出只有两类：

- 结构化结果：`TargetObservation`
- 调试结果：图像窗口 / 标准输出 / 回放目录

这意味着当前仓库的“外部协议”还没有形成。真正的 RMCS 内部总线接口要到第二阶段才会定义。

## 当前非目标

以下内容不应该在第一阶段偷偷引入：

- `/tf`
- `HardSyncSnapshot`
- `rmcs_executor::Component`
- `pluginlib`
- 规划器
- 控制指令
- `fire_control`

这些内容一旦进入，就会把视觉最小骨架变成“半个控制系统”，偏离当前目标。
