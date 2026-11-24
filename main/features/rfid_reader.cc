#include "rfid_reader.h"

#if CONFIG_ESP32_SHOP_RFID

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TAG "Rfid"

RfidReader& RfidReader::GetInstance() {
    static RfidReader instance;
    return instance;
}

bool RfidReader::Init() {
    if (initialized_) {
        return true;
    }

    if (!bus_.Init(CONFIG_ESP32_RC522_SPI_HOST,
                   CONFIG_ESP32_RC522_SCK_GPIO,
                   CONFIG_ESP32_RC522_MOSI_GPIO,
                   CONFIG_ESP32_RC522_MISO_GPIO,
                   CONFIG_ESP32_RC522_CS_GPIO,
                   4 * 1000 * 1000)) {
        return false;
    }

    rc522_ = new Rc522(&bus_);
    if (!rc522_->Init()) {
        ESP_LOGE(TAG, "RC522 init failed");
        return false;
    }

    initialized_ = true;
    ESP_LOGI(TAG, "RC522 ready (CS=%d SCK=%d MOSI=%d MISO=%d)",
             CONFIG_ESP32_RC522_CS_GPIO, CONFIG_ESP32_RC522_SCK_GPIO,
             CONFIG_ESP32_RC522_MOSI_GPIO, CONFIG_ESP32_RC522_MISO_GPIO);
    return true;
}

std::string RfidReader::ReadUid(int timeout_ms) {
    if (!Init()) {
        return "";
    }

    int waited = 0;
    while (waited < timeout_ms) {
        std::string uid;
        if (rc522_->ReadUidOnce(&uid)) {
            ESP_LOGI(TAG, "Card UID: %s", uid.c_str());
            return uid;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
        waited += 100;
    }
    return "";
}

#else

RfidReader& RfidReader::GetInstance() {
    static RfidReader instance;
    return instance;
}

bool RfidReader::Init() { return false; }
std::string RfidReader::ReadUid(int) { return ""; }

#endif // CONFIG_ESP32_SHOP_RFID
