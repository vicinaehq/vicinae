// SPDX-License-Identifier: Zlib
/*
 * TINYEXPR - Tiny recursive descent parser and evaluation engine in C
 *
 * Copyright (c) 2015-2020 Lewis Van Winkle
 *
 * http://CodePlea.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgement in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/*
 * TINYEXPR++ - Tiny recursive descent parser and evaluation engine in C++
 *
 * Copyright (c) 2020-2024 Blake Madden
 *
 * C++ version of the TinyExpr library.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgement in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "tinyexpr.hpp"

// builtin functions
namespace te_builtins {
[[nodiscard]] constexpr static te_type te_false_value() noexcept { return 0; }

[[nodiscard]] constexpr static te_type te_true_value() noexcept { return 1; }

[[nodiscard]] constexpr static te_type te_nan_value() noexcept {
    return te_parser::te_nan;
}

[[nodiscard]] static te_type te_max_integer() noexcept {
    return te_parser::get_max_integer();
}

[[nodiscard]] static te_type te_even(te_type val) {
    if (!std::isfinite(val)) {
        return te_parser::te_nan;
    }
    int64_t rounded{static_cast<int64_t>(std::ceil(std::abs(val)))};
    if ((rounded % 2) != 0) {
        ++rounded;
    }
    return (val < 0 ? -(static_cast<te_type>(rounded))
                    : static_cast<te_type>(rounded));
}

[[nodiscard]] static te_type te_odd(te_type val) {
    if (!std::isfinite(val)) {
        return te_parser::te_nan;
    }
    int64_t rounded{static_cast<int64_t>(std::ceil(std::abs(val)))};
    if ((rounded % 2) == 0) {
        ++rounded;
    }
    return (val < 0 ? -(static_cast<te_type>(rounded))
                    : static_cast<te_type>(rounded));
}

[[nodiscard]] static te_type te_is_even(te_type val) {
    if (!std::isfinite(val)) {
        return te_parser::te_nan;
    }
    const int64_t floored{static_cast<int64_t>(std::floor(val))};
    return ((floored % 2) == 0 ? te_true_value() : te_false_value());
}

[[nodiscard]] static te_type te_is_odd(te_type val) {
    if (!std::isfinite(val)) {
        return te_parser::te_nan;
    }
    const int64_t floored{static_cast<int64_t>(std::floor(val))};
    return ((floored % 2) != 0 ? te_true_value() : te_false_value());
}

[[nodiscard]] constexpr static te_type te_equal(te_type val1,
                                                te_type val2) noexcept {
    return static_cast<te_type>((val1 == val2) ? 1 : 0);
}

[[nodiscard]] constexpr static te_type te_not_equal(te_type val1,
                                                    te_type val2) noexcept {
    return static_cast<te_type>((val1 != val2) ? 1 : 0);
}

[[nodiscard]] constexpr static te_type te_less_than(te_type val1,
                                                    te_type val2) noexcept {
    return static_cast<te_type>((val1 < val2) ? 1 : 0);
}

[[nodiscard]] constexpr static te_type te_less_than_equal_to(
    te_type val1, te_type val2) noexcept {
    return static_cast<te_type>((val1 <= val2) ? 1 : 0);
}

[[nodiscard]] constexpr static te_type te_greater_than(te_type val1,
                                                       te_type val2) noexcept {
    return static_cast<te_type>((val1 > val2) ? 1 : 0);
}

[[nodiscard]] constexpr static te_type te_greater_than_equal_to(
    te_type val1, te_type val2) noexcept {
    return static_cast<te_type>((val1 >= val2) ? 1 : 0);
}

[[nodiscard]] constexpr static te_type te_and(te_type val1, te_type val2) {
    // clang-format off
        return (!std::isfinite(val1) && !std::isfinite(val2)) ?
            te_parser::te_nan :
            static_cast<te_type>(
                (te_parser::double_to_bool(val1) && te_parser::double_to_bool(val2)) ? 1 : 0);
    // clang-format on
}

[[nodiscard]] constexpr static te_type te_or(te_type val1, te_type val2) {
    // clang-format off
        return (!std::isfinite(val1) && !std::isfinite(val2)) ?
            te_parser::te_nan :
            static_cast<te_type>(
                (te_parser::double_to_bool(val1) || te_parser::double_to_bool(val2)) ? 1 : 0);
    // clang-format on
}

[[nodiscard]] static te_type te_not(te_type val) {
    return std::isfinite(val)
               ? static_cast<te_type>(!te_parser::double_to_bool(val))
               : te_parser::te_nan;
}

[[nodiscard]] constexpr static te_type te_pi() noexcept {
    return static_cast<te_type>(3.14159265358979323846);  // NOLINT
}

[[nodiscard]] constexpr static te_type te_e() noexcept {
    return static_cast<te_type>(2.71828182845904523536);  // NOLINT
}

[[nodiscard]] static te_type te_fac(
    te_type val) noexcept { /* simplest version of factorial */
    if (!std::isfinite(val) || val < 0.0) {
        return te_parser::te_nan;
    }
    if (val > (std::numeric_limits<unsigned int>::max)()) {
        return std::numeric_limits<te_type>::infinity();
    }
    const auto usignVal = static_cast<size_t>(val);
    uint32_t result{1};
    for (uint32_t i = 1; i <= usignVal; i++) {
        if (i > (std::numeric_limits<uint32_t>::max)() / result) {
            return std::numeric_limits<te_type>::infinity();
        }
        result *= i;
    }
    return static_cast<te_type>(result);
}

