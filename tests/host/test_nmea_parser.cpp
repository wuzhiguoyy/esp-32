#include <cmath>
#include <cstdio>
#include <cstring>

#include "features/nmea_parser.h"

static int g_failed = 0;

#define CHECK(cond)                                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond);             \
            g_failed++;                                                        \
        }                                                                      \
    } while (0)

static bool Near(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

int main() {
    NmeaFix fix;

    // 1. RMC 有效定位（北京附近）
    const char* rmc = "$GPRMC,082345.00,A,3959.999,N,11628.000,E,12.5,84.4,241126,,,A*6C";
    CHECK(NmeaParser::Parse(rmc, &fix));
    CHECK(fix.valid);
    CHECK(std::strncmp(fix.utc_time.c_str(), "082345.00", 9) == 0);
    CHECK(Near(fix.latitude, 39.0 + 59.999 / 60.0, 1e-4));
    CHECK(Near(fix.longitude, 116.0 + 28.0 / 60.0, 1e-4));
    CHECK(Near(fix.speed_knots, 12.5, 1e-6));

    // 2. RMC 无效定位（V）→ valid=false
    const char* rmc_invalid = "$GPRMC,082345.00,V,,,,,,,241126,,,N*7A";
    CHECK(NmeaParser::Parse(rmc_invalid, &fix));
    CHECK(!fix.valid);

    // 3. GGA 有定位质量
    const char* gga = "$GPGGA,082345.00,3959.999,N,11628.000,E,1,08,0.9,10.0,M,0.0,M,,*5C";
    CHECK(NmeaParser::Parse(gga, &fix));
    CHECK(fix.valid);
    CHECK(Near(fix.latitude, 39.0 + 59.999 / 60.0, 1e-4));

    // 4. GGA 质量 0 → 无效
    const char* gga_invalid = "$GPGGA,082345.00,,,,,0,00,99.9,,,,,,*66";
    CHECK(NmeaParser::Parse(gga_invalid, &fix));
    CHECK(!fix.valid);

    // 5. 非定位语句不认
    const char* other = "$GPGSV,3,1,11,01,10,000,00*74";
    CHECK(!NmeaParser::Parse(other, &fix));

    // 6. 空输入
    CHECK(!NmeaParser::Parse(nullptr, &fix));

    // 7. 南纬西经为负
    CHECK(Near(NmeaParser::ToDegrees("3352.000", "S"), -(33.0 + 52.0 / 60.0), 1e-6));
    CHECK(Near(NmeaParser::ToDegrees("11815.000", "W"), -(118.0 + 15.0 / 60.0), 1e-6));

    if (g_failed == 0) {
        printf("nmea_parser: all tests passed\n");
        return 0;
    }
    printf("nmea_parser: %d test(s) failed\n", g_failed);
    return 1;
}
