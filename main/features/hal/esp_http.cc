#include "esp_http.h"

#include <string>

#include <esp_log.h>

#include "board.h"

#define TAG "EspHttp"

EspHttpClient* EspHttpClient::Get() {
    static EspHttpClient instance;
    return &instance;
}

bool EspHttpClient::PostJson(const std::string& url, const std::string& json,
                             const std::string& headers, int* status) {
    auto& board = Board::GetInstance();
    auto network = board.GetNetwork();
    if (network == nullptr) {
        return false;
    }

    auto http = network->CreateHttp(0);
    if (http == nullptr) {
        return false;
    }

    // headers 格式："Key: Value\nKey2: Value2"
    size_t start = 0;
    while (start < headers.size()) {
        size_t end = headers.find('\n', start);
        std::string line = headers.substr(start, end == std::string::npos ? std::string::npos : end - start);
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            while (!value.empty() && value.front() == ' ') {
                value.erase(value.begin());
            }
            http->SetHeader(key, value);
        }
        if (end == std::string::npos) {
            break;
        }
        start = end + 1;
    }

    http->SetContent(std::string(json));

    if (auto opened = http->Open("POST", url); !opened) {
        ESP_LOGW(TAG, "open failed: %s", opened.error().ToString().c_str());
        return false;
    }

    auto code = http->GetStatusCode();
    if (status != nullptr) {
        *status = code ? *code : -1;
    }
    http->Close();
    return code && *code == 200;
}
