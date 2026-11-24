# 快速上手（5 步）

> 适用于从 1688 等渠道购买开发板后，自行烧录道玄 AI 语音助手的用户。
> 不需要安装 ESP-IDF，不需要编译源码。

---

## 准备清单

| 项目 | 说明 |
| --- | --- |
| 开发板 | ESP32-S3 / ESP32-C3 等，Flash ≥ 4MB |
| 麦克风 + 喇叭 | 部分开发板已集成 |
| USB 数据线 | 能传数据，不是纯充电线 |
| 电脑 | Windows 10/11 |

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

没装 Python 的话，装一个 3.10+ 版本。

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

## 第 5 步：激活

1. 设备联网后，屏幕显示 **6 位激活码**；
2. 打开我们的用户页面注册/登录；
3. 输入激活码；
4. 绑定成功，设备自动激活。

激活后即可使用：

```
说“你好小智”唤醒 → 直接说话 → 设备回答
```

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
