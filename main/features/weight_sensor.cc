#include "weight_sensor.h"

#if CONFIG_ESP32_SHOP_WEIGHT

#include <algorithm>
#include <vector>

#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_rom_sys.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "features/weight_math.h"

#define TAG "Weight"

#define HX711_DOUT CONFIG_ESP32_HX711_DOUT_GPIO
#define HX711_SCK  CONFIG_ESP32_HX711_SCK_GPIO

WeightSensor& WeightSensor::GetInstance() {
    static WeightSensor instance;
    return instance;
}

bool WeightSensor::Init() {
    if (initialized_) {
        return true;
    }
    gpio_config_t out = {};
    out.pin_bit_mask = (1ULL << HX711_SCK);
    out.mode = GPIO_MODE_OUTPUT;
    out.pull_up_en = GPIO_PULLUP_DISABLE;
    out.pull_down_en = GPIO_PULLDOWN_DISABLE;
    out.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&out);

    gpio_config_t in = {};
    in.pin_bit_mask = (1ULL << HX711_DOUT);
    in.mode = GPIO_MODE_INPUT;
    in.pull_up_en = GPIO_PULLUP_ENABLE;
    in.pull_down_en = GPIO_PULLDOWN_DISABLE;
    in.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&in);

    gpio_set_level(static_cast<gpio_num_t>(HX711_SCK), 0);
    initialized_ = true;
    ESP_LOGI(TAG, "HX711 ready DOUT=%d SCK=%d", HX711_DOUT, HX711_SCK);
    return true;
}

bool WeightSensor::IsReady() {
    return Init() && gpio_get_level(static_cast<gpio_num_t>(HX711_DOUT)) == 0;
}

long WeightSensor::ReadRaw() {
    if (!Init()) {
        return 0;
    }

    // 等待转换完成：DOUT 拉低表示数据就绪
    int guard = 1000;
    while (gpio_get_level(static_cast<gpio_num_t>(HX711_DOUT)) == 1 && guard-- > 0) {
        esp_rom_delay_us(100);
        vTaskDelay(1);
    }
    if (guard <= 0) {
        ESP_LOGW(TAG, "HX711 timeout");
        return 0;
    }

    long value = 0;
    for (int i = 0; i < 24; i++) {
        gpio_set_level(static_cast<gpio_num_t>(HX711_SCK), 1);
        esp_rom_delay_us(1);
        value = (value << 1) | gpio_get_level(static_cast<gpio_num_t>(HX711_DOUT));
        gpio_set_level(static_cast<gpio_num_t>(HX711_SCK), 0);
        esp_rom_delay_us(1);
    }

    // 第 25 个脉冲选择增益 128（下一次转换）
    gpio_set_level(static_cast<gpio_num_t>(HX711_SCK), 1);
    esp_rom_delay_us(1);
    gpio_set_level(static_cast<gpio_num_t>(HX711_SCK), 0);

    // 24 位补码转有符号
    if (value & 0x800000) {
        value |= ~0xFFFFFFL;
    }
    return value;
}

void WeightSensor::Tare() {
    std::vector<long> samples;
    for (int i = 0; i < 10; i++) {
        samples.push_back(ReadRaw());
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    std::sort(samples.begin(), samples.end());
    offset_ = samples[samples.size() / 2];
    ESP_LOGI(TAG, "Tare offset=%ld", offset_);
}

void WeightSensor::Calibrate(float known_grams) {
    if (known_grams <= 0) {
        return;
    }
    std::vector<long> samples;
    for (int i = 0; i < 10; i++) {
        samples.push_back(ReadRaw());
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    std::sort(samples.begin(), samples.end());
    long raw = samples[samples.size() / 2] - offset_;
    if (raw == 0) {
        ESP_LOGW(TAG, "Calibrate failed: raw is zero");
        return;
    }
    scale_ = weight_math::CalculateScale(samples[samples.size() / 2], offset_, known_grams);
    ESP_LOGI(TAG, "Calibrated scale=%.3f (raw=%ld, grams=%.1f)", scale_, raw, known_grams);
}

float WeightSensor::ReadGrams() {
    if (!Init()) {
        return 0.0f;
    }
    long raw = ReadRaw();
    return weight_math::ToGrams(raw, offset_, scale_);
}

#else

WeightSensor& WeightSensor::GetInstance() {
    static WeightSensor instance;
    return instance;
}

bool WeightSensor::Init() { return false; }
long WeightSensor::ReadRaw() { return 0; }
void WeightSensor::Tare() {}
void WeightSensor::Calibrate(float) {}
float WeightSensor::ReadGrams() { return 0.0f; }
bool WeightSensor::IsReady() { return false; }

#endif // CONFIG_ESP32_SHOP_WEIGHT
