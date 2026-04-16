#include "math_ops.h"

#include <riscv_vector.h>

void vec_add_rvv(const float *a, const float *b, float *c, size_t n) {
    size_t i = 0;

    while (i < n) {
        // Select the largest vector length (VL) supported for e32,m1,
        // but no larger than the remaining elements.
        size_t vl = __riscv_vsetvl_e32m1(n - i);

        // Load VL float32 elements from A and B.
        vfloat32m1_t va = __riscv_vle32_v_f32m1(&a[i], vl);
        vfloat32m1_t vb = __riscv_vle32_v_f32m1(&b[i], vl);

        // Perform vector add: vc = va + vb.
        vfloat32m1_t vc = __riscv_vfadd_vv_f32m1(va, vb, vl);

        // Store result chunk back to C.
        __riscv_vse32_v_f32m1(&c[i], vc, vl);

        i += vl;
    }
}

float dot_product_rvv(const float *a, const float *b, size_t n) {
    if (n == 0) {
        return 0.0f;
    }

    size_t i = 0;
    size_t vlmax = __riscv_vsetvl_e32m1(n);

    // Keep accumulation in vector registers across the entire loop.
    vfloat32m1_t vacc = __riscv_vfmv_v_f_f32m1(0.0f, vlmax);

    while (i < n) {
        // Chunk size decided by hardware VLEN and remaining element count.
        size_t vl = __riscv_vsetvl_e32m1(n - i);

        // Vector loads for current chunk.
        vfloat32m1_t va = __riscv_vle32_v_f32m1(&a[i], vl);
        vfloat32m1_t vb = __riscv_vle32_v_f32m1(&b[i], vl);

        // Vector multiply-accumulate: vacc += va * vb.
        vacc = __riscv_vfmacc_vv_f32m1(vacc, va, vb, vl);

        i += vl;
    }

    // Single ordered reduction at the end to produce final scalar sum.
    vfloat32m1_t vzero = __riscv_vfmv_v_f_f32m1(0.0f, 1);
    vfloat32m1_t vred = __riscv_vfredosum_vs_f32m1_f32m1(vacc, vzero, vlmax);
    return __riscv_vfmv_f_s_f32m1_f32(vred);
}
