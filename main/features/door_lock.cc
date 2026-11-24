#include "door_lock.h"

#if CONFIG_ESP32_SHOP_LOCK

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "features/door_lock_logic.h"
#include "features/hal/esp_gpio.h"

#define TAG "DoorLock"

#ifndef CONFIG_ESP32_LOCK_ACTIVE_LEVEL
#define CONFIG_ESP32_LOCK_ACTIVE_LEVEL 1
#endif

DoorLock& DoorLock::GetInstance() {
    static DoorLock instance;
    return instance;
}

static DoorLockLogic& Logic() {
    static DoorLockLogic logic(EspGpio::Get(), CONFIG_ESP32_LOCK_GPIO,
                               CONFIG_ESP32_LOCK_ACTIVE_LEVEL == 1);
    return logic;
}

bool DoorLock::Init() {
    bool ok = Logic().Init();
    if (ok) {
        ESP_LOGI(TAG, "Door lock ready on GPIO%d (active level %d)",
                 CONFIG_ESP32_LOCK_GPIO, CONFIG_ESP32_LOCK_ACTIVE_LEVEL);
    } else {
        ESP_LOGE(TAG, "Door lock init failed");
    }
    return ok;
}

bool DoorLock::Open() {
    if (!Logic().Open()) {
        return false;
    }
    ESP_LOGW(TAG, "Door unlocked");
    return true;
}

bool DoorLock::Close() {
    if (!Logic().Close()) {
        return false;
    }
    ESP_LOGI(TAG, "Door locked");
    return true;
}

bool DoorLock::IsOpen() {
    return Logic().IsOpen();
}

bool DoorLock::OpenForMs(int ms) {
    if (ms <= 0) {
        ms = 3000;
    }
    if (!Open()) {
        return false;
    }
    vTaskDelay(pdMS_TO_TICKS(ms));
    return Close();
}

#else

DoorLock& DoorLock::GetInstance() {
    static DoorLock instance;
    return instance;
}

bool DoorLock::Init() { return false; }
bool DoorLock::Open() { return false; }
bool DoorLock::Close() { return false; }
bool DoorLock::IsOpen() { return false; }
bool DoorLock::OpenForMs(int) { return false; }

#endif // CONFIG_ESP32_SHOP_LOCK
