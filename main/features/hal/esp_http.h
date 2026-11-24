#ifndef _HAL_ESP_HTTP_H
#define _HAL_ESP_HTTP_H

#include <string>

#include "features/hal/i_http.h"

/**
 * IHttpClient 的 ESP-IDF 实现（复用 Board 的网络抽象）。
 */
class EspHttpClient : public IHttpClient {
public:
    static EspHttpClient* Get();

    bool PostJson(const std::string& url, const std::string& json,
                  const std::string& headers, int* status) override;

private:
    EspHttpClient() = default;
};

#endif // _HAL_ESP_HTTP_H
