# 宿主机单元测试

纯逻辑模块不依赖 ESP-IDF，可以在电脑上直接编译运行。

## 覆盖模块

| 测试 | 被测代码 | 内容 |
| --- | --- | --- |
| `test_nmea` | `main/features/nmea_parser.cc` | RMC/GGA 解析、无效定位、南北纬、异常输入 |
| `test_weight` | `main/features/weight_math.cc` | 皮重、校准、比例非法 |
| `test_door` | `main/features/door_lock_logic.cc` | 高/低电平触发、开锁关锁、空 GPIO |
| `test_location` | `main/features/location_payload.cc` | 报文结构、负坐标、字符转义 |

## 运行

需要 CMake 和 C++17 编译器。

```bash
cd tests/host
cmake -S . -B build
cmake --build build
cd build
ctest --output-on-failure
```

期望输出：

```
1/2 Test #1: nmea_parser ..... Passed
2/2 Test #2: weight_math ..... Passed
```

也可以直接运行：

```bash
./build/test_nmea
./build/test_weight
```

## 设计原则

| 层 | 是否依赖硬件 | 能否单测 |
| --- | --- | --- |
| `nmea_parser` | 否 | ✅ 宿主机 |
| `weight_math` | 否 | ✅ 宿主机 |
| `door_lock_logic` | 否（IGpio 注入） | ✅ 宿主机 |
| `location_payload` | 否 | ✅ 宿主机 |
| `gps` | UART | 需目标板 |
| `weight_sensor` | GPIO | 需目标板 |
| `rfid_reader` | SPI | 需目标板（接口已抽出） |
| `location_reporter` | HTTP | 需目标板（接口已抽出） |

新增功能时把计算、解析、状态机放纯逻辑层，硬件访问留在驱动层，测试只覆盖纯逻辑层。
