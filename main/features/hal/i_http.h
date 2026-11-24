#ifndef _HAL_I_HTTP_H
#define _HAL_I_HTTP_H

#include <string>

/**
 * HTTP 客户端抽象接口（纯 C++）。
 *
 * 目的：定位上报等业务依赖接口，便于单测时不真正联网。
 */
class IHttpClient {
public:
    virtual ~IHttpClient() = default;

    /**
     * 发送 JSON POST。
     *
     * @param url     完整地址
     * @param json    JSON 字符串
     * @param headers 额外请求头，格式 "Key: Value"，可空
     * @param status  返回 HTTP 状态码
     * @return 是否发送成功
     */
    virtual bool PostJson(const std::string& url, const std::string& json,
                          const std::string& headers, int* status) = 0;
};

#endif // _HAL_I_HTTP_H
