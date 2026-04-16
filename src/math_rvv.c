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
    size_t i = 0;

    // Reduction accumulator lives in vector register state.
    // Lane 0 holds the running sum across chunks.
    vfloat32m1_t vacc = __riscv_vfmv_v_f_f32m1(0.0f, 1);

    while (i < n) {
        // Chunk size decided by hardware VLEN and remaining element count.
        size_t vl = __riscv_vsetvl_e32m1(n - i);

        // Vector loads for current chunk.
        vfloat32m1_t va = __riscv_vle32_v_f32m1(&a[i], vl);
        vfloat32m1_t vb = __riscv_vle32_v_f32m1(&b[i], vl);

        // Element-wise multiply in vector registers.
        vfloat32m1_t vmul = __riscv_vfmul_vv_f32m1(va, vb, vl);

        // Vector reduction sum: vacc[0] += sum(vmul[0:vl]).
        vacc = __riscv_vfredusum_vs_f32m1_f32m1(vmul, vacc, vl);

        i += vl;
    }

    // Extract the scalar sum from lane 0 of the accumulator vector.
    return __riscv_vfmv_f_s_f32m1_f32(vacc);
}
