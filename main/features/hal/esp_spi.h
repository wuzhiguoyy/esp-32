#ifndef _HAL_ESP_SPI_H
#define _HAL_ESP_SPI_H

#include <cstddef>
#include <cstdint>

#include "features/hal/i_spi.h"

/**
 * ISpiBus 的 ESP-IDF 实现。
 */
class EspSpiBus : public ISpiBus {
public:
    EspSpiBus() = default;
    ~EspSpiBus() override;

    /**
     * 安装总线并挂载设备。
     *
     * @param host SPI1 / SPI2
     * @param sck  时钟
     * @param mosi 主出
     * @param miso 主入
     * @param cs   片选
     * @param hz   频率
     * @return 是否成功
     */
    bool Init(int host, int sck, int mosi, int miso, int cs, int hz);

    void Transfer(const uint8_t* tx, uint8_t* rx, size_t len) override;
    void SetCs(int level) override;

private:
    void* device_ = nullptr;
    int cs_pin_ = -1;
};

#endif // _HAL_ESP_SPI_H
