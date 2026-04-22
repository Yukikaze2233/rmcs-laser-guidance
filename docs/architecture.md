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
hikcamera::Camera
-> read_image_with_timestamp()
-> Frame
-> Pipeline::process()
-> TargetObservation
-> draw_debug_overlay() / stdout
```

其中：

- `Frame` 是图像和时间戳的统一载体
- `TargetObservation` 是最小视觉结果
- `Pipeline` 是唯一业务入口

## 模块职责

### Config

负责把 `default.yaml` 解析成强类型结构，避免入口程序散落 YAML 解析逻辑。

### Frame

统一图像输入格式，后续不管是：

- `hikcamera`
- 视频文件
- RMCS bridge

都先变成 `Frame` 再进入 pipeline。

### Pipeline

当前是极简亮点检测器，职责只有：

- 接受 `Frame`
- 返回 `TargetObservation`
- 可选绘制调试 overlay

### Apps

- `hikcamera_preview`
  - 真机入口
- `offline_smoke`
  - 纯软件入口

Apps 只负责运行流程，不负责视觉算法本身。

## 当前输出

当前输出只有两类：

- 结构化结果：`TargetObservation`
- 调试结果：图像窗口 / 标准输出

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

