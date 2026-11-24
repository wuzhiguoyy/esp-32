#include "location_reporter.h"

#include <string>

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "board.h"
#include "ota.h"
#include "system_info.h"
#include "features/gps.h"
#include "features/hal/esp_http.h"
#include "features/location_payload.h"

#define TAG "Location"

LocationReporter& LocationReporter::GetInstance() {
    static LocationReporter instance;
    return instance;
}

#if CONFIG_ESP32_FEATURE_GPS
static std::string BuildBaseUrl(const std::string& ota_url) {
    auto scheme = ota_url.find("://");
    if (scheme == std::string::npos) {
        return "";
    }
    auto slash = ota_url.find('/', scheme + 3);
    if (slash == std::string::npos) {
        return ota_url;
    }
    return ota_url.substr(0, slash);
}
#endif // CONFIG_ESP32_FEATURE_GPS

void LocationReporter::Start() {
#if CONFIG_ESP32_FEATURE_GPS
    if (running_) {
        return;
    }
    running_ = true;
    Gps::GetInstance().Start();
    xTaskCreate([](void* arg) {
        static_cast<LocationReporter*>(arg)->TaskLoop();
    }, "loc_report", 6144, this, 3, nullptr);
    ESP_LOGI(TAG, "Location reporter started, interval=%d s", CONFIG_ESP32_LOCATION_INTERVAL_SEC);
#endif
}

void LocationReporter::Stop() {
    running_ = false;
}

void LocationReporter::TaskLoop() {
#if CONFIG_ESP32_FEATURE_GPS
    const int interval_ms = CONFIG_ESP32_LOCATION_INTERVAL_SEC * 1000;
    while (running_) {
        vTaskDelay(pdMS_TO_TICKS(interval_ms));
        ReportOnce();
    }
#endif
}

void LocationReporter::ReportOnce() {
#if CONFIG_ESP32_FEATURE_GPS
    auto& gps = Gps::GetInstance();
    if (!gps.IsFixed()) {
        ESP_LOGD(TAG, "No GPS fix yet");
        return;
    }

    auto& board = Board::GetInstance();

    std::string ota_url = Ota().GetCheckVersionUrl();
    std::string base = BuildBaseUrl(ota_url);
    if (base.empty()) {
        ESP_LOGW(TAG, "Cannot resolve server base url");
        return;
    }
    std::string url = base + "/api/xiaozhi/device/location";

    std::string body = location_payload::Build(
            SystemInfo::GetMacAddress(), board.GetUuid(),
            gps.Latitude(), gps.Longitude(), gps.SpeedKnots(), gps.UtcTime());

    std::string headers =
            "Device-Id: " + SystemInfo::GetMacAddress() + "\n" +
            "Client-Id: " + board.GetUuid() + "\n" +
            "Content-Type: application/json";

    int status = -1;
    bool ok = EspHttpClient::Get()->PostJson(url, body, headers, &status);
    if (ok) {
        ESP_LOGI(TAG, "Location reported: %.6f, %.6f", gps.Latitude(), gps.Longitude());
    } else {
        ESP_LOGW(TAG, "Report failed, status=%d", status);
    }
#endif
}
