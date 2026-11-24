#ifndef _FEATURE_RTSP_SERVER_H
#define _FEATURE_RTSP_SERVER_H

#include "sdkconfig.h"

#ifndef CONFIG_ESP32_RTSP_SERVER
#define CONFIG_ESP32_RTSP_SERVER 0
#endif

/**
 * 极简 RTSP 服务端：MJPEG over RTP（TCP interleaved）。
 *
 * 地址：rtsp://<device-ip>:<port>/mjpeg/1
 * 客户端：ffmpeg / OpenCV / VLC 均可拉流。
 *
 * 只支持单路、单客户端播放，够用即可。
 */
class RtspServer {
public:
    static RtspServer& GetInstance();

    /** 启动监听（后台任务） */
    bool Start(int port);

private:
    RtspServer() = default;
};

#endif // _FEATURE_RTSP_SERVER_H
