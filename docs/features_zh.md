# 扩展能力：定位 / 录音 / 摄像头 / 防盗

> 所有扩展能力都是**可选编译项**，默认关闭，老板型行为不变。

---

## 一、能力矩阵

| 能力 | 固件 | 服务端 | 状态 |
| --- | --- | --- | --- |
| 语音对话 | ✅ | ✅ | 已有 |
| OTA 激活 | ✅ | ✅ | 已有 |
| **GPS 定位上报** | ✅ 新增 | ✅ 新增 | 可用 |
| 电子围栏 / 防盗告警 | ⏳ 服务端算 | ✅ 可加 | 二期 |
| 拍照上传 | ⚠️ 板型相关 | ⏳ 待做 | 二期 |
| 录音存证 | ⏳ 待做 | ⏳ 待做 | 三期 |
| 实时视频 | ❌ 不适合 ESP32 | — | 不做 |

---

## 二、GPS 定位（已实现）

### 1. 硬件接线

| GPS 模块 | ESP32 |
| --- | --- |
| VCC | 3.3V / 5V（按模块） |
| GND | GND |
| TX | GPIO 4（GPS_RX） |
| RX | 可不接（只读） |

推荐模块：ATGM336H、NEO-6M、NEO-M8N。

### 2. 编译开关

```bash
idf.py menuconfig
# Xiaozhi Assistant → Device Features (optional)
```

| 配置 | 说明 |
| --- | --- |
| `ESP32_FEATURE_GPS` | 打开定位 |
| `ESP32_GPS_UART_NUM` | UART 口（默认 1） |
| `ESP32_GPS_RX_GPIO` | GPS TX 接的引脚（默认 4） |
| `ESP32_GPS_BAUD` | 波特率（默认 9600） |
| `ESP32_LOCATION_INTERVAL_SEC` | 上报间隔（默认 30 秒） |

### 3. 上报接口

```
POST /api/xiaozhi/device/location
```

```json
{
  "device_id": "aa:bb:cc:dd:ee:01",
  "client_id": "uuid",
  "lat": 31.2304,
  "lng": 121.4737,
  "speed": 12.5,
  "utc": "082345.00"
}
```

服务端校验设备已激活后写入 `esp32_location`。

### 4. 建表

```sql
CREATE TABLE `esp32_location` (
  `id`          BIGINT       NOT NULL AUTO_INCREMENT,
  `device_id`   VARCHAR(64)  NOT NULL COMMENT '设备MAC',
  `client_id`   VARCHAR(64)  NOT NULL COMMENT 'Client-Id',
  `lat`         DECIMAL(10,6) DEFAULT NULL,
  `lng`         DECIMAL(10,6) DEFAULT NULL,
  `speed`       DECIMAL(6,2)  DEFAULT NULL COMMENT '节',
  `utc_time`    VARCHAR(16)  DEFAULT NULL,
  `report_time` DATETIME     DEFAULT NULL,
  `create_time` DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `idx_device_time` (`device_id`, `report_time`),
  KEY `idx_client` (`client_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='设备定位';
