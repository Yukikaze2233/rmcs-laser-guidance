# rmcs_laser_guidance

`rmcs_laser_guidance` 是一个最小激光视觉引导骨架，当前阶段只覆盖：

- `hikcamera` 取图
- YAML 配置加载
- 最小亮点检测 pipeline
- 调试窗口与离线冒烟测试

当前阶段**不包含**：

- RMCS 控制接入
- `/gimbal/*` 接口
- `HardSyncSnapshot`
- 发弹时机或 `fire_control`

## Build

```bash
build-rmcs --packages-select rmcs_laser_guidance
```

或：

```bash
cd rmcs_ws
colcon build --packages-select rmcs_laser_guidance --symlink-install --merge-install
```

## Run

离线冒烟：

```bash
ros2 run rmcs_laser_guidance offline_smoke
```

海康相机预览：

```bash
ros2 run rmcs_laser_guidance hikcamera_preview
```

也可以显式传配置路径：

```bash
ros2 run rmcs_laser_guidance hikcamera_preview /abs/path/to/default.yaml
```

## Notes

- 默认配置文件位于 `configs/default.yaml`
- 当前检测逻辑只是一个极简亮点检测占位实现，用于把工程链路跑通
- 后续如果要接 RMCS，再单独增加 bridge 和控制接口

## Docs

- `plan.md`
- `docs/README.md`
- `docs/architecture.md`
- `docs/development.md`
- `docs/future_rmcs_integration.md`
