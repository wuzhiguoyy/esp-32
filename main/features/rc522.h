#ifndef _FEATURE_RC522_H
#define _FEATURE_RC522_H

#include <cstdint>
#include <string>

#include "features/hal/i_spi.h"

/**
 * RC522 寄存器级驱动（纯 C++，SPI 通过 ISpiBus 注入）。
 *
 * 不含 ESP-IDF 依赖，可在 PC 上用假 SPI 总线做单元测试。
 */
class Rc522 {
public:
    explicit Rc522(ISpiBus* bus) : bus_(bus) {}

    /** 初始化寄存器（不负责 SPI 总线安装） */
    bool Init();

    /** 读寄存器 */
    uint8_t ReadRegister(uint8_t address);

    /** 写寄存器 */
    void WriteRegister(uint8_t address, uint8_t value);

    /** 寻卡 + 防冲突，成功返回 4 字节 UID 的十六进制字符串 */
    bool ReadUidOnce(std::string* uid);

private:
    void SetBitMask(uint8_t reg, uint8_t mask);
    void ClearBitMask(uint8_t reg, uint8_t mask);
    void AntennaOn();
    bool WaitForIrq(int max_loops);
    bool RequestCard();
    bool AntiCollision(std::string* uid);

    ISpiBus* bus_;
};

#endif // _FEATURE_RC522_H
