#ifndef CAMERA_H
#define CAMERA_H

#include <expected>
#include <string>

class Camera {
public:
    virtual ~Camera() = default;

    virtual void SetExplainUrl(const std::string& url, const std::string& token) = 0;
    virtual bool Capture() = 0;
    virtual bool SetHMirror(bool enabled) = 0;
    virtual bool SetVFlip(bool enabled) = 0;
    virtual bool SetSwapBytes(bool enabled) { return false; }  // Optional, default no-op
    virtual std::expected<std::string, std::string> Explain(const std::string& question) = 0;
    virtual std::expected<std::string, std::string> Explain(const std::string& question,
                                                            const std::string& url,
                                                            const std::string& token) = 0;

    /**
     * 抓拍一张 JPEG 并输出原始字节（用于上传到自有服务端）。
     * 默认不支持，支持的板型重写。
     */
    virtual bool CaptureJpeg(std::string* out) { return false; }
};

#endif  // CAMERA_H
