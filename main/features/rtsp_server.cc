#include "rtsp_server.h"

#if CONFIG_ESP32_RTSP_SERVER

#include <cstdio>
#include <cstring>
#include <string>

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>

#include "board.h"

#define TAG "Rtsp"

namespace {

constexpr int kMaxClients = 2;
constexpr int kRtpPayloadType = 26;  // JPEG
constexpr int kMaxPayload = 1200;

uint16_t g_seq = 0;
uint32_t g_ts = 0;
uint32_t g_ssrc = 0x12345678;

int ReadRequest(int sock, std::string* out) {
    char buffer[1500];
    int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        return n;
    }
    buffer[n] = '\0';
    *out = buffer;
    return n;
}

std::string HeaderValue(const std::string& req, const char* name) {
    std::string key(name);
    auto pos = req.find(key);
    if (pos == std::string::npos) {
        return "";
    }
    pos += key.size();
    auto end = req.find("\r\n", pos);
    return req.substr(pos, end == std::string::npos ? end : end - pos);
}

void Reply(int sock, const std::string& body, const char* contentType, const std::string& cseq,
           const std::string& extra = "") {
    char header[512];
    snprintf(header, sizeof(header),
             "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sContent-Type: %s\r\nContent-Length: %d\r\n\r\n",
             cseq.c_str(), extra.c_str(), contentType, static_cast<int>(body.size()));
    send(sock, header, strlen(header), 0);
    if (!body.empty()) {
        send(sock, body.data(), body.size(), 0);
    }
}

bool CaptureJpeg(std::string* jpeg) {
    auto camera = Board::GetInstance().GetCamera();
    if (camera == nullptr) {
        return false;
    }
    return camera->CaptureJpeg(jpeg);
}

bool SendInterleavedRtp(int sock, const std::string& jpeg) {
    const int mtu = kMaxPayload;
    const int offsetStep = mtu - 8;  // 8 字节 JPEG 头

    for (int offset = 0; offset < static_cast<int>(jpeg.size()); offset += offsetStep) {
        int length = static_cast<int>(jpeg.size()) - offset;
        if (length > offsetStep) {
            length = offsetStep;
        }
        bool last = (offset + length) >= static_cast<int>(jpeg.size());

        uint8_t packet[12 + 8 + kMaxPayload];
        uint8_t* p = packet;
        p[0] = 0x80;
        p[1] = static_cast<uint8_t>(last ? (0x80 | kRtpPayloadType) : kRtpPayloadType);
        p[2] = static_cast<uint8_t>(g_seq >> 8);
        p[3] = static_cast<uint8_t>(g_seq & 0xFF);
        p[4] = static_cast<uint8_t>(g_ts >> 24);
        p[5] = static_cast<uint8_t>(g_ts >> 16);
        p[6] = static_cast<uint8_t>(g_ts >> 8);
        p[7] = static_cast<uint8_t>(g_ts & 0xFF);
        p[8] = static_cast<uint8_t>(g_ssrc >> 24);
        p[9] = static_cast<uint8_t>(g_ssrc >> 16);
        p[10] = static_cast<uint8_t>(g_ssrc >> 8);
        p[11] = static_cast<uint8_t>(g_ssrc & 0xFF);

        // JPEG/RTP header (RFC 2435)
        p[12] = 0;
        p[13] = static_cast<uint8_t>((offset >> 16) & 0xFF);
        p[14] = static_cast<uint8_t>((offset >> 8) & 0xFF);
        p[15] = static_cast<uint8_t>(offset & 0xFF);
        p[16] = 1;    // type
        p[17] = 255;  // quality
        p[18] = 0;
        p[19] = 0;

        memcpy(p + 20, jpeg.data() + offset, length);

        int payload_len = 12 + 8 + length;
        uint8_t interleaved[4] = { '$', 0, static_cast<uint8_t>((payload_len >> 8) & 0xFF),
                                   static_cast<uint8_t>(payload_len & 0xFF) };
        if (send(sock, interleaved, 4, 0) < 0 || send(sock, packet, payload_len, 0) < 0) {
            return false;
        }

        g_seq++;
    }
    g_ts += 9000 / 5;  // 5 fps
    return true;
}

