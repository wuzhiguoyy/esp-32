# 固件发布流程

> 面向项目维护者：把固件编译成 bin 并发布到 GitHub Releases，供客户下载。

---

## 一、准备环境

| 项 | 要求 |
| --- | --- |
| ESP-IDF | v6.1（最低 v6.0.1） |
| 终端 | ESP-IDF PowerShell / CMD |
| 源码 | 本仓库 |

打开 **ESP-IDF PowerShell**，`cd` 到仓库根目录。

---

## 二、编译一个板型

```bash
idf.py set-target esp32s3
idf.py menuconfig      # Xiaozhi Assistant → Board Type 选择板型
idf.py build
idf.py merge-bin
```

产物：`build/merged-binary.bin`

---

## 三、用脚本批量打包

```bat
tools\build-release.bat bread-compact-wifi esp32s3 1.0.0
```

输出：

```
release\bread-compact-wifi-esp32s3-v1.0.0.bin
```

### 建议首批发布板型

| 板型 | 芯片 | 文件名 |
| --- | --- | --- |
| bread-compact-wifi | esp32s3 | `bread-compact-wifi-esp32s3-v1.0.0.bin` |
| esp32-s3-box-3 | esp32s3 | `esp32-s3-box-3-esp32s3-v1.0.0.bin` |
| m5stack-core-s3 | esp32s3 | `m5stack-core-s3-esp32s3-v1.0.0.bin` |
| xmini-c3 | esp32c3 | `xmini-c3-esp32c3-v1.0.0.bin` |

---

## 四、发布到 GitHub Releases

### 方式 A：网页上传（简单）

1. 打开 `https://github.com/wuzhiguoyy/esp-32/releases/new`
2. Tag 填 `v1.0.0`，标题填 `v1.0.0`
3. 说明里写更新内容
4. 上传 `release/*.bin`
5. 点 **Publish release**

### 方式 B：命令行（gh）

```bash
gh release create v1.0.0 release/*.bin \
  --title "v1.0.0" \
  --notes "首个正式版本：支持 OTA 激活、语音对话、MCP 控制"
```

没有 gh 就装：

```bash
winget install GitHub.cli
gh auth login
```

---

## 五、发布说明模板

```markdown
## v1.0.0

### 新增
- OTA 激活与设备绑定
- WebSocket 实时语音对话
- MCP 设备控制
- 管理后台

### 支持板型
- bread-compact-wifi（ESP32-S3）
- esp32-s3-box-3（ESP32-S3）
- m5stack-core-s3（ESP32-S3）

### 烧录
下载对应 bin，使用 tools/flash.bat 一键烧录，烧录地址 0x0。
```

---

## 六、客户下载入口

README 与快速上手文档已经指向：

```
https://github.com/wuzhiguoyy/esp-32/releases
```

Gitee 用户也可以在 Gitee 的「发行版」里上传同一批 bin。

---

## 七、版本号规范

| 变更类型 | 版本 |
| --- | --- |
| 修复 bug | 1.0.1 |
| 新增功能 | 1.1.0 |
| 不兼容变更 | 2.0.0 |

---

## 八、发布后检查

- [ ] Releases 页面能看到 bin
- [ ] 文件名带板型与芯片
- [ ] 下载后用 `flash.bat` 能烧进去
- [ ] 设备能激活并对话
- [ ] Gitee 同步上传一份
