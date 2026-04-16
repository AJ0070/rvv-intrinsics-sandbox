#include "math_ops.h"

void vec_add_scalar(const float *a, const float *b, float *c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

float dot_product_scalar(const float *a, const float *b, size_t n) {
    float acc = 0.0f;

    for (size_t i = 0; i < n; ++i) {
        acc += a[i] * b[i];
    }

    return acc;
}
