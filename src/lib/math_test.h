#pragma once
#include "lib/math.h"
#include "lib/test.h"

static void math_test(Test *test) {
    TEST(f_abs(-12.3f) == 12.3f);
    TEST(f_abs(12.3f) == 12.3f);
    TEST(f_abs(0) == 0);

    TEST(i_abs(12) == 12);
    TEST(i_abs(-12) == 12);
    TEST(i_abs(0) == 0);

    TEST(f_floor(0.0f) == 0);
    TEST(f_floor(0.1f) == 0);
    TEST(f_floor(0.9f) == 0);
    TEST(f_floor(1.0f) == 1);

    TEST(f_floor(-0.0f) == 0);
    TEST(f_floor(-0.1f) == -1);
    TEST(f_floor(-0.9f) == -1);
    TEST(f_floor(-1.0f) == -1);

    TEST(f_round(0.0f) == 0);
    TEST(f_round(0.1f) == 0);
    TEST(f_round(0.5f) == 1);
    TEST(f_round(0.9f) == 1);
    TEST(f_round(1.0f) == 1);
    TEST(f_round(1.4f) == 1);

    TEST(f_round(-0.0f) == 0);
    TEST(f_round(-0.1f) == 0);
    TEST(f_round(-0.5f) == 0);
    TEST(f_round(-0.9f) == -1);
    TEST(f_round(-1.0f) == -1);
    TEST(f_round(-1.4f) == -1);

    TEST(is_near(f_fract(12.345), 0.345));
    TEST(is_near(f_fract(0), 0));
    TEST(is_near(f_fract(-1.12), 0.88));
    TEST(is_near(f_fract(-10.12), 0.88));
    TEST(is_near(f_fract(10.12), 0.12));

    TEST(f_trunc(1.2f) == 1);
    TEST(f_trunc(0.0f) == 0);
    TEST(f_trunc(-1.2f) == -1);

    TEST(f_min(1, 2) == 1);
    TEST(f_min(-2, 1) == -2);
    TEST(f_max(1, 2) == 2);
    TEST(f_max(-2, 1) == 1);

    TEST(is_near(f_step(0), 0));
    TEST(is_near(f_step(1), 1));
    TEST(is_near(f_step(-10), 0));
    TEST(is_near(f_step(10), 1));
    // TEST(is_near(f_step(0.5), 0.5));

    TEST(is_near(f_exp(0), 1.0f));
    // TEST(is_near(f_exp(1), 2.71828f));
    // TEST(is_near(f_exp(2.302585092994046), 10.0f));
    TEST(is_near(f_exp(-1000), 0.0f));

    TEST(is_near(f_clamp(-1000, -10, 10), -10.0f));
    TEST(is_near(f_clamp(1000, -10, 10), 10.0f));
    TEST(is_near(f_clamp(8, -10, 10), 8.0f));
    TEST(is_near(f_clamp(-8, -10, 10), -8.0f));

    TEST(is_near(f_wrap(0, 1, 2), 1.0));
    TEST(is_near(f_wrap(0.123, 1, 2), 1.123));
    TEST(is_near(f_wrap(3.123, 1, 2), 1.123));

    TEST(is_near(f_sin2pi(0.0f / 4.0f), 0));
    TEST(is_near(f_sin2pi(1.0f / 4.0f), 1));
    TEST(is_near(f_sin2pi(2.0f / 4.0f), 0));
    TEST(is_near(f_sin2pi(3.0f / 4.0f), -1));
    TEST(is_near(f_sin2pi(4.0f / 4.0f), 0));

    TEST(is_near(f_cos2pi(0.0f / 4.0f), 1));
    TEST(is_near(f_cos2pi(1.0f / 4.0f), 0));
    TEST(is_near(f_cos2pi(2.0f / 4.0f), -1));
    TEST(is_near(f_cos2pi(3.0f / 4.0f), 0));
    TEST(is_near(f_cos2pi(4.0f / 4.0f), 1));

    TEST(is_near(f_sin(R1), 1));
    TEST(is_near(f_sin(R2), 0));
    TEST(is_near(f_sin(R4), 0));

    TEST(is_near(f_cos(R1), 0));
    TEST(is_near(f_cos(R2), -1));
    TEST(is_near(f_cos(R4), 1));

    TEST(is_near(f_tan(PI / 4), 1.0f));
    TEST(is_near(f_tan(-PI / 4), -1.0f));
    // TEST(f_tan(R1) > 1e6f);
    // TEST(f_tan(R2) < -1e6f);
    // TEST(is_near(f_tan(R4), 1));

    TEST(is_near(f_pow2(0), 1));
    TEST(is_near(f_pow2(1), 2));
    TEST(is_near(f_pow2(-4), 1 / 16.0f));
    TEST(is_near(f_pow2(4), 16.0));

    u32 n = 1024;
    for (u32 i = 0; i < n; ++i) {
        f32 x = (f32)i / (f32)n * 4 - 2;
        // TEST(is_near(f_atan(f_tan(x)), x));
        // TEST(is_near(f_sqrt(x * x), x));
    }
}
