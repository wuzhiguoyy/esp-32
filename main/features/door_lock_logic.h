#ifndef _FEATURE_DOOR_LOCK_LOGIC_H
#define _FEATURE_DOOR_LOCK_LOGIC_H

#include "features/hal/i_gpio.h"

/**
 * 门锁逻辑（纯 C++，不依赖 ESP-IDF）。
 *
 * 只做电平状态管理，实际 GPIO 由 IGpio 注入，
 * 因此可以在 PC 上用假 GPIO 验证开锁/关锁行为。
 */
class DoorLockLogic {
public:
    /**
     * @param gpio        GPIO 实现
     * @param pin         继电器引脚
     * @param active_high true=高电平开锁，false=低电平开锁
     */
    DoorLockLogic(IGpio* gpio, int pin, bool active_high);

    bool Init();
    bool Open();
    bool Close();
    bool IsOpen() const;

private:
    int OnLevel() const;
    int OffLevel() const;

    IGpio* gpio_;
    int pin_;
    bool active_high_;
    bool initialized_ = false;
    bool open_ = false;
};

#endif // _FEATURE_DOOR_LOCK_LOGIC_H