void HandleClient(void* arg) {
    int sock = reinterpret_cast<intptr_t>(arg);
    std::string request;

    while (true) {
        if (ReadRequest(sock, &request) <= 0) {
            break;
        }

        std::string cseq = HeaderValue(request, "CSeq: ");
        while (!cseq.empty() && (cseq.back() == '\r' || cseq.back() == '\n')) {
            cseq.pop_back();
        }

        if (request.rfind("OPTIONS", 0) == 0) {
            Reply(sock, "", "text/plain", cseq,
                  "Public: OPTIONS, DESCRIBE, SETUP, PLAY, TEARDOWN\r\n");
        } else if (request.rfind("DESCRIBE", 0) == 0) {
            std::string sdp =
                    "v=0\r\n"
                    "o=- 0 0 IN IP4 0.0.0.0\r\n"
                    "s=Daoxuan MJPEG\r\n"
                    "c=IN IP4 0.0.0.0\r\n"
                    "t=0 0\r\n"
                    "m=video 0 RTP/AVP 26\r\n"
                    "a=control:track1\r\n"
                    "a=rtpmap:26 JPEG/90000\r\n";
            Reply(sock, sdp, "application/sdp", cseq,
                  "Content-Base: rtsp://device/mjpeg/1/\r\n");
        } else if (request.rfind("SETUP", 0) == 0) {
            Reply(sock, "", "text/plain", cseq,
                  "Transport: RTP/AVP/TCP;unicast;interleaved=0-1\r\nSession: 12345678\r\n");
        } else if (request.rfind("PLAY", 0) == 0) {
            Reply(sock, "", "text/plain", cseq,
                  "Session: 12345678\r\nRTP-Info: url=track1;seq=0;rtptime=0\r\n");
            while (true) {
                std::string jpeg;
                if (!CaptureJpeg(&jpeg)) {
                    vTaskDelay(pdMS_TO_TICKS(200));
                    continue;
                }
                if (!SendInterleavedRtp(sock, jpeg)) {
                    break;
                }
                vTaskDelay(pdMS_TO_TICKS(200));
            }
            break;
        } else {
            Reply(sock, "", "text/plain", cseq);
        }
    }

    close(sock);
    vTaskDelete(nullptr);
}

}  // namespace

RtspServer& RtspServer::GetInstance() {
    static RtspServer instance;
    return instance;
}

bool RtspServer::Start(int port) {
    static bool started = false;
    if (started) {
        return true;
    }

    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "socket create failed");
        return false;
    }

    int reuse = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(listen_sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        ESP_LOGE(TAG, "bind %d failed", port);
        close(listen_sock);
        return false;
    }
    listen(listen_sock, kMaxClients);

    xTaskCreate([](void* arg) {
        int server = reinterpret_cast<intptr_t>(arg);
        while (true) {
            int client = accept(server, nullptr, nullptr);
            if (client < 0) {
                vTaskDelay(pdMS_TO_TICKS(100));
                continue;
            }
            xTaskCreate(HandleClient, "rtsp_client", 8192,
                        reinterpret_cast<void*>(static_cast<intptr_t>(client)), 5, nullptr);
        }
    }, "rtsp_server", 4096, reinterpret_cast<void*>(static_cast<intptr_t>(listen_sock)), 5, nullptr);

    started = true;
    ESP_LOGI(TAG, "RTSP server started on port %d", port);
    return true;
}

#else

RtspServer& RtspServer::GetInstance() {
    static RtspServer instance;
    return instance;
}

bool RtspServer::Start(int) { return false; }

#endif // CONFIG_ESP32_RTSP_SERVER