[[nodiscard]] static te_type te_absolute_value(te_type val) {
    return std::fabs(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_log(te_type val) {
    return std::log(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_log10(te_type val) {
    return std::log10(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_pow(te_type val1, te_type val2) {
    return std::pow(static_cast<te_type>(val1), static_cast<te_type>(val2));
}

[[nodiscard]] static te_type te_tan(te_type val) {
    return std::tan(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_tanh(te_type val) {
    return std::tanh(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_trunc(te_type val) {
    return std::trunc(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_sin(te_type val) {
    return std::sin(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_sinh(te_type val) {
    return std::sinh(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_sqrt(te_type val) {
    if (val < 0) {
        throw std::runtime_error("Negative value passed to SQRT.");
    }
    return std::sqrt(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_floor(te_type val) {
    return std::floor(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_ceil(te_type val) {
    return std::ceil(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_exp(te_type val) {
    return std::exp(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_cos(te_type val) {
    return std::cos(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_cosh(te_type val) {
    return std::cosh(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_acos(te_type val) {
    return std::acos(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_asin(te_type val) {
    if (std::isfinite(val) && (val < -1.0 || val > 1.0)) {
        throw std::runtime_error(
            "Argument passed to ASIN must be between -1 and 1.");
    }
    return std::asin(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_atan(te_type val) {
    return std::atan(static_cast<te_type>(val));
}

[[nodiscard]] static te_type te_atan2(te_type val1, te_type val2) {
    return std::atan2(static_cast<te_type>(val1), (static_cast<te_type>(val2)));
}

[[nodiscard]] static te_type te_tgamma(te_type val) { return std::tgamma(val); }

[[nodiscard]] static te_type te_random() {
    std::random_device rdev;
    std::mt19937 gen(rdev());
    std::uniform_real_distribution<te_type> distr(0, 1);
    return distr(gen);
}

[[nodiscard]] constexpr static te_type te_divide(te_type val1, te_type val2) {
    if (val2 == 0) {
        throw std::runtime_error("Division by zero.");
    }
    return val1 / val2;
}

[[nodiscard]] static te_type te_modulus(te_type val1, te_type val2) {
    if (val2 == 0) {
        throw std::runtime_error("Modulus by zero.");
    }
    return std::fmod(val1, val2);
}

[[nodiscard]] static te_type te_sum(te_type val1, te_type val2, te_type val3,
                                    te_type val4, te_type val5, te_type val6,
                                    te_type val7) {
    return (!std::isfinite(val1) ? 0 : val1) +
           (!std::isfinite(val2) ? 0 : val2) +
           (!std::isfinite(val3) ? 0 : val3) +
           (!std::isfinite(val4) ? 0 : val4) +
           (!std::isfinite(val5) ? 0 : val5) +
           (!std::isfinite(val6) ? 0 : val6) +
           (!std::isfinite(val7) ? 0 : val7);
}

[[nodiscard]] static te_type te_average(te_type val1, te_type val2,
                                        te_type val3, te_type val4,
                                        te_type val5, te_type val6,
                                        te_type val7) {
    const auto validN =
        (!std::isfinite(val1) ? 0 : 1) + (!std::isfinite(val2) ? 0 : 1) +
        (!std::isfinite(val3) ? 0 : 1) + (!std::isfinite(val4) ? 0 : 1) +
        (!std::isfinite(val5) ? 0 : 1) + (!std::isfinite(val6) ? 0 : 1) +
        (!std::isfinite(val7) ? 0 : 1);
    const auto total = te_sum(val1, val2, val3, val4, val5, val6, val7);
    return te_divide(total, static_cast<te_type>(validN));
}

/// @warning This version of round emulates Excel behavior of supporting
///     negative decimal places (e.g., ROUND(21.5, -1) = 20). Be aware
///     of that if using this function outside of TinyExpr++.
[[nodiscard]] static te_type te_round(te_type val,
                                      te_type decimalPlaces)  // NOLINT
{
    const bool useNegativeRound{decimalPlaces < 0};
    const size_t adjustedDecimalPlaces{
        !std::isfinite(decimalPlaces)
            ? 0
            : static_cast<size_t>(std::abs(decimalPlaces))};

    const auto decimalPostition =
        static_cast<te_type>(std::pow(10, adjustedDecimalPlaces));
    if (!std::isfinite(decimalPostition)) {
        return te_parser::te_nan;
    }
    constexpr te_type ROUND_EPSILON{0.5};  // NOLINT

    if (!useNegativeRound) {
        if (val < 0) {
            return (decimalPostition == 0)
                       ? std::ceil(val - ROUND_EPSILON)
                       : std::ceil(
                             static_cast<te_type>(val * decimalPostition) -
                             ROUND_EPSILON) /
                             decimalPostition;
        }
        return (decimalPostition == 0)
                   ? std::floor(val + ROUND_EPSILON)
                   : std::floor(static_cast<te_type>(val * decimalPostition) +
                                ROUND_EPSILON) /
                         decimalPostition;
    }
    // ROUND(21.5, -1) = 20
    if (val < 0) {
        return std::ceil(static_cast<te_type>(val / decimalPostition) -
                         ROUND_EPSILON) *
               decimalPostition;
    }
    return std::floor(static_cast<te_type>(val / decimalPostition) +
                      ROUND_EPSILON) *
           decimalPostition;
}

// Combinations (without repetition)
[[nodiscard]] static te_type te_ncr(te_type val1, te_type val2) noexcept {
    if (!std::isfinite(val1) || !std::isfinite(val2) || val1 < 0.0 ||
        val2 < 0.0 || val1 < val2) {
        return te_parser::te_nan;
    }
    if (val1 > ((std::numeric_limits<unsigned int>::max)()) ||
        val2 > (std::numeric_limits<unsigned int>::max)()) {
        return std::numeric_limits<te_type>::infinity();
    }
    const uint32_t usignN{static_cast<unsigned int>(val1)};
    uint32_t usignR{static_cast<unsigned int>(val2)};
    uint32_t result{1};
    if (usignR > usignN / 2) {
        usignR = usignN - usignR;
    }
    for (decltype(usignR) i = 1; i <= usignR; i++) {
        if (result >
            ((std::numeric_limits<uint32_t>::max)()) / (usignN - usignR + i)) {
            return std::numeric_limits<te_type>::infinity();
        }
        result *= usignN - usignR + i;
        result /= i;
    }
    return static_cast<te_type>(result);
}

// Permutations (without repetition)
[[nodiscard]] static te_type te_npr(te_type val1, te_type val2) noexcept {
    return te_ncr(val1, val2) * te_fac(val2);
}

[[nodiscard]] constexpr static te_type te_add(te_type val1,
                                              te_type val2) noexcept {
    return val1 + val2;
}

[[nodiscard]] constexpr static te_type te_sub(te_type val1,
                                              te_type val2) noexcept {
    return val1 - val2;
}

[[nodiscard]] constexpr static te_type te_mul(te_type val1,
                                              te_type val2) noexcept {
    return val1 * val2;
}

#if __cplusplus >= 202002L && !defined(TE_FLOAT)
//--------------------------------------------------
[[nodiscard]] static te_type te_right_rotate8(te_type val1, te_type val2) {
    constexpr int BITNESS{8};
    if (std::floor(val1) != val1 || std::floor(val2) != val2) {
        throw std::runtime_error(
            "Bitwise RIGHT ROTATE operation must use integers.");
    }
    if (val1 < 0) {
        throw std::runtime_error(
            "Bitwise RIGHT ROTATE value must be positive.");
    }
    if (val2 > BITNESS) {
        throw std::runtime_error("Rotation operation must be between 0-8.");
    }

    return static_cast<te_type>(
        std::rotr(static_cast<uint8_t>(val1), static_cast<int>(val2)));
}

//--------------------------------------------------
[[nodiscard]] static te_type te_left_rotate8(te_type val1, te_type val2) {
    constexpr int BITNESS{8};
    if (std::floor(val1) != val1 || std::floor(val2) != val2) {
        throw std::runtime_error(
            "Bitwise LEFT ROTATE operation must use integers.");
    }
    if (val1 < 0) {
        throw std::runtime_error("Bitwise LEFT ROTATE value must be positive.");
    }
    if (val2 > BITNESS) {
        throw std::runtime_error("Rotation operation must be between 0-8.");
    }

    return static_cast<te_type>(
        std::rotl(static_cast<uint8_t>(val1), static_cast<int>(val2)));
}

//--------------------------------------------------
[[nodiscard]] static te_type te_right_rotate16(te_type val1, te_type val2) {
    constexpr int BITNESS{16};
    if (std::floor(val1) != val1 || std::floor(val2) != val2) {
        throw std::runtime_error(
            "Bitwise RIGHT ROTATE operation must use integers.");
    }
    if (val1 < 0) {
        throw std::runtime_error(
            "Bitwise RIGHT ROTATE value must be positive.");
    }
    if (val2 > BITNESS) {
        throw std::runtime_error("Rotation operation must be between 0-16.");
    }

    return static_cast<te_type>(
        std::rotr(static_cast<uint16_t>(val1), static_cast<int>(val2)));
}

//--------------------------------------------------
[[nodiscard]] static te_type te_left_rotate16(te_type val1, te_type val2) {
    constexpr int BITNESS{16};
    if (std::floor(val1) != val1 || std::floor(val2) != val2) {
        throw std::runtime_error(
            "Bitwise LEFT ROTATE operation must use integers.");
    }
    if (val1 < 0) {
        throw std::runtime_error("Bitwise LEFT ROTATE value must be positive.");
    }
    if (val2 > BITNESS) {
        throw std::runtime_error("Rotation operation must be between 0-16.");
    }

    return static_cast<te_type>(
        std::rotl(static_cast<uint16_t>(val1), static_cast<int>(val2)));
}

//--------------------------------------------------
[[nodiscard]] static te_type te_right_rotate32(te_type val1, te_type val2) {
    constexpr int BITNESS{32};
    if constexpr (!te_parser::supports_32bit()) {
        throw std::runtime_error(
            "32-bit bitwise operations are not supported.");
    }
    if (std::floor(val1) != val1 || std::floor(val2) != val2) {
        throw std::runtime_error(
            "Bitwise RIGHT ROTATE operation must use integers.");
    }
    if (val1 < 0) {
        throw std::runtime_error(
            "Bitwise RIGHT ROTATE value must be positive.");
    }
    if (val2 > BITNESS) {
        throw std::runtime_error("Rotation operation must be between 0-32.");
    }

    return static_cast<te_type>(
        std::rotr(static_cast<uint32_t>(val1), static_cast<int>(val2)));
}

//--------------------------------------------------
[[nodiscard]] static te_type te_left_rotate32(te_type val1, te_type val2) {
    constexpr int BITNESS{32};
    if constexpr (!te_parser::supports_32bit()) {
        throw std::runtime_error(
            "32-bit bitwise operations are not supported.");
    }
    if (std::floor(val1) != val1 || std::floor(val2) != val2) {
        throw std::runtime_error(
            "Bitwise LEFT ROTATE operation must use integers.");
    }
    if (val1 < 0) {
        throw std::runtime_error("Bitwise LEFT ROTATE value must be positive.");
    }
    if (val2 > BITNESS) {
        throw std::runtime_error("Rotation operation must be between 0-32.");
    }

    return static_cast<te_type>(
        std::rotl(static_cast<uint32_t>(val1), static_cast<int>(val2)));
}

//--------------------------------------------------
[[nodiscard]] static te_type te_right_rotate64(te_type val1, te_type val2) {
    constexpr int BITNESS{63};
    if constexpr (!te_parser::supports_64bit()) {
        throw std::runtime_error(
            "64-bit bitwise operations are not supported.");
    }
    if (std::floor(val1) != val1 || std::floor(val2) != val2) {
        throw std::runtime_error(
            "Bitwise RIGHT ROTATE operation must use integers.");
    }
    if (val1 < 0) {
        throw std::runtime_error(
            "Bitwise RIGHT ROTATE value must be positive.");
    }
    if (val2 > BITNESS) {
        throw std::runtime_error("Rotation operation must be between 0-63");
    }

    return static_cast<te_type>(
        std::rotr(static_cast<uint64_t>(val1), static_cast<int>(val2)));
}

//--------------------------------------------------
[[nodiscard]] static te_type te_left_rotate64(te_type val1, te_type val2) {
    constexpr int BITNESS{63};
    if constexpr (!te_parser::supports_64bit()) {
        throw std::runtime_error(
            "64-bit bitwise operations are not supported.");
    }
    if (std::floor(val1) != val1 || std::floor(val2) != val2) {
        throw std::runtime_error(
            "Bitwise LEFT ROTATE operation must use integers.");
    }
    if (val1 < 0) {
        throw std::runtime_error("Bitwise LEFT ROTATE value must be positive.");
    }
    if (val2 > BITNESS) {
        throw std::runtime_error("Rotation operation must be between 0-63");
    }

    return static_cast<te_type>(
        std::rotl(static_cast<uint64_t>(val1), static_cast<int>(val2)));
}

//--------------------------------------------------
[[nodiscard]] static te_type te_right_rotate(te_type val1, te_type val2) {
    if constexpr (te_parser::supports_64bit()) {
        return te_right_rotate64(val1, val2);
    } else if constexpr (te_parser::supports_32bit()) {
        return te_right_rotate32(val1, val2);
    } else {
        return te_right_rotate16(val1, val2);
    }
}

//--------------------------------------------------
[[nodiscard]] static te_type te_left_rotate(te_type val1, te_type val2) {
    if constexpr (te_parser::supports_64bit()) {
        return te_left_rotate64(val1, val2);
    } else if constexpr (te_parser::supports_32bit()) {
        return te_left_rotate32(val1, val2);
    } else {
        return te_left_rotate16(val1, val2);
    }
}
#endif
//--------------------------------------------------
[[nodiscard]] static te_type te_bitwise_not8(te_type val) {
    if (std::floor(val) != val) {
        throw std::runtime_error("Bitwise NOT must use integers.");
    }
    if (val < 0) {
        throw std::runtime_error("Bitwise NOT value must be positive.");
    }
    if (val > std::numeric_limits<uint8_t>::max()) {
        throw std::runtime_error("Value is too large for bitwise NOT.");
    }

    // force the bit manipulation to stay unsigned, like what Excel does
    const uint8_t intVal{static_cast<uint8_t>(val)};
    const decltype(intVal) result{std::bit_not<decltype(intVal)>{}(intVal)};
    return static_cast<te_type>(result);
}

//--------------------------------------------------
[[nodiscard]] static te_type te_bitwise_not16(te_type val) {
    if (std::floor(val) != val) {
        throw std::runtime_error("Bitwise NOT must use integers.");
    }
    if (val < 0) {
        throw std::runtime_error("Bitwise NOT value must be positive.");
    }
    if (val > std::numeric_limits<uint16_t>::max()) {
        throw std::runtime_error("Value is too large for bitwise NOT.");
    }

    const uint16_t intVal{static_cast<uint16_t>(val)};
    const decltype(intVal) result{std::bit_not<decltype(intVal)>{}(intVal)};
    return static_cast<te_type>(result);
}

//--------------------------------------------------
[[nodiscard]] static te_type te_bitwise_not32(te_type val) {
    if constexpr (!te_parser::supports_32bit()) {
        throw std::runtime_error(
            "32-bit bitwise operations are not supported.");
    }
    if (std::floor(val) != val) {
        throw std::runtime_error("Bitwise NOT must use integers.");
    }
    if (val < 0) {
        throw std::runtime_error("Bitwise NOT value must be positive.");
    }
    if (val > std::numeric_limits<uint32_t>::max()) {
        throw std::runtime_error("Value is too large for bitwise NOT.");
    }

    const uint32_t intVal{static_cast<uint32_t>(val)};
    const decltype(intVal) result{std::bit_not<decltype(intVal)>{}(intVal)};
    return static_cast<te_type>(result);
}

//--------------------------------------------------
[[nodiscard]] static te_type te_bitwise_not64(te_type val) {
    if constexpr (!te_parser::supports_64bit()) {
        throw std::runtime_error(
            "64-bit bitwise operations are not supported.");
    }
    if (std::floor(val) != val) {
        throw std::runtime_error("Bitwise NOT must use integers.");
    }
    if (val < 0) {
        throw std::runtime_error("Bitwise NOT value must be positive.");
    }
    if (val > std::numeric_limits<uint64_t>::max()) {
        throw std::runtime_error("Value is too large for bitwise NOT.");
    }

    const uint64_t intVal{static_cast<uint64_t>(val)};
    const decltype(intVal) result{std::bit_not<decltype(intVal)>{}(intVal)};
    return static_cast<te_type>(result);
}

//--------------------------------------------------
[[nodiscard]] static te_type te_bitwise_not(te_type val) {
    if constexpr (te_parser::supports_64bit()) {
        return te_bitwise_not64(val);
    } else if constexpr (te_parser::supports_32bit()) {
        return te_bitwise_not32(val);
    } else {
        return te_bitwise_not16(val);
    }
}

//--------------------------------------------------
[[nodiscard]] static te_type te_bitwise_or(te_type val1, te_type val2) {
    if (std::floor(val1) != val1 || std::floor(val2) != val2) {
        throw std::runtime_error("Bitwise OR operation must use integers.");
    }
    // negative technically should be allowed, but spreadsheet programs do
    // not allow them; hence, we won't either
    if (val1 < 0 || val2 < 0) {
        throw std::runtime_error(
            "Bitwise OR operation must use positive integers.");
    }
    if (val1 > te_parser::MAX_BITOPS_VAL || val2 > te_parser::MAX_BITOPS_VAL) {
        throw std::runtime_error("Value is too large for bitwise operation.");
    }
    return static_cast<te_type>(static_cast<uint64_t>(val1) |
                                static_cast<uint64_t>(val2));
}

//--------------------------------------------------
[[nodiscard]] static te_type te_bitwise_xor(te_type val1, te_type val2) {
    if (std::floor(val1) != val1 || std::floor(val2) != val2) {
        throw std::runtime_error("Bitwise XOR operation must use integers.");
    }
    // negative technically should be allowed, but spreadsheet programs do
    // not allow them; hence, we won't either
    if (val1 < 0 || val2 < 0) {
        throw std::runtime_error(
            "Bitwise XOR operation must use positive integers.");
    }
    if (val1 > te_parser::MAX_BITOPS_VAL || val2 > te_parser::MAX_BITOPS_VAL) {
        throw std::runtime_error("Value is too large for bitwise operation.");
    }
    return static_cast<te_type>(static_cast<uint64_t>(val1) ^
                                static_cast<uint64_t>(val2));
}

//--------------------------------------------------
[[nodiscard]] static te_type te_bitwise_and(te_type val1, te_type val2) {
    if (std::floor(val1) != val1 || std::floor(val2) != val2) {
        throw std::runtime_error("Bitwise AND operation must use integers.");
    }
    // negative technically should be allowed, but spreadsheet programs do
    // not allow them; hence, we won't either
    if (val1 < 0 || val2 < 0) {
        throw std::runtime_error(
            "Bitwise AND operation must use positive integers.");
    }
    if (val1 > te_parser::MAX_BITOPS_VAL || val2 > te_parser::MAX_BITOPS_VAL) {
        throw std::runtime_error("Value is too large for bitwise operation.");
    }
    return static_cast<te_type>(static_cast<uint64_t>(val1) &
                                static_cast<uint64_t>(val2));
}

// Shift operators
//--------------------------------------------------
[[nodiscard]] static te_type te_left_shift(te_type val1, te_type val2) {
    // For 64-bit, you can shift 63 bits.
    // If we are limited to something like 53 bits, then we can use that (same
    // as Excel)
    constexpr static auto MAX_BITNESS_PARAM =
        (te_parser::supports_64bit() ? te_parser::get_max_integer_bitness() - 1
                                     : te_parser::get_max_integer_bitness());
    if (std::floor(val1) != val1) {
        throw std::runtime_error(
            "Left side of left shift (<<) operation must be an integer.");
    }
    if (std::floor(val2) != val2) {
        throw std::runtime_error(
            "Additive expression of left shift (<<) operation must be an "
            "integer.");
    }
    if (val1 < 0) {
        throw std::runtime_error(
            "Left side of left shift (<<) operation cannot be negative.");
    }
    if (val1 > te_parser::MAX_BITOPS_VAL) {
        throw std::runtime_error("Value is too large for bitwise operation.");
    }
    // bitness is limited to 53-bit or 64-bit, so ensure shift doesn't go beyond
    // that and cause undefined behavior
    if (val2 < 0 || val2 > MAX_BITNESS_PARAM) {
        throw std::runtime_error(
            "Additive expression of left shift (<<) operation must be between "
            "0-" +
            std::to_string(MAX_BITNESS_PARAM));
    }

    const auto multipler =
        (static_cast<uint64_t>(1) << static_cast<uint64_t>(val2));
    const auto maxBaseNumber =
        (std::numeric_limits<uint64_t>::max() / multipler);
    if (static_cast<uint64_t>(val1) > maxBaseNumber) {
        throw std::runtime_error(
            "Overflow in left shift (<<) operation; base number is too large.");
    }
    return static_cast<te_type>(static_cast<uint64_t>(val1)
                                << static_cast<uint64_t>(val2));
}

//--------------------------------------------------
[[nodiscard]] static te_type te_right_shift(te_type val1, te_type val2) {
    constexpr static auto MAX_BITNESS_PARAM =
        (te_parser::supports_64bit() ? te_parser::get_max_integer_bitness() - 1
                                     : te_parser::get_max_integer_bitness());

    if (std::floor(val1) != val1) {
        throw std::runtime_error(
            "Left side of right shift (>>) operation must be an integer.");
    }
    if (std::floor(val2) != val2) {
        throw std::runtime_error(
            "Additive expression of right shift (>>) operation must be an "
            "integer.");
    }
    if (val1 < 0) {
        throw std::runtime_error(
            "Left side of right shift (<<) operation cannot be negative.");
    }
    if (val1 > te_parser::MAX_BITOPS_VAL) {
        throw std::runtime_error("Value is too large for bitwise operation.");
    }
    if (val2 < 0 || val2 > MAX_BITNESS_PARAM) {
        throw std::runtime_error(
            "Additive expression of right shift (>>) operation must be between "
            "0-" +
            std::to_string(MAX_BITNESS_PARAM));
    }

    return static_cast<te_type>(static_cast<uint64_t>(val1) >>
                                static_cast<uint64_t>(val2));
}

/// @warning This emulates Excel, where a negative shift amount acts as a right
/// shift.\n
///     Be aware of this if using this function outside of TinyExpr++.
//--------------------------------------------------
[[nodiscard]] static te_type te_left_shift_or_right(te_type val1,
                                                    te_type val2) {
    return (val2 >= 0) ? te_left_shift(val1, val2)
                       : te_right_shift(val1, std::abs(val2));
}

/// @warning This emulates Excel, where a negative shift amount acts as a right
/// shift.\n
///     Be aware of this if using this function outside of TinyExpr++.
//--------------------------------------------------
[[nodiscard]] static te_type te_right_shift_or_left(te_type val1,
                                                    te_type val2) {
    return (val2 >= 0) ? te_right_shift(val1, val2)
                       : te_left_shift(val1, std::abs(val2));
}

[[nodiscard]] constexpr static te_type te_sqr(te_type val) noexcept {
    return val * val;
}

[[nodiscard]] static te_type te_max_maybe_nan(te_type val1,
                                              te_type val2MaybeNan) noexcept {
    return (std::max)(val1, !std::isfinite(val2MaybeNan) ? val1 : val2MaybeNan);
}

[[nodiscard]] static te_type te_max(te_type val1, te_type val2, te_type val3,
                                    te_type val4, te_type val5, te_type val6,
                                    te_type val7) noexcept {
    // assumes that at least val1 is a number, rest can be NaN
    // NOLINTBEGIN
    auto maxVal = te_max_maybe_nan(val1, val2);
    maxVal = te_max_maybe_nan(maxVal, val3);
    maxVal = te_max_maybe_nan(maxVal, val4);
    maxVal = te_max_maybe_nan(maxVal, val5);
    maxVal = te_max_maybe_nan(maxVal, val6);
    return te_max_maybe_nan(maxVal, val7);
    // NOLINTEND
}

[[nodiscard]] static te_type te_min_maybe_nan(te_type val1,
                                              te_type val2MaybeNan) noexcept {
    return (std::min)(val1, !std::isfinite(val2MaybeNan) ? val1 : val2MaybeNan);
}

[[nodiscard]] static te_type te_min(te_type val1, te_type val2, te_type val3,
                                    te_type val4, te_type val5, te_type val6,
                                    te_type val7) noexcept {
    // assumes that at least val1 is legit, rest can be NaN
    // NOLINTBEGIN
    auto minVal = te_min_maybe_nan(val1, val2);
    minVal = te_min_maybe_nan(minVal, val3);
    minVal = te_min_maybe_nan(minVal, val4);
    minVal = te_min_maybe_nan(minVal, val5);
    minVal = te_min_maybe_nan(minVal, val6);
    return te_min_maybe_nan(minVal, val7);
    // NOLINTEND
}

[[nodiscard]] static te_type te_and_maybe_nan(te_type val1,
                                              te_type val2MaybeNan) {
    return !std::isfinite(val2MaybeNan)
               ? static_cast<te_type>(te_parser::double_to_bool(val1))
               : static_cast<te_type>(te_parser::double_to_bool(val1) &&
                                      te_parser::double_to_bool(val2MaybeNan));
}

[[nodiscard]] static te_type te_and_variadic(te_type val1, te_type val2,
                                             te_type val3, te_type val4,
                                             te_type val5, te_type val6,
                                             te_type val7) {
    // at least val1 must be legit, rest can be NaN
    if (!std::isfinite(val1)) {
        return te_parser::te_nan;
    }
    // NOLINTBEGIN
    auto andVal = te_and_maybe_nan(val1, val2);
    andVal = te_and_maybe_nan(andVal, val3);
    andVal = te_and_maybe_nan(andVal, val4);
    andVal = te_and_maybe_nan(andVal, val5);
    andVal = te_and_maybe_nan(andVal, val6);
    return te_and_maybe_nan(andVal, val7);
    // NOLINTEND
}

[[nodiscard]] static te_type te_or_maybe_nan(te_type val1,
                                             te_type val2MaybeNan) {
    return !std::isfinite(val2MaybeNan)
               ? static_cast<te_type>(te_parser::double_to_bool(val1))
               : static_cast<te_type>(te_parser::double_to_bool(val1) ||
                                      te_parser::double_to_bool(val2MaybeNan));
}

[[nodiscard]] static te_type te_or_variadic(te_type val1, te_type val2,
                                            te_type val3, te_type val4,
                                            te_type val5, te_type val6,
                                            te_type val7) {
    // at least val1 must be legit, rest can be NaN
    if (!std::isfinite(val1)) {
        return te_parser::te_nan;
    }
    // NOLINTBEGIN
    auto orVal = te_or_maybe_nan(val1, val2);
    orVal = te_or_maybe_nan(orVal, val3);
    orVal = te_or_maybe_nan(orVal, val4);
    orVal = te_or_maybe_nan(orVal, val5);
    orVal = te_or_maybe_nan(orVal, val6);
    return te_or_maybe_nan(orVal, val7);
    // NOLINTEND
}

[[nodiscard]] static te_type te_if(te_type val1, te_type val2, te_type val3) {
    return te_parser::double_to_bool(val1) ? val2 : val3;
}

[[nodiscard]] static te_type te_ifs(te_type if1, te_type if1True, te_type if2,
                                    te_type if2True, te_type if3,
                                    te_type if3True) {
    return te_parser::double_to_bool(if1)   ? if1True
           : te_parser::double_to_bool(if2) ? if2True
           : te_parser::double_to_bool(if3) ? if3True
                                            : te_parser::te_nan;
}

[[nodiscard]] constexpr static te_type te_supports_32bit() noexcept {
    return te_parser::supports_32bit() ? te_true_value() : te_false_value();
}

[[nodiscard]] constexpr static te_type te_supports_64bit() noexcept {
    return te_parser::supports_64bit() ? te_true_value() : te_false_value();
}

// cotangent
[[nodiscard]] static te_type te_cot(te_type val) noexcept {
    if (val == 0.0) {
        return te_parser::te_nan;
    }
    return 1 / static_cast<te_type>(std::tan(val));
}

[[nodiscard]] constexpr static te_type te_sign(te_type val) noexcept {
    return static_cast<te_type>((val < 0.0) ? -1 : (val > 0.0) ? 1 : 0);
}

[[nodiscard]] constexpr static te_type te_negate(te_type val) noexcept {
    return -val;
}

[[nodiscard]] constexpr static te_type te_comma(
    [[maybe_unused]] te_type unusedVal,  // NOLINT
    te_type val2) noexcept {
    return val2;
}
}  // namespace te_builtins

//--------------------------------------------------
void te_parser::te_free_parameters(te_expr* texp) {
    if (texp == nullptr) {
        return;
    }
    if (is_closure(texp->m_value)) {
        // last param is the context object, we don't manage that here
        for (auto param = texp->m_parameters.begin();
             param != texp->m_parameters.end() - 1; ++param) {
            te_free(*param);
            *param = nullptr;
        }
    } else if (is_function(texp->m_value)) {
        for (auto* param : texp->m_parameters) {
            te_free(param);
            param = nullptr;
        }
    }
}

//--------------------------------------------------
const std::set<te_variable> te_parser::m_functions = {  // NOLINT
    {"abs", static_cast<te_fun1>(te_builtins::te_absolute_value), TE_PURE},
    {"acos", static_cast<te_fun1>(te_builtins::te_acos), TE_PURE},
    // variadic, accepts 1-7 arguments
    {"and", static_cast<te_fun7>(te_builtins::te_and_variadic),
     static_cast<te_variable_flags>(TE_PURE | TE_VARIADIC)},
    {"asin", static_cast<te_fun1>(te_builtins::te_asin), TE_PURE},
    {"atan", static_cast<te_fun1>(te_builtins::te_atan), TE_PURE},
    {"atan2", static_cast<te_fun2>(te_builtins::te_atan2), TE_PURE},
    {"average", static_cast<te_fun7>(te_builtins::te_average),
     static_cast<te_variable_flags>(TE_PURE | TE_VARIADIC)},
#ifndef TE_FLOAT
    {"bitand", static_cast<te_fun2>(te_builtins::te_bitwise_and), TE_PURE},
    {"bitor", static_cast<te_fun2>(te_builtins::te_bitwise_or), TE_PURE},
#if __cplusplus >= 202002L
    {"bitlrotate8", static_cast<te_fun2>(te_builtins::te_left_rotate8),
     TE_PURE},
    {"bitrrotate8", static_cast<te_fun2>(te_builtins::te_right_rotate8),
     TE_PURE},
    {"bitlrotate16", static_cast<te_fun2>(te_builtins::te_left_rotate16),
     TE_PURE},
    {"bitrrotate16", static_cast<te_fun2>(te_builtins::te_right_rotate16),
     TE_PURE},
    {"bitlrotate32", static_cast<te_fun2>(te_builtins::te_left_rotate32),
     TE_PURE},
    {"bitrrotate32", static_cast<te_fun2>(te_builtins::te_right_rotate32),
     TE_PURE},
    {"bitlrotate64", static_cast<te_fun2>(te_builtins::te_left_rotate64),
     TE_PURE},
    {"bitrrotate64", static_cast<te_fun2>(te_builtins::te_right_rotate64),
     TE_PURE},
    {"bitlrotate", static_cast<te_fun2>(te_builtins::te_left_rotate), TE_PURE},
    {"bitrrotate", static_cast<te_fun2>(te_builtins::te_right_rotate), TE_PURE},
#endif
    {"bitnot8", static_cast<te_fun1>(te_builtins::te_bitwise_not8), TE_PURE},
    {"bitnot16", static_cast<te_fun1>(te_builtins::te_bitwise_not16), TE_PURE},
    {"bitnot32", static_cast<te_fun1>(te_builtins::te_bitwise_not32), TE_PURE},
    {"bitnot64", static_cast<te_fun1>(te_builtins::te_bitwise_not64), TE_PURE},
    {"bitnot", static_cast<te_fun1>(te_builtins::te_bitwise_not), TE_PURE},
    {"bitlshift", static_cast<te_fun2>(te_builtins::te_left_shift_or_right),
     TE_PURE},
    {"bitrshift", static_cast<te_fun2>(te_builtins::te_right_shift_or_left),
     TE_PURE},
    {"bitxor", static_cast<te_fun2>(te_builtins::te_bitwise_xor), TE_PURE},
#endif
    {"ceil", static_cast<te_fun1>(te_builtins::te_ceil), TE_PURE},
    {"clamp",
     static_cast<te_fun3>([](const te_type num, const te_type start,
                             const te_type end)  // NOLINT
                          {
                              return (start <= end)
                                         ? std::clamp<te_type>(num, start, end)
                                         : std::clamp<te_type>(num, end, start);
                          }),
     TE_PURE},
    {"combin", static_cast<te_fun2>(te_builtins::te_ncr), TE_PURE},
    {"cos", static_cast<te_fun1>(te_builtins::te_cos), TE_PURE},
    {"cosh", static_cast<te_fun1>(te_builtins::te_cosh), TE_PURE},
    {"cot", static_cast<te_fun1>(te_builtins::te_cot), TE_PURE},
    {"e", static_cast<te_fun0>(te_builtins::te_e), TE_PURE},
    {"even", static_cast<te_fun1>(te_builtins::te_even), TE_PURE},
    {"exp", static_cast<te_fun1>(te_builtins::te_exp), TE_PURE},
    {"fac", static_cast<te_fun1>(te_builtins::te_fac), TE_PURE},
    {"fact", static_cast<te_fun1>(te_builtins::te_fac), TE_PURE},
    {"false", static_cast<te_fun0>(te_builtins::te_false_value), TE_PURE},
    {"floor", static_cast<te_fun1>(te_builtins::te_floor), TE_PURE},
    {"iseven", static_cast<te_fun1>(te_builtins::te_is_even), TE_PURE},
    {"isodd", static_cast<te_fun1>(te_builtins::te_is_odd), TE_PURE},
    {"if", static_cast<te_fun3>(te_builtins::te_if), TE_PURE},
    {"ifs", static_cast<te_fun6>(te_builtins::te_ifs),
     static_cast<te_variable_flags>(TE_PURE | TE_VARIADIC)},
    {"ln", static_cast<te_fun1>(te_builtins::te_log), TE_PURE},
    {"log10", static_cast<te_fun1>(te_builtins::te_log10), TE_PURE},
    {"max", static_cast<te_fun7>(te_builtins::te_max),
     static_cast<te_variable_flags>(TE_PURE | TE_VARIADIC)},
    {"maxint", static_cast<te_fun0>(te_builtins::te_max_integer), TE_PURE},
    {"min", static_cast<te_fun7>(te_builtins::te_min),
     static_cast<te_variable_flags>(TE_PURE | TE_VARIADIC)},
    {"mod", static_cast<te_fun2>(te_builtins::te_modulus), TE_PURE},
    {"nan", static_cast<te_fun0>(te_builtins::te_nan_value), TE_PURE},
    {"ncr", static_cast<te_fun2>(te_builtins::te_ncr), TE_PURE},
    {"not", static_cast<te_fun1>(te_builtins::te_not), TE_PURE},
    {"npr", static_cast<te_fun2>(te_builtins::te_npr), TE_PURE},
    {"odd", static_cast<te_fun1>(te_builtins::te_odd), TE_PURE},
    {"or", static_cast<te_fun7>(te_builtins::te_or_variadic),
     static_cast<te_variable_flags>(TE_PURE | TE_VARIADIC)},
    {"permut", static_cast<te_fun2>(te_builtins::te_npr), TE_PURE},
    {"pi", static_cast<te_fun0>(te_builtins::te_pi), TE_PURE},
    {"pow", static_cast<te_fun2>(te_builtins::te_pow), TE_PURE},
    {"power", /* Excel alias*/ static_cast<te_fun2>(te_builtins::te_pow),
     TE_PURE},
    {"rand", static_cast<te_fun0>(te_builtins::te_random), TE_PURE},
    {"round", static_cast<te_fun2>(te_builtins::te_round),
     static_cast<te_variable_flags>(TE_PURE | TE_VARIADIC)},
    {"sign", static_cast<te_fun1>(te_builtins::te_sign), TE_PURE},
    {"sin", static_cast<te_fun1>(te_builtins::te_sin), TE_PURE},
    {"sinh", static_cast<te_fun1>(te_builtins::te_sinh), TE_PURE},
    {"sqr", static_cast<te_fun1>(te_builtins::te_sqr), TE_PURE},
    {"sqrt", static_cast<te_fun1>(te_builtins::te_sqrt), TE_PURE},
    {"sum", static_cast<te_fun7>(te_builtins::te_sum),
     static_cast<te_variable_flags>(TE_PURE | TE_VARIADIC)},
    {"supports32bit", static_cast<te_fun0>(te_builtins::te_supports_32bit),
     TE_PURE},
    {"supports64bit", static_cast<te_fun0>(te_builtins::te_supports_64bit),
     TE_PURE},
    {"tan", static_cast<te_fun1>(te_builtins::te_tan), TE_PURE},
    {"tanh", static_cast<te_fun1>(te_builtins::te_tanh), TE_PURE},
    {"tgamma", static_cast<te_fun1>(te_builtins::te_tgamma), TE_PURE},
    {"true", static_cast<te_fun0>(te_builtins::te_true_value), TE_PURE},
    {"trunc", static_cast<te_fun1>(te_builtins::te_trunc), TE_PURE}};

//--------------------------------------------------
void te_parser::next_token(te_parser::state* theState) {
    assert(theState);
    if (theState == nullptr) {
        return;
    }

    theState->m_type = te_parser::state::token_type::TOK_NULL;

    do  // NOLINT
    {
        if (*theState->m_next == 0) {
            theState->m_type = te_parser::state::token_type::TOK_END;
            return;
        }

        /* Try reading a number. */
        if (((*theState->m_next >= '0') && (*theState->m_next <= '9')) ||
            (*theState->m_next == get_decimal_separator())) {
            char* nEnd{nullptr};
#ifdef TE_FLOAT
            theState->m_value =
                static_cast<te_type>(std::strtof(theState->m_next, &nEnd));
#elif defined(TE_LONG_DOUBLE)
            theState->m_value =
                static_cast<te_type>(std::strtold(theState->m_next, &nEnd));
#else
            theState->m_value =
                static_cast<te_type>(std::strtod(theState->m_next, &nEnd));
#endif
            theState->m_next = nEnd;
            theState->m_type = te_parser::state::token_type::TOK_NUMBER;
        } else {
            /* Look for a variable or builtin function call. */
            if (is_letter(*theState->m_next) || (*theState->m_next == '_')) {
                const char* start = theState->m_next;
                while (is_name_char_valid(*theState->m_next)) {
                    std::advance(theState->m_next, 1);
                }

                m_varFound = false;
                const std::string_view currentVarToken{
                    start, static_cast<std::string::size_type>(
                               theState->m_next - start)};
                m_currentVar = find_lookup(theState, currentVarToken);
                if (m_currentVar != theState->m_lookup.cend()) {
                    m_varFound = true;
                } else {
                    m_currentVar = find_builtin(currentVarToken);
                    if (m_currentVar != m_functions.cend()) {
                        m_varFound = true;
                    }
                    // if unknown symbol resolve is not a no-op, then try using
                    // it to see what this variable is
                    else if (m_unknownSymbolResolve.index() != 0) {
                        try {
                            // "te_type usr(string_view)" resolver
                            if (m_unknownSymbolResolve.index() == 1) {
                                const auto retUsrVal = std::get<1>(
                                    m_unknownSymbolResolve)(currentVarToken);
                                if (std::isfinite(retUsrVal)) {
                                    add_variable_or_function(
                                        {te_variable::name_type{
                                             currentVarToken},
                                         retUsrVal});
                                    m_currentVar =
                                        find_lookup(theState, currentVarToken);
                                    assert(m_currentVar !=
                                               theState->m_lookup.cend() &&
                                           "Internal error in parser using "
                                           "unknown symbol resolver.");
                                    if (m_currentVar !=
                                        theState->m_lookup.cend()) {
                                        m_resolvedVariables.insert(
                                            te_variable::name_type{
                                                currentVarToken});
                                        m_varFound = true;
                                    }
                                }
                            }
                            // "te_type usr(string_view, string&)" resolver
                            else if (m_unknownSymbolResolve.index() == 2) {
                                const auto retUsrVal =
                                    std::get<2>(m_unknownSymbolResolve)(
                                        currentVarToken, m_lastErrorMessage);
                                if (std::isfinite(retUsrVal)) {
                                    add_variable_or_function(
                                        {te_variable::name_type{
                                             currentVarToken},
                                         retUsrVal});
                                    m_currentVar =
                                        find_lookup(theState, currentVarToken);
                                    assert(m_currentVar !=
                                               theState->m_lookup.cend() &&
                                           "Internal error in parser using "
                                           "unknown symbol resolver.");
                                    if (m_currentVar !=
                                        theState->m_lookup.cend()) {
                                        m_resolvedVariables.insert(
                                            te_variable::name_type{
                                                currentVarToken});
                                        m_varFound = true;
                                    }
                                }
                            }
                        } catch (const std::exception& exp) {
                            m_lastErrorMessage = exp.what();
                        }
                    }
                }

                if (!m_varFound) {
                    theState->m_type = te_parser::state::token_type::TOK_ERROR;
                } else {
#ifndef TE_NO_BOOKKEEPING
                    // keep track of what's been used in the formula
                    if (is_function(m_currentVar->m_value) ||
                        is_closure(m_currentVar->m_value)) {
                        m_usedFunctions.insert(m_currentVar->m_name);
                    } else {
                        m_usedVars.insert(m_currentVar->m_name);
                    }
#endif

                    if (is_constant(m_currentVar->m_value)) {
                        theState->m_type =
                            te_parser::state::token_type::TOK_NUMBER;
                        theState->m_value = m_currentVar->m_value;
                    } else if (is_variable(m_currentVar->m_value)) {
                        theState->m_type =
                            te_parser::state::token_type::TOK_VARIABLE;
                        theState->m_value = m_currentVar->m_value;
                    } else if (is_function(m_currentVar->m_value)) {
                        theState->m_type =
                            te_parser::state::token_type::TOK_FUNCTION;
                        theState->m_varType = m_currentVar->m_type;
                        theState->m_value = m_currentVar->m_value;
                    } else if (is_closure(m_currentVar->m_value)) {
                        theState->context = m_currentVar->m_context;
                        theState->m_type =
                            te_parser::state::token_type::TOK_FUNCTION;
                        theState->m_varType = m_currentVar->m_type;
                        theState->m_value = m_currentVar->m_value;
                    }
                }
            } else {
                /* Look for an operator or special character. */
                const auto tok = *theState->m_next;
                std::advance(theState->m_next, 1);
                if (tok == '+') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value = te_builtins::te_add;
                } else if (tok == '-') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value = te_builtins::te_sub;
                }
#ifndef TE_FLOAT
                else if (tok == '~') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value = te_builtins::te_bitwise_not;
                }
#else
                else if (tok == '~') {
                    theState->m_type = te_parser::state::token_type::TOK_ERROR;
                }
#endif
                else if (tok == '*' && (*theState->m_next == '*')) {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_pow);
                    std::advance(theState->m_next, 1);
                } else if (tok == '*') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value = te_builtins::te_mul;
                } else if (tok == '/') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value = te_builtins::te_divide;
                }
#if defined(TE_BITWISE_OPERATORS) && !defined(TE_FLOAT)
                else if (tok == '^') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_bitwise_xor);
                }
#else
                else if (tok == '^') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_pow);
                }
#endif
                else if (tok == '%') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value = te_builtins::te_modulus;
                }
#ifdef TE_BRACKETS_AS_PARENS
                else if (tok == '(' || tok == '[')
#else
                else if (tok == '(')
#endif
                {
                    theState->m_type = te_parser::state::token_type::TOK_OPEN;
                }
#ifdef TE_BRACKETS_AS_PARENS
                else if (tok == ')' || tok == ']')
#else
                else if (tok == ')')
#endif
                {
                    theState->m_type = te_parser::state::token_type::TOK_CLOSE;
                } else if (tok == get_list_separator()) {
                    theState->m_type = te_parser::state::token_type::TOK_SEP;
                }
#if __cplusplus >= 202002L && !defined(TE_FLOAT)
                // rotate (circular shift) operators (uses the 64-bit integer
                // version)
                else if (tok == '<' && (*theState->m_next == '<') &&
                         (*std::next(theState->m_next) == '<')) {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_left_rotate);
                    std::advance(theState->m_next, 2);
                } else if (tok == '>' && (*theState->m_next == '>') &&
                           (*std::next(theState->m_next) == '>')) {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_right_rotate);
                    std::advance(theState->m_next, 2);
                }
#else
                // if rotation is not availble, error if used in formula
                else if ((tok == '<' && (*theState->m_next == '<') &&
                          (*std::next(theState->m_next) == '<')) ||
                         (tok == '>' && (*theState->m_next == '>') &&
                          (*std::next(theState->m_next) == '>'))) {
                    theState->m_type = te_parser::state::token_type::TOK_ERROR;
                }
#endif
#ifndef TE_FLOAT
                // shift operators
                else if (tok == '<' && (*theState->m_next == '<')) {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_left_shift);
                    std::advance(theState->m_next, 1);
                } else if (tok == '>' && (*theState->m_next == '>')) {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_right_shift);
                    std::advance(theState->m_next, 1);
                }
#else
                // shift operators that will be disabled for float types
                else if ((tok == '<' && (*theState->m_next == '<')) ||
                         (tok == '>' && (*theState->m_next == '>'))) {
                    theState->m_type = te_parser::state::token_type::TOK_ERROR;
                }
#endif
                // logical operators
                else if (tok == '=' && (*theState->m_next == '=')) {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_equal);
                    std::advance(theState->m_next, 1);
                } else if (tok == '=') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_equal);
                } else if (tok == '!' && (*theState->m_next == '='))  // NOLINT
                {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_not_equal);
                    std::advance(theState->m_next, 1);
                } else if (tok == '<' && (*theState->m_next == '>')) {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_not_equal);
                    std::advance(theState->m_next, 1);
                } else if (tok == '<' && (*theState->m_next == '=')) {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value = static_cast<te_fun2>(
                        te_builtins::te_less_than_equal_to);
                    std::advance(theState->m_next, 1);
                } else if (tok == '<') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_less_than);
                } else if (tok == '>' && (*theState->m_next == '=')) {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value = static_cast<te_fun2>(
                        te_builtins::te_greater_than_equal_to);
                    std::advance(theState->m_next, 1);
                } else if (tok == '>') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_greater_than);
                } else if (tok == '&' && (*theState->m_next == '&')) {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_and);
                    std::advance(theState->m_next, 1);
                }
#if defined(TE_BITWISE_OPERATORS) && !defined(TE_FLOAT)
                else if (tok == '&') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_bitwise_and);
                }
#else
                else if (tok == '&') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_and);
                }
#endif
                else if (tok == '|' && (*theState->m_next == '|')) {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_or);
                    std::advance(theState->m_next, 1);
                }
#if defined(TE_BITWISE_OPERATORS) && !defined(TE_FLOAT)
                else if (tok == '|') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_bitwise_or);
                }
#else
                else if (tok == '|') {
                    theState->m_type = te_parser::state::token_type::TOK_INFIX;
                    theState->m_value =
                        static_cast<te_fun2>(te_builtins::te_or);
                }
#endif
                else if (tok == ' ' || tok == '\t' || tok == '\n' ||
                         tok == '\r') { /*noop*/
                } else {
                    theState->m_type = te_parser::state::token_type::TOK_ERROR;
                }
            }
        }
    } while (theState->m_type == te_parser::state::token_type::TOK_NULL);
}

