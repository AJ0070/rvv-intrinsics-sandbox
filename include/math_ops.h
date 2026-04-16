#ifndef MATH_OPS_H
#define MATH_OPS_H

#include <stddef.h>

void vec_add_scalar(const float *a, const float *b, float *c, size_t n);
float dot_product_scalar(const float *a, const float *b, size_t n);

void vec_add_rvv(const float *a, const float *b, float *c, size_t n);
float dot_product_rvv(const float *a, const float *b, size_t n);

#endif
