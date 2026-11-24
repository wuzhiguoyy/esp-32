#include <cmath>
#include <cstdio>

#include "features/weight_math.h"

static int g_failed = 0;

#define CHECK(cond)                                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond);             \
            g_failed++;                                                        \
        }                                                                      \
    } while (0)

static bool Near(float a, float b, float eps = 1e-3f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // 1. 皮重 1000，比例 400（每克 400 个原始单位）
    CHECK(Near(weight_math::ToGrams(1400, 1000, 400.0f), 1.0f));
    CHECK(Near(weight_math::ToGrams(1000, 1000, 400.0f), 0.0f));
    CHECK(Near(weight_math::ToGrams(600, 1000, 400.0f), -1.0f));

    // 2. 比例非法 → 0
    CHECK(Near(weight_math::ToGrams(1400, 1000, 0.0f), 0.0f));

    // 3. 校准：500g 对应 200000 原始差值 → scale = 400
    CHECK(Near(weight_math::CalculateScale(201000, 1000, 500.0f), 400.0f, 1e-2f));

    // 4. 校准参数非法
    CHECK(Near(weight_math::CalculateScale(201000, 201000, 500.0f), 0.0f));
    CHECK(Near(weight_math::CalculateScale(201000, 1000, 0.0f), 0.0f));

    if (g_failed == 0) {
        printf("weight_math: all tests passed\n");
        return 0;
    }
    printf("weight_math: %d test(s) failed\n", g_failed);
    return 1;
}