//--------------------------------------------------
te_expr* te_parser::base(te_parser::state* theState) {
    /* <base>      =    <constant> | <variable> | <function-0> {"(" ")"} |
       <function-1> <power> | <function-X> "(" <expr> {"," <expr>} ")" | "("
       <list> ")" */
    te_expr* ret{nullptr};

    if (theState->m_type == te_parser::state::token_type::TOK_OPEN) {
        next_token(theState);
        ret = list(theState);
        if (theState->m_type != te_parser::state::token_type::TOK_CLOSE) {
            theState->m_type = te_parser::state::token_type::TOK_ERROR;
        } else {
            next_token(theState);
        }
    } else if (theState->m_type == te_parser::state::token_type::TOK_NUMBER) {
        ret = new_expr(TE_DEFAULT, theState->m_value);
        next_token(theState);
    } else if (theState->m_type == te_parser::state::token_type::TOK_VARIABLE) {
        ret = new_expr(TE_DEFAULT, theState->m_value);
        next_token(theState);
    } else if (theState->m_type == te_parser::state::token_type::TOK_NULL ||
               theState->m_type == te_parser::state::token_type::TOK_ERROR ||
               theState->m_type == te_parser::state::token_type::TOK_END ||
               theState->m_type == te_parser::state::token_type::TOK_SEP ||
               theState->m_type == te_parser::state::token_type::TOK_CLOSE ||
               theState->m_type == te_parser::state::token_type::TOK_INFIX) {
        ret = new_expr(TE_DEFAULT, te_variant_type{te_nan});
        theState->m_type = te_parser::state::token_type::TOK_ERROR;
    } else if (is_function0(theState->m_value) ||
               is_closure0(theState->m_value)) {
        ret = new_expr(theState->m_varType, theState->m_value, {});
        if (is_closure(theState->m_value)) {
            ret->m_parameters[0] = theState->context;
        }
        next_token(theState);
        if (theState->m_type == te_parser::state::token_type::TOK_OPEN) {
            next_token(theState);
            if (theState->m_type != te_parser::state::token_type::TOK_CLOSE) {
                theState->m_type = te_parser::state::token_type::TOK_ERROR;
            } else {
                next_token(theState);
            }
        }
    } else if (is_function1(theState->m_value) ||
               is_closure1(theState->m_value)) {
        ret = new_expr(theState->m_varType, theState->m_value);
        if (is_closure(theState->m_value)) {
            ret->m_parameters[1] = theState->context;
        }
        next_token(theState);
        ret->m_parameters[0] = power(theState);
    } else if (is_function2(theState->m_value) ||
               is_closure2(theState->m_value) ||
               is_function3(theState->m_value) ||
               is_closure3(theState->m_value) ||
               is_function4(theState->m_value) ||
               is_closure4(theState->m_value) ||
               is_function5(theState->m_value) ||
               is_closure5(theState->m_value) ||
               is_function6(theState->m_value) ||
               is_closure6(theState->m_value) ||
               is_function7(theState->m_value) ||
               is_closure7(theState->m_value)) {
        const int arity = get_arity(theState->m_value);

        ret = new_expr(theState->m_varType, theState->m_value);
        if (is_closure(theState->m_value)) {
            ret->m_parameters[arity] = theState->context;
        }
        next_token(theState);

        if (theState->m_type != te_parser::state::token_type::TOK_OPEN) {
            theState->m_type = te_parser::state::token_type::TOK_ERROR;
        } else {
            // If there are vars or other functions in the parameters, keep
            // track of the original opening function; that is what we will do
            // our variadic check on.
            const bool varValid{m_varFound};
            const std::set<te_variable>::const_iterator openingVar =
                m_currentVar;
            // load any parameters
            int i{0};  // NOLINT
            for (i = 0; i < arity; i++) {
                next_token(theState);
                ret->m_parameters[i] = expr_level1(theState);
                if (theState->m_type != te_parser::state::token_type::TOK_SEP) {
                    break;
                }
            }
            if (theState->m_type == te_parser::state::token_type::TOK_CLOSE &&
                (i != arity - 1) && varValid &&
                is_variadic(openingVar->m_type)) {
                next_token(theState);
            } else if (theState->m_type !=
                           te_parser::state::token_type::TOK_CLOSE ||
                       (i != arity - 1)) {
                theState->m_type = te_parser::state::token_type::TOK_ERROR;
            } else {
                next_token(theState);
            }
        }
    }

    return ret;
}

