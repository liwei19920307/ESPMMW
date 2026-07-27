# ESPMMW Distance Card（HA 集成）

把 1D 毫米波距离尺 Lovelace 卡片打包成 Home Assistant 自定义集成，支持 **HACS 一键安装**，并自动注册前端资源。

## HACS 安装（推荐）

1. HACS → 右上角三点 → **自定义仓库**
2. 仓库：`https://github.com/liwei19920307/ESPMMW`
3. 类别：**Integration**
4. 搜索 **ESPMMW Distance Card** → 下载
5. **重启 Home Assistant**
6. 设置 → 设备与服务 → 添加集成 → **ESPMMW Distance Card**

## 手动安装

```bash
cp -r custom_components/espmmw_distance_card /config/custom_components/
```

重启 HA，再在「添加集成」中加入 **ESPMMW Distance Card**。

## 添加卡片

```yaml
type: custom:espmmw-distance-card
title: ESPMMW 雷达
presence: binary_sensor.espmmw_x_mmw
move_distance: sensor.espmmw_x_move_distance
move_energy: sensor.espmmw_x_move_energy
static_distance: sensor.espmmw_x_static_distance
static_energy: sensor.espmmw_x_static_energy
max_distance: 450
unit: cm
```

> 实体 ID 以 HA 里实际为准（ESPHome 常带设备名前缀）。距离单位为 **cm**，量程默认 450。

完整仪表盘片段见 [`dashboard.yaml`](./dashboard.yaml)。

## 资源 URL

集成会注册：

`/espmmw_distance_card/espmmw-distance-card.js?v=…`

无需再手动拷贝到 `/config/www/`。
