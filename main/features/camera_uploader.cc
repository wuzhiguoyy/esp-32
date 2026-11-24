#include "camera_uploader.h"

#include <string>
#include <utility>

#include <esp_log.h>

#include "board.h"
#include "ota.h"
#include "system_info.h"

#define TAG "CamUpload"

namespace {

std::string BaseUrl() {
    std::string ota_url = Ota().GetCheckVersionUrl();
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

std::string ExtractUrl(const std::string& json) {
    auto key = json.find("\"url\":\"");
    if (key == std::string::npos) {
        return "";
    }
    size_t start = key + 7;
    size_t end = json.find('"', start);
    if (end == std::string::npos) {
        return "";
    }
    return json.substr(start, end - start);
}

}  // namespace

bool CameraUploader::CaptureAndUpload(std::string* image_url) {
    auto& board = Board::GetInstance();
    auto camera = board.GetCamera();
    if (camera == nullptr) {
        ESP_LOGW(TAG, "No camera on this board");
        return false;
    }

    std::string jpeg;
    if (!camera->CaptureJpeg(&jpeg)) {
        ESP_LOGW(TAG, "Capture jpeg failed");
        return false;
    }

    std::string base = BaseUrl();
    if (base.empty()) {
        return false;
    }

    const std::string boundary = "----DaoxuanBoundary7d1f";
    std::string body;
    body.reserve(jpeg.size() + 256);
    body += "--" + boundary + "\r\n";
    body += "Content-Disposition: form-data; name=\"bizType\"\r\n\r\nPHOTO\r\n";
    body += "--" + boundary + "\r\n";
    body += "Content-Disposition: form-data; name=\"file\"; filename=\"photo.jpg\"\r\n";
    body += "Content-Type: image/jpeg\r\n\r\n";
    body += jpeg;
    body += "\r\n--" + boundary + "--\r\n";

    auto network = board.GetNetwork();
    if (network == nullptr) {
        return false;
    }
    auto http = network->CreateHttp(0);
    if (http == nullptr) {
        return false;
    }

    http->SetHeader("Device-Id", SystemInfo::GetMacAddress());
    http->SetHeader("Client-Id", board.GetUuid());
    http->SetHeader("Content-Type", "multipart/form-data; boundary=" + boundary);
    http->SetContent(std::move(body));

    std::string url = base + "/api/esp32/file/upload";
    if (auto opened = http->Open("POST", url); !opened) {
        ESP_LOGW(TAG, "Upload open failed: %s", opened.error().ToString().c_str());
        return false;
    }

    auto status = http->GetStatusCode();
    bool ok = status && *status == 200;
    if (ok && image_url != nullptr) {
        *image_url = ExtractUrl(http->ReadAll());
    }
    http->Close();

    ESP_LOGI(TAG, "Upload photo %s (status=%d, size=%zu)",
             ok ? "ok" : "failed", status ? *status : -1, jpeg.size());
    return ok;
}
