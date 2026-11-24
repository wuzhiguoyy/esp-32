# Tools

| 文件 | 用途 |
| --- | --- |
| `flash.bat` | Windows 一键烧录脚本，供客户使用 |

## 使用方式

1. 安装 Python 3.10+ 与 esptool：

```bash
pip install esptool
```

2. 把 `flash.bat` 和固件 `merged-binary.bin` 放在同一目录；
3. 双击 `flash.bat`，按提示输入串口号。

支持把 `.bin` 直接拖到 `flash.bat` 上。

## 固件从哪来

在 [Releases](https://github.com/wuzhiguoyy/esp-32/releases) 下载对应板型的 `merged-binary.bin`。

自行编译：

```bash
idf.py set-target esp32s3
idf.py menuconfig   # 选择板型
idf.py build
idf.py merge-bin
```

产物位于 `build/merged-binary.bin`，烧录地址 `0x0`。

## 详细教程

见 [docs/quickstart_zh.md](../docs/quickstart_zh.md)。
