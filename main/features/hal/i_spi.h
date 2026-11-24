#ifndef _HAL_I_SPI_H
#define _HAL_I_SPI_H

#include <cstddef>
#include <cstdint>

/**
 * SPI 抽象接口（纯 C++）。
 *
 * 目的：RC522 等 SPI 外设依赖接口，便于用假总线单测寄存器时序。
 */
class ISpiBus {
public:
    virtual ~ISpiBus() = default;

    /**
     * 全双工传输。
     *
     * @param tx  发送缓冲
     * @param rx  接收缓冲，可为空
     * @param len 字节数
     */
    virtual void Transfer(const uint8_t* tx, uint8_t* rx, size_t len) = 0;

    /** 片选控制 */
    virtual void SetCs(int level) = 0;
};

#endif // _HAL_I_SPI_H
