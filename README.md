# 道玄 AI 语音助手 · ESP32 固件

**Daoxuan AI Voice Assistant — ESP32 Firmware**

> 面向客户与渠道的完整使用说明（中文）：[README_zh.md](README_zh.md)

## 简介

道玄 AI 语音助手是运行在 ESP32 系列芯片上的 AI 语音产品，支持离线唤醒、语音对话、设备控制与远程升级。

固件内置官方服务地址，刷入后连接 Wi-Fi 即可使用，无需自建服务器。

支持芯片：ESP32 / ESP32-C3 / ESP32-C5 / ESP32-C6 / ESP32-S3 / ESP32-P4。

## 快速开始

```bash
git clone https://github.com/wuzhiguoyy/esp-32.git
cd esp-32

# 选择板型
idf.py menuconfig        # Xiaozhi Assistant -> Board Type

# 设置芯片
idf.py set-target esp32s3

# 编译
idf.py build

# 烧录并查看日志
idf.py -p COM3 flash monitor
```

详细步骤、量产烧录、激活流程与常见问题，见 **[README_zh.md](README_zh.md)**。

## 管理后台

仓库内置 `vue-ai/` 设备管理后台（Vue 3 + Vite + Element Plus），支持设备绑定、套餐订单、固件发布与操作审计。

```bash
cd vue-ai
npm install
npm run dev
```

接口走 `/api/admin/esp32/**`，需要 JWT。

已内置 GitHub Pages 自动部署工作流，配置 `VITE_API_BASE` Secret 后推送即可发布。

详细说明见 **[README_zh.md 第十四节](README_zh.md)**。

## 硬件形态

| 目录 | 含义 |
| --- | --- |
| `docs/v0/` | 原型验证：面包板 + 杜邦线 + 开发板 + 麦克风 + 喇叭 |
| `docs/v1/` | 成品形态：带外壳或成品的开发板，适合产品选型与量产 |

<table>
<tr>
<td align="center"><img src="docs/v0/ESP32-BreadBoard.jpg" width="220"><br>v0 面包板原型</td>
<td align="center"><img src="docs/v1/espbox3.jpg" width="220"><br>v1 成品开发板</td>
</tr>
</table>

完整板卡图片与选型说明见 **[README_zh.md](README_zh.md#51-两种硬件形态v0-与-v1)**。

## 重要说明

- 固件内置官方服务地址（`main/ota.cc` 中的 `GetDefaultOtaUrl()`）；
- 请勿修改该地址：修改后设备将无法激活、对话与升级；
- 开源范围为设备端固件代码；服务器、语音识别、大模型与语音合成属于官方商业服务。

## 许可证

本项目基于 MIT 协议的开源 ESP32 语音框架二次开发，原始版权归原作者所有。

`LICENSE` 文件必须保留。品牌与发行由上海道玄微数信息科技有限公司维护。
