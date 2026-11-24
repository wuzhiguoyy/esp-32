#ifndef _HAL_I_GPIO_H
#define _HAL_I_GPIO_H

/**
 * GPIO 抽象接口（纯 C++，不依赖 ESP-IDF）。
 *
 * 目的：门锁、称重等驱动依赖接口而不是 gpio_set_level，
 * 这样在 PC 上可以用假实现做单元测试。
 */
class IGpio {
public:
    virtual ~IGpio() = default;

    /** 配置为输出 */
    virtual void ConfigureOutput(int pin) = 0;

    /** 配置为输入，可带上拉 */
    virtual void ConfigureInput(int pin, bool pull_up) = 0;

    /** 输出电平：0 或 1 */
    virtual void SetLevel(int pin, int level) = 0;

    /** 读取电平：0 或 1 */
    virtual int GetLevel(int pin) = 0;
};

#endif // _HAL_I_GPIO_H
