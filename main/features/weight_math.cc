#include "weight_math.h"

#include <cmath>

namespace weight_math {

float ToGrams(long raw, long offset, float scale) {
    if (scale == 0.0f || std::isnan(scale) || std::isinf(scale)) {
        return 0.0f;
    }
    return static_cast<float>(raw - offset) / scale;
}

float CalculateScale(long raw, long offset, float known_grams) {
    if (known_grams <= 0.0f) {
        return 0.0f;
    }
    long delta = raw - offset;
    if (delta == 0) {
        return 0.0f;
    }
    return static_cast<float>(delta) / known_grams;
}

}  // namespace weight_math