//--------------------------------------------------
te_expr* te_parser::list(te_parser::state* theState) {
    /* <list>      =    <expr> {"," <expr>} */
    te_expr* ret = expr_level1(theState);

    while (theState->m_type == te_parser::state::token_type::TOK_SEP) {
        next_token(theState);
        ret = new_expr(TE_PURE, te_variant_type(te_builtins::te_comma),
                       {ret, expr_level1(theState)});
    }

    return ret;
}

// Operator precedence, lowest to highest:
//--------------------------------------------------
te_expr* te_parser::expr_level1(te_parser::state* theState) {
    /* <expr>      =    <term> {(logic operations) <term>} */
    // These are the lowest of operator precedence
    // (once we have split tokens into arguments)
    te_expr* ret = expr_level2(theState);

    while (theState->m_type == te_parser::state::token_type::TOK_INFIX &&
           is_function2(theState->m_value) &&
           get_function2(theState->m_value) == te_builtins::te_or) {
        const te_fun2 func = get_function2(theState->m_value);
        next_token(theState);
        ret = new_expr(TE_PURE, func, {ret, expr_level2(theState)});
    }

    return ret;
}

//--------------------------------------------------
te_expr* te_parser::expr_level2(te_parser::state* theState) {
    /* <expr>      =    <term> {(logic operations) <term>} */
    // next to lowest in precedence...
    te_expr* ret = expr_level3(theState);

    while (theState->m_type == te_parser::state::token_type::TOK_INFIX &&
           is_function2(theState->m_value) &&
           get_function2(theState->m_value) == te_builtins::te_and) {
        const te_fun2 func = get_function2(theState->m_value);
        next_token(theState);
        ret = new_expr(TE_PURE, func, {ret, expr_level3(theState)});
    }

    return ret;
}

