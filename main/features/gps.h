#ifndef _FEATURE_GPS_H
#define _FEATURE_GPS_H

#include <cstdint>
#include <mutex>
#include <string>

#include "sdkconfig.h"

#ifndef CONFIG_ESP32_FEATURE_GPS
#define CONFIG_ESP32_FEATURE_GPS 0
#endif

/**
 * GPS 驱动：只负责 UART 读取与状态缓存。
 * NMEA 解析交给 NmeaParser（纯逻辑，可单测）。
 */
class Gps {
public:
    static Gps& GetInstance();

    bool Start();
    bool IsFixed();
    double Latitude();
    double Longitude();
    double SpeedKnots();
    std::string UtcTime();

    /** 供单元测试注入一行报文（不经过 UART） */
    void FeedLine(const char* line);

private:
    Gps() = default;

    void TaskLoop();

    bool started_ = false;
    bool fixed_ = false;
    double lat_ = 0.0;
    double lng_ = 0.0;
    double speed_knots_ = 0.0;
    std::string utc_;
    std::mutex mutex_;
};

#endif // _FEATURE_GPS_H
