#ifndef _FEATURE_WEIGHT_MATH_H
#define _FEATURE_WEIGHT_MATH_H

/**
 * 称重换算（纯逻辑，不依赖硬件）。
 * 由 HX711 原始读数换算成克数，便于单元测试。
 */
namespace weight_math {

/**
 * 原始值转克数。
 *
 * @param raw     HX711 原始读数
 * @param offset  皮重（零点原始值）
 * @param scale   比例系数（raw/gram），为 0 时返回 0
 * @return 克数
 */
float ToGrams(long raw, long offset, float scale);

/**
 * 用已知砝码计算比例系数。
 *
 * @param raw        当前原始读数
 * @param offset     皮重
 * @param known_grams 砝码克数
 * @return 比例系数，参数非法时返回 0
 */
float CalculateScale(long raw, long offset, float known_grams);

}  // namespace weight_math

#endif // _FEATURE_WEIGHT_MATH_H