//--------------------------------------------------
te_expr* te_parser::expr_level3(te_parser::state* theState) {
    /* <expr>      =    <term> {(logic operations) <term>} */
    // next to lowest in precedence...
    te_expr* ret = expr_level4(theState);

    while (theState->m_type == te_parser::state::token_type::TOK_INFIX &&
           is_function2(theState->m_value) &&
           get_function2(theState->m_value) == te_builtins::te_bitwise_or) {
        const te_fun2 func = get_function2(theState->m_value);
        next_token(theState);
        ret = new_expr(TE_PURE, func, {ret, expr_level4(theState)});
    }

    return ret;
}

//--------------------------------------------------
te_expr* te_parser::expr_level4(te_parser::state* theState) {
    /* <expr>      =    <term> {(logic operations) <term>} */
    // next to lowest in precedence...
    te_expr* ret = expr_level5(theState);

    while (theState->m_type == te_parser::state::token_type::TOK_INFIX &&
           is_function2(theState->m_value) &&
           get_function2(theState->m_value) == te_builtins::te_bitwise_xor) {
        const te_fun2 func = get_function2(theState->m_value);
        next_token(theState);
        ret = new_expr(TE_PURE, func, {ret, expr_level5(theState)});
    }

    return ret;
}

//--------------------------------------------------
te_expr* te_parser::expr_level5(te_parser::state* theState) {
    /* <expr>      =    <term> {(logic operations) <term>} */
    // next to lowest in precedence...
    te_expr* ret = expr_level6(theState);

    while (theState->m_type == te_parser::state::token_type::TOK_INFIX &&
           is_function2(theState->m_value) &&
           get_function2(theState->m_value) == te_builtins::te_bitwise_and) {
        const te_fun2 func = get_function2(theState->m_value);
        next_token(theState);
        ret = new_expr(TE_PURE, func, {ret, expr_level6(theState)});
    }

    return ret;
}

