#include "math_ops.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1024
#define ABS_EPSILON 1e-3f
#define REL_EPSILON 5e-4f

static bool almost_equal(float lhs, float rhs) {
    float abs_diff = fabsf(lhs - rhs);
    if (abs_diff <= ABS_EPSILON) {
        return true;
    }

    float scale = fmaxf(fabsf(lhs), fabsf(rhs));
    if (scale < 1.0f) {
        scale = 1.0f;
    }

    return abs_diff <= (REL_EPSILON * scale);
}

static void fill_random(float *x, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        x[i] = ((float)rand() / (float)RAND_MAX) * 100.0f - 50.0f;
    }
}

int main(void) {
    float *a = malloc(sizeof(float) * N);
    float *b = malloc(sizeof(float) * N);
    float *c_scalar = malloc(sizeof(float) * N);
    float *c_rvv = malloc(sizeof(float) * N);

    if (a == NULL || b == NULL || c_scalar == NULL || c_rvv == NULL) {
        fprintf(stderr, "Allocation failed\n");
        free(a);
        free(b);
        free(c_scalar);
        free(c_rvv);
        return 1;
    }

    srand(12345);
    fill_random(a, N);
    fill_random(b, N);

    vec_add_scalar(a, b, c_scalar, N);
    vec_add_rvv(a, b, c_rvv, N);

    float dot_scalar = dot_product_scalar(a, b, N);
    float dot_rvv = dot_product_rvv(a, b, N);

    bool ok = true;

    for (size_t i = 0; i < N; ++i) {
        if (!almost_equal(c_scalar[i], c_rvv[i])) {
            fprintf(stderr, "Vector add mismatch at index %zu: scalar=%f rvv=%f\n", i, c_scalar[i], c_rvv[i]);
            ok = false;
            break;
        }
    }

    if (!almost_equal(dot_scalar, dot_rvv)) {
        fprintf(stderr, "Dot product mismatch: scalar=%f rvv=%f\n", dot_scalar, dot_rvv);
        ok = false;
    }

    if (ok) {
        printf("RVV Verification Passed!\n");
    } else {
        printf("RVV Verification Failed!\n");
    }

    free(a);
    free(b);
    free(c_scalar);
    free(c_rvv);

    return ok ? 0 : 1;
}
