#include "gps.h"

#include "features/nmea_parser.h"

#if CONFIG_ESP32_FEATURE_GPS

#include <driver/uart.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TAG "Gps"

Gps& Gps::GetInstance() {
    static Gps instance;
    return instance;
}

bool Gps::Start() {
    if (started_) {
        return true;
    }

    uart_config_t config = {};
    config.baud_rate = CONFIG_ESP32_GPS_BAUD;
    config.data_bits = UART_DATA_8_BITS;
    config.parity = UART_PARITY_DISABLE;
    config.stop_bits = UART_STOP_BITS_1;
    config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    config.source_clk = UART_SCLK_DEFAULT;

    uart_port_t port = static_cast<uart_port_t>(CONFIG_ESP32_GPS_UART_NUM);

    esp_err_t err = uart_driver_install(port, 1024, 0, 0, nullptr, 0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "uart_driver_install failed: %s", esp_err_to_name(err));
        return false;
    }
    uart_param_config(port, &config);
    uart_set_pin(port, CONFIG_ESP32_GPS_TX_GPIO, CONFIG_ESP32_GPS_RX_GPIO,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    xTaskCreate([](void* arg) {
        static_cast<Gps*>(arg)->TaskLoop();
    }, "gps_task", 4096, this, 4, nullptr);

    started_ = true;
    ESP_LOGI(TAG, "GPS started on UART%d RX=%d baud=%d",
             CONFIG_ESP32_GPS_UART_NUM, CONFIG_ESP32_GPS_RX_GPIO, CONFIG_ESP32_GPS_BAUD);
    return true;
}

void Gps::TaskLoop() {
    uart_port_t port = static_cast<uart_port_t>(CONFIG_ESP32_GPS_UART_NUM);
    char line[160];
    int index = 0;
    uint8_t byte = 0;

    while (true) {
        int read = uart_read_bytes(port, &byte, 1, pdMS_TO_TICKS(1000));
        if (read <= 0) {
            continue;
        }
        if (byte == '\n' || byte == '\r') {
            if (index > 0) {
                line[index] = '\0';
                FeedLine(line);
                index = 0;
            }
            continue;
        }
        if (index < static_cast<int>(sizeof(line)) - 1) {
            line[index++] = static_cast<char>(byte);
        } else {
            index = 0;
        }
    }
}

void Gps::FeedLine(const char* line) {
    NmeaFix fix;
    if (!NmeaParser::Parse(line, &fix)) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    utc_ = fix.utc_time;
    fixed_ = fix.valid;
    if (fix.valid) {
        lat_ = fix.latitude;
        lng_ = fix.longitude;
        speed_knots_ = fix.speed_knots;
    }
}

bool Gps::IsFixed() {
    std::lock_guard<std::mutex> lock(mutex_);
    return fixed_;
}

double Gps::Latitude() {
    std::lock_guard<std::mutex> lock(mutex_);
    return lat_;
}

double Gps::Longitude() {
    std::lock_guard<std::mutex> lock(mutex_);
    return lng_;
}

double Gps::SpeedKnots() {
    std::lock_guard<std::mutex> lock(mutex_);
    return speed_knots_;
}

std::string Gps::UtcTime() {
    std::lock_guard<std::mutex> lock(mutex_);
    return utc_;
}

#else

Gps& Gps::GetInstance() {
    static Gps instance;
    return instance;
}

bool Gps::Start() { return false; }
bool Gps::IsFixed() { return false; }
double Gps::Latitude() { return 0.0; }
double Gps::Longitude() { return 0.0; }
double Gps::SpeedKnots() { return 0.0; }
std::string Gps::UtcTime() { return ""; }
void Gps::FeedLine(const char*) {}

#endif // CONFIG_ESP32_FEATURE_GPS