//--------------------------------------------------
te_expr* te_parser::expr_level6(te_parser::state* theState) {
    /* <expr>      =    <term> {(logic operations) <term>} */
    // next to lowest in precedence...
    te_expr* ret = expr_level7(theState);

    while (theState->m_type == te_parser::state::token_type::TOK_INFIX &&
           is_function2(theState->m_value) &&
           (get_function2(theState->m_value) == te_builtins::te_equal ||
            get_function2(theState->m_value) == te_builtins::te_not_equal)) {
        const te_fun2 func = get_function2(theState->m_value);
        next_token(theState);
        ret = new_expr(TE_PURE, func, {ret, expr_level7(theState)});
    }

    return ret;
}

//--------------------------------------------------
te_expr* te_parser::expr_level7(te_parser::state* theState) {
    /* <expr>      =    <term> {(comparison operators) <term>} */
    te_expr* ret = expr_level8(theState);

    while (theState->m_type == te_parser::state::token_type::TOK_INFIX &&
           is_function2(theState->m_value) &&
           (get_function2(theState->m_value) == te_builtins::te_less_than ||
            get_function2(theState->m_value) ==
                te_builtins::te_less_than_equal_to ||
            get_function2(theState->m_value) == te_builtins::te_greater_than ||
            get_function2(theState->m_value) ==
                te_builtins::te_greater_than_equal_to)) {
        const te_fun2 func = get_function2(theState->m_value);
        next_token(theState);
        ret = new_expr(TE_PURE, func, {ret, expr_level8(theState)});
    }

    return ret;
}

