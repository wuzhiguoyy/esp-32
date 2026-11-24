#ifndef _FEATURE_LOCATION_PAYLOAD_H
#define _FEATURE_LOCATION_PAYLOAD_H

#include <string>

/**
 * 定位上报 JSON 组装（纯逻辑，可单测）。
 */
namespace location_payload {

/**
 * 组装上报报文。
 *
 * @param device_id 设备 MAC
 * @param client_id Client-Id
 * @param lat       纬度
 * @param lng       经度
 * @param speed     速度（节）
 * @param utc       GPS UTC 时间
 * @return JSON 字符串
 */
std::string Build(const std::string& device_id, const std::string& client_id,
                  double lat, double lng, double speed, const std::string& utc);

}  // namespace location_payload

#endif // _FEATURE_LOCATION_PAYLOAD_H
