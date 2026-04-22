# 仓库指南

## 项目定位
`rmcs_laser_guidance` 是激光视觉引导模块的独立仓库，当前阶段只负责视觉链本身：

- 使用 `hikcamera` 取图
- 将图像封装为统一 `Frame`
- 运行最小 `Pipeline`
- 输出 `TargetObservation`
- 提供离线与真机相机验证入口

它当前**不是**：

- RMCS 运行时组件
- `sp_vision_25` 的裁剪版
- 带 `/gimbal/*` 输出的控制模块
- 带姿态同步或发弹决策的自瞄系统

如果你只记一件事，请记住：

- 这个仓库现在的设计目标是“先把视觉链路本身做干净”，因此核心库必须能在**不依赖 RMCS 控制链**的情况下独立运行和测试。

## 仓库地图

### 顶层

- `README.md`
  - 构建、运行和文档入口。
- `plan.md`
  - 项目的总目标，描述最终要达成的系统能力。
- `AGENTS.md`
  - 给后续接手 agent 的架构说明。项目结构变化时应同步更新。
- `package.xml`
  - ROS 2 包清单。
- `CMakeLists.txt`
  - 包构建图。
- `configs/default.yaml`
  - 最小默认配置。

### 代码目录

- `include/rmcs_laser_guidance/`
  - 公共 API，只保留 `config/types/pipeline`。
- `src/`
  - 核心实现，当前只有配置加载和最小亮点检测。
- `apps/`
  - 手工运行入口。
- `tests/`
  - 最小自动化测试。
- `docs/`
  - 项目文档，包含架构、开发约束与未来接入方向。

## 当前架构

### 数据流

当前主链非常短：

```text
hikcamera::Camera
-> Frame {image, timestamp}
-> Pipeline::process(...)
-> TargetObservation {detected, center, contour, brightness}
-> debug overlay / stdout
```

这条链只解决两个问题：

1. 图像是否能稳定采集
2. 检测 API 是否已经被工程化固定下来

### 运行入口

当前有两个入口程序：

- `hikcamera_preview`
  - 真机相机预览入口
  - 用于联通 `hikcamera` 和 `Pipeline`
- `offline_smoke`
  - 纯软件冒烟入口
  - 不依赖相机，只验证 pipeline 基本行为

### 测试入口

- `pipeline_smoke_test`
  - 验证 `Pipeline` 对空帧、黑图不会误报

### 构建图

当前 CMake 目标关系是：

```text
rmcs_laser_guidance_core
├── config.cpp
└── pipeline.cpp

hikcamera_preview -> rmcs_laser_guidance_core + hikcamera
offline_smoke -> rmcs_laser_guidance_core
pipeline_smoke_test -> rmcs_laser_guidance_core
```

## 公共接口

当前稳定 API 只有三类：

- `Config`
  - 读取 `camera.*` 和 `debug.*` 配置
- `Frame`
  - 统一封装图像和时间戳
- `TargetObservation`
  - 表示当前视觉结果
- `Pipeline`
  - 处理 `Frame` 并返回 `TargetObservation`

在当前阶段，这些接口比检测效果本身更重要，因为后续 tracker / solver / bridge 都会建立在这里。

## 配置约定

当前只允许两类配置：

- `camera.*`
  - `timeout_ms`
  - `exposure_us`
  - `framerate`
  - `gain`
  - `invert_image`
- `debug.*`
  - `show_window`
  - `draw_overlay`

原则：

- 第一阶段不要把配置做成 `sp_vision_25` 那种大扁平表
- 只有在真正出现新的可调行为时才增加键
- 不要预埋大量未来字段

## 当前边界

明确不做：

- `rmcs_executor`
- `pluginlib`
- `/tf`
- `HardSyncSnapshot`
- `/gimbal/*`
- `fire_control`
- `tracker`
- `solver`
- `planner`
- 多相机
- 视频回放

原因很简单：这些都属于第二阶段“接入 RMCS 或进入闭环控制”之后的事情，不是当前最小骨架要解决的问题。

## 开发约束

- 核心库必须保持无 ROS node 依赖。
- `apps/` 负责运行流程，不要把业务逻辑塞进去。
- `src/` 里的实现必须可以被 `tests/` 直接调用。
- 任何新的目录拆分都要有真实复杂度支撑，不要提前仿照 `sp_vision_25` 建 `io/tasks/tools` 三层。
- 每次架构变化后，优先更新：
  - `AGENTS.md`
  - `docs/architecture.md`
  - `README.md`

## 后续演进方向

预计第二阶段才会开始讨论：

- `tracker` 是否需要引入
- 是否接入 `/tf`
- 是否新增 `bridge`
- 是否输出 `/gimbal/laser_guidance/*`
- 是否与 RMCS 的 gimbal controller 对接

在那之前，这个仓库的判断标准只有一个：

- 不接控制链，视觉链是否已经足够干净、稳定、可测试