//--------------------------------------------------
te_expr* te_parser::expr_level8(te_parser::state* theState) {
    /* <expr>      =    <term> {("<<" | ">>") <term>} */
    te_expr* ret = expr_level9(theState);

    while (
        theState->m_type == te_parser::state::token_type::TOK_INFIX &&
        is_function2(theState->m_value) &&
        (get_function2(theState->m_value) == te_builtins::te_left_shift ||
         get_function2(theState->m_value) == te_builtins::te_right_shift
#if __cplusplus >= 202002L && !defined(TE_FLOAT)
         || get_function2(theState->m_value) == te_builtins::te_left_rotate ||
         get_function2(theState->m_value) == te_builtins::te_right_rotate ||
         get_function2(theState->m_value) == te_builtins::te_left_rotate32 ||
         get_function2(theState->m_value) == te_builtins::te_right_rotate32 ||
         get_function2(theState->m_value) == te_builtins::te_left_rotate16 ||
         get_function2(theState->m_value) == te_builtins::te_right_rotate16 ||
         get_function2(theState->m_value) == te_builtins::te_left_rotate8 ||
         get_function2(theState->m_value) == te_builtins::te_right_rotate8
#endif
         )) {
        const te_fun2 func = get_function2(theState->m_value);
        next_token(theState);
        ret = new_expr(TE_PURE, func, {ret, expr_level9(theState)});
    }

    return ret;
}

//--------------------------------------------------
te_expr* te_parser::expr_level9(te_parser::state* theState) {
    /* <expr>      =    <term> {("+" | "-") <term>} */
    te_expr* ret = term(theState);

    while (theState->m_type == te_parser::state::token_type::TOK_INFIX &&
           is_function2(theState->m_value) &&
           (get_function2(theState->m_value) == te_builtins::te_add ||
            get_function2(theState->m_value) == te_builtins::te_sub)) {
        const te_fun2 func = get_function2(theState->m_value);
        next_token(theState);
        ret = new_expr(TE_PURE, func, {ret, term(theState)});
    }

    return ret;
}

// Higher levels of operator precendence:
//--------------------------------------------------
te_expr* te_parser::term(te_parser::state* theState) {
    /* <term>      =    <factor> {("*" | "/" | "%") <factor>} */
    // third from highest level of operator precendence
    te_expr* ret = factor(theState);

    while (theState->m_type == te_parser::state::token_type::TOK_INFIX &&
           is_function2(theState->m_value) &&
           (get_function2(theState->m_value) == te_builtins::te_mul ||
            get_function2(theState->m_value) == te_builtins::te_divide ||
            get_function2(theState->m_value) == te_builtins::te_modulus)) {
        const te_fun2 func = get_function2(theState->m_value);
        next_token(theState);
        ret = new_expr(TE_PURE, func, {ret, factor(theState)});
    }

    return ret;
}

//--------------------------------------------------
#ifdef TE_POW_FROM_RIGHT
te_expr* te_parser::factor(te_parser::state* theState) {
    /* <factor>    =    <power> {"^" <power>} */
    // second from highest level of operator precendence
    te_expr* ret = power(theState);

    int neg{0};

    if (ret->m_type == TE_PURE && is_function1(ret->m_value) &&
        get_function1(ret->m_value) == te_builtins::te_negate) {
        te_expr* se = ret->m_parameters[0];
        delete ret;
        ret = se;
        neg = 1;
    }

    te_expr* insertion{nullptr};
    while (theState->m_type == te_parser::state::token_type::TOK_INFIX &&
           is_function2(theState->m_value) &&
           (get_function2(theState->m_value) ==
            static_cast<te_fun2>(te_builtins::te_pow))) {
        const te_fun2 t = get_function2(theState->m_value);
        next_token(theState);

        if (insertion) {
            /* Make exponentiation go right-to-left. */
            te_expr* insert = new_expr(
                TE_PURE, t, {insertion->m_parameters[1], power(theState)});
            insertion->m_parameters[1] = insert;
            insertion = insert;
        } else {
            ret = new_expr(TE_PURE, t, {ret, power(theState)});
            insertion = ret;
        }
    }

    if (neg) {
        ret = new_expr(TE_PURE, te_variant_type(te_builtins::te_negate), {ret});
    }

    return ret;
}
#else
te_expr* te_parser::factor(te_parser::state* theState) {
    /* <factor>    =    <power> {"^" <power>} */
    // second from highest level of operator precendence
    te_expr* ret = power(theState);

    while (theState->m_type == te_parser::state::token_type::TOK_INFIX &&
           is_function2(theState->m_value) &&
           (get_function2(theState->m_value) ==
            static_cast<te_fun2>(te_builtins::te_pow))) {
        const te_fun2 func = get_function2(theState->m_value);
        next_token(theState);
        ret = new_expr(TE_PURE, func, {ret, power(theState)});
    }

    return ret;
}
#endif

