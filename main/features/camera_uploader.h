#ifndef _FEATURE_CAMERA_UPLOADER_H
#define _FEATURE_CAMERA_UPLOADER_H

#include <string>

/**
 * 摄像头抓拍并上传到自有服务端（/api/esp32/file/upload）。
 *
 * 依赖板型支持 PIXFORMAT_JPEG 输出。
 */
class CameraUploader {
public:
    /**
     * 抓拍一张并上传。
     *
     * @param image_url 返回的访问地址，可为空
     * @return 是否成功
     */
    static bool CaptureAndUpload(std::string* image_url);
};

#endif // _FEATURE_CAMERA_UPLOADER_H
