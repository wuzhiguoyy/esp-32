#ifndef _FEATURE_NMEA_PARSER_H
#define _FEATURE_NMEA_PARSER_H

#include <cstddef>
#include <string>

/**
 * NMEA 解析器（纯逻辑，不依赖 ESP-IDF）。
 *
 * 设计目的：把报文解析从串口驱动中剥离出来，可以在 PC 上直接单元测试。
 * 只负责“一行字符串 -> 定位数据”，不碰 UART、GPIO、FreeRTOS。
 */
struct NmeaFix {
    bool valid = false;
    double latitude = 0.0;
    double longitude = 0.0;
    double speed_knots = 0.0;
    std::string utc_time;
};

class NmeaParser {
public:
    /**
     * 解析一行 NMEA 报文。
     *
     * @param line 形如 $GPRMC,... 或 $GPGGA,... 的字符串
     * @param fix  输出结果；无有效定位时 valid=false
     * @return 是否识别为定位报文
     */
    static bool Parse(const char* line, NmeaFix* fix);

    /**
     * NMEA 的 ddmm.mmmm 转十进制度。
     *
     * @param value      坐标字符串
     * @param hemisphere N/S/E/W
     * @return 十进制度，南纬/西经为负
     */
    static double ToDegrees(const char* value, const char* hemisphere);
};

#endif // _FEATURE_NMEA_PARSER_H
