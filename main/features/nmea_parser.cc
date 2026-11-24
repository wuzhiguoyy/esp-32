#include "nmea_parser.h"

#include <cstdlib>
#include <cstring>

namespace {

void SplitFields(char* line, char* fields[], int max_fields, int* count) {
    int n = 0;
    fields[n++] = line;
    for (char* p = line; *p != '\0' && n < max_fields; ++p) {
        if (*p == ',') {
            *p = '\0';
            fields[n++] = p + 1;
        }
    }
    *count = n;
}

bool Contains(const char* line, const char* token) {
    return line != nullptr && token != nullptr && strstr(line, token) != nullptr;
}

}  // namespace

double NmeaParser::ToDegrees(const char* value, const char* hemisphere) {
    if (value == nullptr || hemisphere == nullptr || value[0] == '\0') {
        return 0.0;
    }
    double raw = atof(value);
    int degrees = static_cast<int>(raw / 100.0);
    double minutes = raw - degrees * 100.0;
    double result = degrees + minutes / 60.0;
    if (hemisphere[0] == 'S' || hemisphere[0] == 'W') {
        result = -result;
    }
    return result;
}

bool NmeaParser::Parse(const char* line, NmeaFix* fix) {
    if (line == nullptr || fix == nullptr || line[0] != '$') {
        return false;
    }

    char buffer[160];
    strncpy(buffer, line, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char* fields[16] = {};
    int count = 0;
    SplitFields(buffer, fields, 16, &count);

    if (Contains(line, "RMC")) {
        if (count < 8) {
            return false;
        }
        bool valid = fields[2] != nullptr && fields[2][0] == 'A';
        fix->utc_time = fields[1] != nullptr ? fields[1] : "";
        fix->valid = valid;
        if (valid) {
            fix->latitude = ToDegrees(fields[3], fields[4]);
            fix->longitude = ToDegrees(fields[5], fields[6]);
            fix->speed_knots = fields[7] != nullptr ? atof(fields[7]) : 0.0;
        }
        return true;
    }

    if (Contains(line, "GGA")) {
        if (count < 7) {
            return false;
        }
        int quality = fields[6] != nullptr ? atoi(fields[6]) : 0;
        fix->utc_time = fields[1] != nullptr ? fields[1] : "";
        if (quality > 0) {
            fix->valid = true;
            fix->latitude = ToDegrees(fields[2], fields[3]);
            fix->longitude = ToDegrees(fields[4], fields[5]);
        } else {
            fix->valid = false;
        }
        return true;
    }

    return false;
}
