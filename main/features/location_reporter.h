#ifndef _FEATURE_LOCATION_REPORTER_H
#define _FEATURE_LOCATION_REPORTER_H

#include "sdkconfig.h"

#ifndef CONFIG_ESP32_FEATURE_GPS
#define CONFIG_ESP32_FEATURE_GPS 0
#endif

/**
 * 周期性把 GPS 坐标上报到服务端。
 * 未启用 GPS 时为空实现，不影响老板型。
 */
class LocationReporter {
public:
    static LocationReporter& GetInstance();

    void Start();
    void Stop();

    /** 手动上报一次（调试用） */
    void ReportOnce();

private:
    LocationReporter() = default;
    void TaskLoop();

    bool running_ = false;
};

#endif // _FEATURE_LOCATION_REPORTER_H
