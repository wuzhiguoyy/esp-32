# 快速上手（5 步）

> 适用于从 1688 等渠道购买开发板后，自行烧录道玄 AI 语音助手的用户。
> 不需要安装 ESP-IDF，不需要编译源码。

---

## 准备清单

| 项目 | 要求 |
| --- | --- |
| 开发板 | ESP32-S3 / ESP32-C3 等，Flash ≥ 4MB |
| 麦克风 + 喇叭 | 部分开发板已集成 |
| USB 数据线 | 能传数据，不是纯充电线 |
| 电脑系统 | Windows 10 / 11（64 位） |
| **Python** | **3.10 或更高**（烧录工具 esptool 需要） |
| esptool | 4.7 或更高 |

> 不想装 Python？看下方「免安装烧录」章节，用 esptool.exe 直接烧。

### 版本对照（照着装）

| 软件 | 版本 | 怎么查 |
| --- | --- | --- |
| Python | **3.10+** | `python --version` |
| esptool | **4.7+** | `esptool --version` |
| 串口驱动 | 最新 | 设备管理器能看到 COM 口 |

Python 低于 3.10 会报 `esptool requires Python >=3.10`，直接去 python.org 装 3.10 或 3.11。

### 硬件要求（按用途区分）

| 用途 | 需要什么 |
| --- | --- |
| 仅烧录 + 激活 | ESP32 开发板 + USB 数据线（串口看激活码） |
| 屏幕显示激活码 | 带屏板型 |
| 语音对话 | 麦克风 + 喇叭（或 I2S 功放模块） |
| 完全体验 | 带屏 + 麦克风 + 喇叭（如 BOX3、CoreS3） |

> 裸板也能激活：没有屏幕就用串口看激活码，激活后再接麦克风和喇叭即可对话。

---

## 第 1 步：下载固件

打开 [Releases](https://github.com/wuzhiguoyy/esp-32/releases) 页面，按你的板型下载：

```
bread-compact-wifi-esp32s3-v1.0.0.bin
esp32-s3-box-3-v1.0.0.bin
```

没有你的板型？看 [自定义开发板指南](custom-board_zh.md) 自行编译。

---

## 第 2 步：安装串口驱动

| 芯片 | 驱动 |
| --- | --- |
| CH340 | [CH341SER](http://www.wch.cn/download/CH341SER_EXE.html) |
| CP210x | [Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) |

装完插上板子，设备管理器里能看到 `COM3`、`COM4` 之类的端口。

---

## 第 3 步：一键烧录

### 安装 esptool（只需一次）

```bash
pip install esptool
```

没装 Python 的话，先装 **Python 3.10 或 3.11**（勾选 Add to PATH），再执行上面的命令。

装完确认：

```bash
python --version
esptool --version
```

### 免安装烧录（不想装 Python）

1. 下载 esptool 独立版：<https://github.com/espressif/esptool/releases>
   选 `esptool-v4.x.x-windows-amd64.zip`；
2. 解压得到 `esptool.exe`；
3. 把它放到 `flash.bat` 同目录；
4. 双击 `flash.bat`，脚本会自动识别。

这种方式不需要 Python，适合完全不懂技术的客户。

### 运行烧录脚本

1. 把 `flash.bat` 和固件 `.bin` 放在同一个文件夹；
2. 双击 `flash.bat`；
3. 按提示输入串口号，例如 `COM3`；
4. 板子进入下载模式（按住 BOOT → 点 RST → 松开 BOOT）；
5. 回车开始烧录。

看到 `[DONE] Flash OK` 即成功。

> 也可以直接把 `.bin` 拖到 `flash.bat` 上。

### 烧录失败怎么办

| 现象 | 解决 |
| --- | --- |
| 找不到串口 | 装驱动、换数据线 |
| `Failed to connect` | 重新进下载模式 |
| 高速失败 | 脚本会自动降速重试 |
| 校验失败 | 换 USB 口，别用扩展坞 |

---

## 第 4 步：配网

1. 烧录完成，按一下 RST 重启；
2. 手机搜索 Wi-Fi，连接 `Daoxuan-xxxx`；
3. 浏览器打开 `192.168.4.1`；
4. 选择你家 2.4G Wi-Fi，输入密码；
5. 保存，设备自动重启。

> 部分板子支持蓝牙配网，按屏幕提示操作。

---

## 第 5 步：获取激活码并激活

激活码由**服务器在设备联网后生成**，三种方式任选其一：

### 方式 A：屏幕显示（带屏板）

设备联网后屏幕直接显示 6 位数字，例如 `123456`。

### 方式 B：串口查看（所有板子，推荐）

无屏板必须用这种方式。

**使用 flash.bat 的客户**：

烧录完成后脚本会自动打开串口监视器，等设备连上 Wi-Fi，会看到：

```
==================================================
 ACTIVATION CODE: 123456
 Open the website, register, then enter this code.
==================================================
```

**手动查看**：

| 工具 | 操作 |
| --- | --- |
| PuTTY | 串口 `COM3`，波特率 `115200`，连接 |
| XCOM / SSCOM | 选串口，115200，打开 |
| ESP-IDF | `idf.py -p COM3 monitor` |

串口打开后按一下板子 RST，等几秒即可看到激活码。

### 方式 C：语音播报（有喇叭的板子）

部分板型会在激活时语音提示，按提示操作即可。

### 激活

拿到 6 位码后：

1. 打开网站注册账号；
2. 登录后进入「我的设备」；
3. 输入激活码；
4. 绑定成功，设备自动激活。

激活后设备就可以语音对话。

> **没有激活码？** 确认设备已连上 Wi-Fi，并在串口里搜索关键字 `ACTIVATION CODE`。

---

## 常见问题

**Q：必须用你们的服务器吗？**
是。固件内置官方服务地址，负责语音识别、大模型对话和升级。

**Q：可以自己部署服务器吗？**
可以，需要实现 OTA 与 WebSocket 协议，详见 [OTA 文档](ota_zh.md) 和 [WebSocket 文档](websocket_zh.md)。

**Q：没有声音？**
检查喇叭接线、板型是否选对。烧错板型会导致音频引脚不对。

**Q：能换自己的大模型 Key 吗？**
可以。登录后在我的设备 → 模型 Key 里填写。

---

## 需要帮助

- 提交 [Issue](https://github.com/wuzhiguoyy/esp-32/issues)
- 国内镜像：[Gitee](https://gitee.com/wuzhiguo/esp-32)
