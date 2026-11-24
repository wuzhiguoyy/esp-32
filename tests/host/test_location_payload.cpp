#include <cstdio>
#include <string>

#include "features/location_payload.h"

static int g_failed = 0;

#define CHECK(cond)                                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond);             \
            g_failed++;                                                        \
        }                                                                      \
    } while (0)

static bool Contains(const std::string& text, const std::string& part) {
    return text.find(part) != std::string::npos;
}

int main() {
    std::string json = location_payload::Build(
            "aa:bb:cc:dd:ee:01", "client-123", 31.230400, 121.473700, 12.50, "082345.00");

    CHECK(Contains(json, "\"device_id\":\"aa:bb:cc:dd:ee:01\""));
    CHECK(Contains(json, "\"client_id\":\"client-123\""));
    CHECK(Contains(json, "\"lat\":31.230400"));
    CHECK(Contains(json, "\"lng\":121.473700"));
    CHECK(Contains(json, "\"speed\":12.50"));
    CHECK(Contains(json, "\"utc\":\"082345.00\""));
    CHECK(json.front() == '{');
    CHECK(json.back() == '}');

    // 负数坐标
    std::string south = location_payload::Build("d", "c", -33.868800, -70.652400, 0.0, "");
    CHECK(Contains(south, "\"lat\":-33.868800"));
    CHECK(Contains(south, "\"lng\":-70.652400"));

    // 特殊字符转义
    std::string escaped = location_payload::Build("d\"x", "c\\y", 0.0, 0.0, 0.0, "");
    CHECK(Contains(escaped, "d\\\"x"));
    CHECK(Contains(escaped, "c\\\\y"));

    if (g_failed == 0) {
        printf("location_payload: all tests passed\n");
        return 0;
    }
    printf("location_payload: %d test(s) failed\n", g_failed);
    return 1;
}
