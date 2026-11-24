#ifndef _FEATURE_DOOR_LOCK_H
#define _FEATURE_DOOR_LOCK_H

#include "sdkconfig.h"

#ifndef CONFIG_ESP32_SHOP_LOCK
#define CONFIG_ESP32_SHOP_LOCK 0
#endif

/**
 * 电磁锁 / 继电器控制（无人超市门禁）。
 *
 * 接线：GPIO -> 继电器 IN，继电器常开触点接电磁锁电源。
 * 注意：继电器建议独立 12V 供电，ESP32 只给控制信号，避免烧板。
 */
class DoorLock {
public:
    static DoorLock& GetInstance();

    /** 初始化 GPIO，默认关闭 */
    bool Init();

    /** 开锁（继电器吸合） */
    bool Open();

    /** 关锁 */
    bool Close();

    /** 当前是否处于开锁状态 */
    bool IsOpen();

    /** 开锁并保持 ms 毫秒后自动关锁（阻塞） */
    bool OpenForMs(int ms);

private:
    DoorLock() = default;
    bool initialized_ = false;
    bool open_ = false;
};

#endif // _FEATURE_DOOR_LOCK_H