//--------------------------------------------------
te_expr* te_parser::power(te_parser::state* theState) {
    /* <power>     =    {("-" | "+")} <base> */
    // highest level of operator precendence
    int theSign{1};
    bool bitwiseNot{false};
    while (theState->m_type == te_parser::state::token_type::TOK_INFIX &&
           ((is_function2(theState->m_value) &&
             (get_function2(theState->m_value) == te_builtins::te_add ||
              get_function2(theState->m_value) == te_builtins::te_sub))
#ifndef TE_FLOAT
            ||
            (is_function1(theState->m_value) &&
             (get_function1(theState->m_value) == te_builtins::te_bitwise_not))
#endif
                )) {
        if (is_function2(theState->m_value) &&
            get_function2(theState->m_value) == te_builtins::te_sub) {
            theSign = -theSign;
        } else if (is_function1(theState->m_value) &&
                   get_function1(theState->m_value) ==
                       te_builtins::te_bitwise_not) {
            bitwiseNot = true;
        }
        next_token(theState);
    }

    te_expr* ret{nullptr};

    if (bitwiseNot) {
        ret = new_expr(TE_PURE, te_variant_type(te_builtins::te_bitwise_not),
                       {base(theState)});
    } else if (theSign == -1) {
        ret = new_expr(TE_PURE, te_variant_type(te_builtins::te_negate),
                       {base(theState)});
    } else {
        ret = base(theState);
    }

    return ret;
}

//--------------------------------------------------
te_type te_parser::te_eval(const te_expr* texp) {
    if (texp == nullptr) {
        return te_nan;
    }

    // NOLINTBEGIN
    // cppcheck-suppress unreadVariable
    const auto M = [&texp = std::as_const(texp)](const size_t e) {
        return (e < texp->m_parameters.size()) ? te_eval(texp->m_parameters[e])
                                               : te_nan;
    };

    switch (texp->m_value.index()) {
        case 0:
            return get_constant(texp->m_value);
        case 1:
            return *(get_variable(texp->m_value));
        case 2:
            return get_function0(texp->m_value)();
        case 3:
            return get_function1(texp->m_value)(M(0));
        case 4:
            return get_function2(texp->m_value)(M(0), M(1));
        case 5:
            return get_function3(texp->m_value)(M(0), M(1), M(2));
        case 6:
            return get_function4(texp->m_value)(M(0), M(1), M(2), M(3));
        case 7:
            return get_function5(texp->m_value)(M(0), M(1), M(2), M(3), M(4));
        case 8:
            return get_function6(texp->m_value)(M(0), M(1), M(2), M(3), M(4),
                                                M(5));
        case 9:
            return get_function7(texp->m_value)(M(0), M(1), M(2), M(3), M(4),
                                                M(5), M(6));
        case 10:
            return get_closure0(texp->m_value)(texp->m_parameters[0]);
        case 11:
            return get_closure1(texp->m_value)(texp->m_parameters[1], M(0));
        case 12:
            return get_closure2(texp->m_value)(texp->m_parameters[2], M(0),
                                               M(1));
        case 13:
            return get_closure3(texp->m_value)(texp->m_parameters[3], M(0),
                                               M(1), M(2));
        case 14:
            return get_closure4(texp->m_value)(texp->m_parameters[4], M(0),
                                               M(1), M(2), M(3));
        case 15:
            return get_closure5(texp->m_value)(texp->m_parameters[5], M(0),
                                               M(1), M(2), M(3), M(4));
        case 16:
            return get_closure6(texp->m_value)(texp->m_parameters[6], M(0),
                                               M(1), M(2), M(3), M(4), M(5));
        case 17:
            return get_closure7(texp->m_value)(texp->m_parameters[7], M(0),
                                               M(1), M(2), M(3), M(4), M(5),
                                               M(6));
        default:
            return te_nan;
    };
    // NOLINTEND
}

//--------------------------------------------------
void te_parser::optimize(te_expr* texp) {
    if (texp == nullptr) {
        return;
    }
    /* Evaluates as much as possible. */
    if (is_constant(texp->m_value) || is_variable(texp->m_value)) {
        return;
    }

    /* Only optimize out functions flagged as pure. */
    if (is_pure(texp->m_type)) {
        const int arity = get_arity(texp->m_value);
        bool known{true};
        for (int i = 0; i < arity; ++i) {
            if (texp->m_parameters[i] == nullptr) {
                break;
            }
            optimize(texp->m_parameters[i]);
            if (!is_constant(texp->m_parameters[i]->m_value)) {
                known = false;
            }
        }
        if (known) {
            const auto value = te_eval(texp);
            te_free_parameters(texp);
            texp->m_type = TE_DEFAULT;
            texp->m_value = value;
        }
    }
}

//--------------------------------------------------
te_expr* te_parser::te_compile(const std::string_view expression,
                               std::set<te_variable>& variables) {
    state theState(expression.data(), TE_DEFAULT, variables);

    next_token(&theState);
    te_expr* root = list(&theState);

    if (theState.m_type != te_parser::state::token_type::TOK_END) {
        te_free(root);
        m_errorPos = (theState.m_next - theState.m_start);
        if (m_errorPos > 0) {
            --m_errorPos;
        }
        return nullptr;
    }

    optimize(root);
    m_errorPos = te_parser::npos;
    return root;
}

//--------------------------------------------------
bool te_parser::compile(const std::string_view expression) {
    reset_state();
    if (get_list_separator() == get_decimal_separator()) {
        throw std::runtime_error(
            "List and decimal separators cannot be the same");
    }
    if (expression.empty()) {
        m_expression.clear();
        m_errorPos = 0;
        return false;
    }
    m_expression.assign(expression);

    // In case the expression was a spreadsheet formula like "=SUM(...)",
    // remove the '=' in front.
    if ((m_expression.length() > 0) && m_expression.front() == '=') {
        m_expression.erase(0, 1);
    }

    size_t commentStart{0};
    while (commentStart != std::string::npos) {
        commentStart = m_expression.find('/', commentStart);
        if (commentStart == std::string::npos ||
            commentStart == m_expression.length() - 1) {
            break;
        }
        // remove multi-line comments
        if (m_expression[commentStart + 1] == '*') {
            auto commentEnd = m_expression.find("*/", commentStart);
            if (commentEnd == std::string::npos) {
                m_errorPos = static_cast<decltype(m_errorPos)>(commentStart);
                m_parseSuccess = false;
                m_result = te_nan;
                return false;
            }
            m_expression.erase(commentStart, (commentEnd + 2) - commentStart);
        }
        // remove single-line comments
        else if (m_expression[commentStart + 1] == '/') {
            auto commentEnd = m_expression.find_first_of("\n\r", commentStart);
            if (commentEnd == std::string::npos) {
                m_expression.erase(commentStart);
                break;
            }
            m_expression.erase(commentStart, commentEnd - commentStart);
        } else {
            ++commentStart;
        }
    }

    try {
        m_compiledExpression =
            te_compile(m_expression, get_variables_and_functions());
        m_parseSuccess = (m_compiledExpression != nullptr);
    } catch (const std::exception& expt) {
        m_parseSuccess = false;
        m_result = te_nan;
        m_lastErrorMessage = expt.what();
    }

    reset_usr_resolved_if_necessary();

    return m_parseSuccess;
}

//--------------------------------------------------
te_type te_parser::evaluate() {
    try {
        m_result = (m_compiledExpression != nullptr)
                       ? te_eval(m_compiledExpression)
                       : te_nan;
    } catch (const std::exception& expt) {
        m_parseSuccess = false;
        m_result = te_nan;
        m_lastErrorMessage = expt.what();
    }

    reset_usr_resolved_if_necessary();

    return m_result;
}

//--------------------------------------------------
te_type te_parser::evaluate(
    const std::string_view
        expression)  // NOLINT(-readability-identifier-naming)
{
    if (compile(expression)) {
        return evaluate();
    }
    return te_nan;
}

//--------------------------------------------------
// cppcheck-suppress unusedFunction
std::string te_parser::list_available_functions_and_variables() {
    std::string report = "Built-in Functions:\n";
    for (const auto& func : m_functions) {
        report.append(func.m_name).append("\n");
    }
    report.append("\nCustom Functions & Variables:\n");
    for (const auto& func : get_variables_and_functions()) {
        report.append(func.m_name).append("\n");
    }
    return report;
}

//--------------------------------------------------
// cppcheck-suppress unusedFunction
std::string te_parser::info() {
    std::string sysInfo{"tinyexpr++ system info:\n=======================\n"};
#ifdef TE_FLOAT
    sysInfo += "Data type:                float\n";
#elif defined(TE_LONG_DOUBLE)
    sysInfo += "Data type:                long double\n";
#else
    sysInfo += "Data type:                double\n";
#endif
    if constexpr (supports_32bit()) {
        sysInfo += "Supports 32-bit integers: yes\n";
    } else {
        sysInfo += "Supports 32-bit integers: no\n";
    }
    if constexpr (supports_64bit()) {
        sysInfo += "Supports 64-bit integers: yes\n";
    } else {
        sysInfo += "Supports 64-bit integers: no\n";
    }
    sysInfo += "Max supported integer:    " +
               std::to_string(static_cast<uint64_t>(get_max_integer())) + "\n";
#ifdef TE_BITWISE_OPERATORS
    sysInfo +=
        "^, &, | operators:        bitwise XOR, bitwise AND, and bitwise OR\n";
#else
    sysInfo +=
        "^, &, | operators:        exponentiation, logical AND, and logical "
        "OR\n";
#endif
#ifdef TE_BRACKETS_AS_PARENS
    sysInfo += "[] are treated as ():     yes\n";
#else
    sysInfo += "[] are treated as ():     no\n";
#endif
#ifdef TE_POW_FROM_RIGHT
    sysInfo += "Exponentiation:           performed right-to-left\n";
#else
    sysInfo += "Exponentiation:           performed left-to-right\n";
#endif
#ifdef TE_NO_BOOKKEEPING
    sysInfo += "Function-use tracking:    disabled\n";
#else
    sysInfo += "Function-use tracking:    enabled\n";
#endif
    return sysInfo;
}
