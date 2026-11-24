#include "location_payload.h"

#include <cstdio>

namespace location_payload {

namespace {

std::string Escape(const std::string& input) {
    std::string out;
    out.reserve(input.size() + 8);
    for (char c : input) {
        if (c == '"' || c == '\\') {
            out.push_back('\\');
        }
        out.push_back(c);
    }
    return out;
}

}  // namespace

std::string Build(const std::string& device_id, const std::string& client_id,
                  double lat, double lng, double speed, const std::string& utc) {
    char number[128];
    snprintf(number, sizeof(number),
             "\"lat\":%.6f,\"lng\":%.6f,\"speed\":%.2f",
             lat, lng, speed);

    std::string json = "{";
    json += "\"device_id\":\"" + Escape(device_id) + "\",";
    json += "\"client_id\":\"" + Escape(client_id) + "\",";
    json += number;
    json += ",\"utc\":\"" + Escape(utc) + "\"";
    json += "}";
    return json;
}

}  // namespace location_payload
