#ifndef _FEATURE_WEIGHT_SENSOR_H
#define _FEATURE_WEIGHT_SENSOR_H

#include "sdkconfig.h"

#ifndef CONFIG_ESP32_SHOP_WEIGHT
#define CONFIG_ESP32_SHOP_WEIGHT 0
#endif

/**
 * HX711 称重模块（无人超市货架/称重台）。
 *
 * 接线：HX711 DT -> GPIO_DOUT，SCK -> GPIO_SCK，VCC/GND 按模块（3.3V 或 5V）。
 * HX711 是 24 位 ADC，两线同步串行，这里用 GPIO 软件时序读取。
 * 需要先校准：放已知砝码，调用 Calibrate() 计算比例系数。
 */
class WeightSensor {
public:
    static WeightSensor& GetInstance();

    bool Init();

    /** 原始 24 位读数（多次采样取中位数，去除毛刺） */
    long ReadRaw();

    /** 去皮：当前重量作为零点 */
    void Tare();

    /** 校准：known_grams 为当前砝码重量 */
    void Calibrate(float known_grams);

    /** 读取重量（克），未初始化返回 0 */
    float ReadGrams();

    bool IsReady();

private:
    WeightSensor() = default;
    bool initialized_ = false;
    long offset_ = 0;
    float scale_ = 1.0f;
};

#endif // _FEATURE_WEIGHT_SENSOR_H
