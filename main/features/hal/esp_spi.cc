#include "esp_spi.h"

#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>

#define TAG "EspSpi"

EspSpiBus::~EspSpiBus() {
    if (device_ != nullptr) {
        spi_bus_remove_device(static_cast<spi_device_handle_t>(device_));
        device_ = nullptr;
    }
}

bool EspSpiBus::Init(int host, int sck, int mosi, int miso, int cs, int hz) {
    spi_bus_config_t bus = {};
    bus.mosi_io_num = mosi;
    bus.miso_io_num = miso;
    bus.sclk_io_num = sck;
    bus.quadwp_io_num = -1;
    bus.quadhd_io_num = -1;
    bus.max_transfer_sz = 32;

    spi_host_device_t spi_host = static_cast<spi_host_device_t>(host);
    if (spi_bus_initialize(spi_host, &bus, SPI_DMA_CH_AUTO) != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_initialize failed");
        return false;
    }

    spi_device_interface_config_t dev = {};
    dev.clock_speed_hz = hz;
    dev.mode = 0;
    dev.spics_io_num = -1;  // 片选由软件控制
    dev.queue_size = 4;

    spi_device_handle_t handle = nullptr;
    if (spi_bus_add_device(spi_host, &dev, &handle) != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_add_device failed");
        return false;
    }
    device_ = handle;
    cs_pin_ = cs;

    gpio_config_t io = {};
    io.pin_bit_mask = (1ULL << cs);
    io.mode = GPIO_MODE_OUTPUT;
    io.pull_up_en = GPIO_PULLUP_DISABLE;
    io.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io);
    gpio_set_level(static_cast<gpio_num_t>(cs), 1);

    return true;
}

void EspSpiBus::SetCs(int level) {
    if (cs_pin_ >= 0) {
        gpio_set_level(static_cast<gpio_num_t>(cs_pin_), level);
    }
}

void EspSpiBus::Transfer(const uint8_t* tx, uint8_t* rx, size_t len) {
    if (device_ == nullptr || tx == nullptr || len == 0) {
        return;
    }
    SetCs(0);
    spi_transaction_t t = {};
    t.length = 8 * len;
    t.tx_buffer = tx;
    t.rx_buffer = rx;
    spi_device_transmit(static_cast<spi_device_handle_t>(device_), &t);
    SetCs(1);
}
