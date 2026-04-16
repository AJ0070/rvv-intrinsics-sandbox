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
    float acc = 0.0f;
    size_t i = 0;

    while (i < n) {
        // Chunk size decided by hardware VLEN and remaining element count.
        size_t vl = __riscv_vsetvl_e32m1(n - i);

        // Vector loads for current chunk.
        vfloat32m1_t va = __riscv_vle32_v_f32m1(&a[i], vl);
        vfloat32m1_t vb = __riscv_vle32_v_f32m1(&b[i], vl);

        // Element-wise multiply in vector registers.
        vfloat32m1_t vmul = __riscv_vfmul_vv_f32m1(va, vb, vl);

        // Store vector products to a temporary buffer, then accumulate
        // in scalar order to match the scalar baseline exactly.
        float partial[vl];
        __riscv_vse32_v_f32m1(partial, vmul, vl);

        for (size_t j = 0; j < vl; ++j) {
            acc += partial[j];
        }

        i += vl;
    }

    return acc;
}
