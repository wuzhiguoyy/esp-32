#include "esp_gpio.h"

#include <driver/gpio.h>

EspGpio* EspGpio::Get() {
    static EspGpio instance;
    return &instance;
}

void EspGpio::ConfigureOutput(int pin) {
    gpio_config_t io = {};
    io.pin_bit_mask = (1ULL << pin);
    io.mode = GPIO_MODE_OUTPUT;
    io.pull_up_en = GPIO_PULLUP_DISABLE;
    io.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io);
}

void EspGpio::ConfigureInput(int pin, bool pull_up) {
    gpio_config_t io = {};
    io.pin_bit_mask = (1ULL << pin);
    io.mode = GPIO_MODE_INPUT;
    io.pull_up_en = pull_up ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    io.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io);
}

void EspGpio::SetLevel(int pin, int level) {
    gpio_set_level(static_cast<gpio_num_t>(pin), level);
}

int EspGpio::GetLevel(int pin) {
    return gpio_get_level(static_cast<gpio_num_t>(pin));
}