```

### 5. 串口日志

```
I (12345) Gps: GPS started on UART1 RX=4 baud=9600
I (54321) Location: Location reported: 31.230400, 121.473700
```

---

## 三、防盗（二期）

依赖定位能力，服务端判断：

| 规则 | 说明 |
| --- | --- |
| 电子围栏 | 离开设定半径 → 记录告警 |
| 移动告警 | ACC 断电后仍有位移 → 告警 |
| 离线告警 | 超过 N 分钟无上报 → 告警 |

固件不需要改，服务端加规则表 + 定时任务。

---

## 四、摄像头（已实现上传）

| 项 | 说明 |
| --- | --- |
| 固件 | `features/camera_uploader.cc`：抓拍 JPEG → multipart 上传 |
| 接口 | `POST /api/esp32/file/upload`（Client-Id 识别设备） |
| 存储 | OSS，记录写入 `esp32_file`（biz_type=PHOTO） |
| MCP 工具 | `self.camera.upload_photo`，返回图片地址 |
| 前提 | 板型摄像头输出 `PIXFORMAT_JPEG` |

抓拍流程：

```
MCP 调用 self.camera.upload_photo
→ camera->CaptureJpeg()
→ multipart POST 到 /api/esp32/file/upload
→ 返回 { fileId, url }
```

### 实时视频

ESP32 不适合连续视频：内存、带宽、功耗都不够。

替代方案：

| 方案 | 说明 |
| --- | --- |
| 定时抓拍 | 每 5～30 秒上传一张，后台按时间轴看 |
| 低帧率 MJPEG | 1～3 帧/秒，仅局域网可用 |

## 五、录音（未实现，方案已定）

| 项 | 说明 |
| --- | --- |
| 现状 | 语音是实时流，识别完即丢弃，不落盘 |
| 服务端 | `esp32_file` 与上传接口已就绪（biz_type=RECORD） |
| 缺口 | 设备端没有“录制到文件”的入口 |
| 方案 A | 固件把麦克风 PCM 写 SD 卡，再上传 |
| 方案 B | 服务端把一轮对话的 Opus 帧封装成 Ogg 后上传 |

推荐方案 B：不用改音频主流程，先让每轮对话自动存证，再评估是否需要连续录音。

## 六、合规提醒

录音、定位、拍摄涉及他人隐私：

- 录制他人对话需告知；
- 定位设备应限于自有资产；
- 取证用途注意证据链与当地法律。

## 七、兼容性

| 场景 | 是否受影响 |
| --- | --- |
| 老固件（无 GPS） | 不受影响，继续用 |
| 老设备不升级 | 服务端兼容，位置为空 |
| 新板带 GPS | 需在 menuconfig 打开并配置引脚 |
| 服务端 | 新增表和接口，不改老表 |

---

## 七、无人超市外设（固件已加）

### 7.1 电磁锁 / 继电器

| 配置 | 说明 |
| --- | --- |
| `ESP32_SHOP_LOCK` | 打开门锁控制 |
| `ESP32_LOCK_GPIO` | 继电器 IN 接的 GPIO（默认 5） |
| `ESP32_LOCK_ACTIVE_LEVEL` | 高电平触发填 1，低电平填 0 |

MCP 工具：

| 工具 | 作用 |
| --- | --- |
| `self.lock.open` | 开锁 |
| `self.lock.close` | 关锁 |
| `self.lock.status` | 查询状态 |

> 继电器必须独立供电，ESP32 只出控制信号，否则烧板。

### 7.2 HX711 称重

| 配置 | 说明 |
| --- | --- |
| `ESP32_SHOP_WEIGHT` | 打开称重 |
| `ESP32_HX711_DOUT_GPIO` | DT 引脚（默认 6） |
| `ESP32_HX711_SCK_GPIO` | SCK 引脚（默认 7） |

MCP 工具：

| 工具 | 作用 |
| --- | --- |
| `self.weight.read` | 返回克数 |
| `self.weight.tare` | 去皮归零 |

校准：

```
1. 空载 → self.weight.tare
2. 放 500g 砝码 → 代码里调用 Calibrate(500)
```

### 7.3 RC522 RFID 刷卡

| 配置 | 说明 |
| --- | --- |
| `ESP32_SHOP_RFID` | 打开读卡 |
| `ESP32_RC522_*_GPIO` | CS/SCK/MOSI/MISO/RST |
| `ESP32_RC522_SPI_HOST` | SPI1 或 SPI2 |

MCP 工具：

| 工具 | 作用 |
| --- | --- |
| `self.rfid.read_uid` | 等待刷卡，返回 UID |

> RC522 只能接 3.3V，5V 会烧。

### 7.4 扫码枪 / 小票打印

| 外设 | 说明 |
| --- | --- |
| USB 扫码枪 | ESP32-S3 需要 USB Host 组件，固件暂未集成 |
| 串口扫码枪 | 可以按 GPS 的方式加 UART 驱动，后续实现 |
| 热敏打印机 | 需串口驱动 + ESC/POS 指令，后续实现 |

## 八、无人超市完整链路还差什么

| 环节 | 现状 |
| --- | --- |
| 语音提示 | ✅ |
| 开门 | ✅ 固件已支持 |
| 称重 | ✅ 固件已支持 |
| 刷卡 | ✅ 固件已支持 |
| 商品识别 | ❌ 需摄像头 + 服务端识别 |
| 下单/支付 | ❌ 需服务端订单系统 |
| 关门结算 | ❌ 需服务端订单系统 |

固件只负责“感知 + 执行”，交易逻辑必须在服务端。
