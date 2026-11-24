#ifndef _FEATURE_RFID_READER_H
#define _FEATURE_RFID_READER_H

#include "sdkconfig.h"

#ifndef CONFIG_ESP32_SHOP_RFID
#define CONFIG_ESP32_SHOP_RFID 0
#endif

#include <string>

#include "features/hal/esp_spi.h"
#include "features/rc522.h"

/**
 * RC522 读卡器设备封装：负责 SPI 总线安装与轮询。
 * 寄存器逻辑在 Rc522（纯逻辑，可单测）。
 */
class RfidReader {
public:
    static RfidReader& GetInstance();

    bool Init();

    /**
     * 等待刷卡。
     *
     * @param timeout_ms 超时毫秒
     * @return UID 十六进制字符串，超时返回空
     */
    std::string ReadUid(int timeout_ms);

private:
    RfidReader() = default;

    bool initialized_ = false;
    EspSpiBus bus_;
    Rc522* rc522_ = nullptr;
};

#endif // _FEATURE_RFID_READER_H
