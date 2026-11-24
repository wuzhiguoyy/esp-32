#ifndef _HAL_ESP_GPIO_H
#define _HAL_ESP_GPIO_H

#include "features/hal/i_gpio.h"

/**
 * IGpio 的 ESP-IDF 实现。
 */
class EspGpio : public IGpio {
public:
    static EspGpio* Get();

    void ConfigureOutput(int pin) override;
    void ConfigureInput(int pin, bool pull_up) override;
    void SetLevel(int pin, int level) override;
    int GetLevel(int pin) override;

private:
    EspGpio() = default;
};

#endif // _HAL_ESP_GPIO_H
