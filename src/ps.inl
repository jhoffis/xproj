/* !!RC1.0 (line 11) */
#pragma push_macro("MASK")
#undef MASK
#define MASK(mask, val) (((val) << (__builtin_ffs(mask)-1)) & (mask))

pb_push1(p, NV097_SET_COMBINER_FACTOR0,
    MASK(0xFF000000, 0x00)
    | MASK(0x00FF0000, 0xFF)
    | MASK(0x0000FF00, 0x00)
    | MASK(0x000000FF, 0x00));
p += 2;
pb_push1(p, NV097_SET_SPECULAR_FOG_FACTOR + 0,
    MASK(0xFF000000, 0x00)
    | MASK(0x00FF0000, 0xFF)
    | MASK(0x0000FF00, 0x00)
    | MASK(0x000000FF, 0x00));
p += 2;
pb_push1(p, NV097_SET_COMBINER_COLOR_ICW + 0 * 4,
    MASK(NV097_SET_COMBINER_COLOR_ICW_A_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_COLOR_ICW_A_ALPHA, 1) | MASK(NV097_SET_COMBINER_COLOR_ICW_A_MAP, 0x0)
    | MASK(NV097_SET_COMBINER_COLOR_ICW_B_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_COLOR_ICW_B_ALPHA, 1) | MASK(NV097_SET_COMBINER_COLOR_ICW_B_MAP, 0x0)
    | MASK(NV097_SET_COMBINER_COLOR_ICW_C_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_COLOR_ICW_C_ALPHA, 1) | MASK(NV097_SET_COMBINER_COLOR_ICW_C_MAP, 0x0)
    | MASK(NV097_SET_COMBINER_COLOR_ICW_D_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_COLOR_ICW_D_ALPHA, 1) | MASK(NV097_SET_COMBINER_COLOR_ICW_D_MAP, 0x0));
p += 2;
pb_push1(p, NV097_SET_COMBINER_COLOR_OCW + 0 * 4,
    MASK(NV097_SET_COMBINER_COLOR_OCW_AB_DST, 0x0)
    | MASK(NV097_SET_COMBINER_COLOR_OCW_CD_DST, 0x0)
    | MASK(NV097_SET_COMBINER_COLOR_OCW_SUM_DST, 0x0)
    | MASK(NV097_SET_COMBINER_COLOR_OCW_MUX_ENABLE, 0)
    | MASK(NV097_SET_COMBINER_COLOR_OCW_AB_DOT_ENABLE, 0)
    | MASK(NV097_SET_COMBINER_COLOR_OCW_CD_DOT_ENABLE, 0)
    | MASK(NV097_SET_COMBINER_COLOR_OCW_OP, NV097_SET_COMBINER_COLOR_OCW_OP_NOSHIFT));
p += 2;
pb_push1(p, NV097_SET_COMBINER_ALPHA_ICW + 0 * 4,
    MASK(NV097_SET_COMBINER_ALPHA_ICW_A_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_ALPHA_ICW_A_ALPHA, 1) | MASK(NV097_SET_COMBINER_ALPHA_ICW_A_MAP, 0x0)
    | MASK(NV097_SET_COMBINER_ALPHA_ICW_B_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_ALPHA_ICW_B_ALPHA, 1) | MASK(NV097_SET_COMBINER_ALPHA_ICW_B_MAP, 0x0)
    | MASK(NV097_SET_COMBINER_ALPHA_ICW_C_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_ALPHA_ICW_C_ALPHA, 1) | MASK(NV097_SET_COMBINER_ALPHA_ICW_C_MAP, 0x0)
    | MASK(NV097_SET_COMBINER_ALPHA_ICW_D_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_ALPHA_ICW_D_ALPHA, 1) | MASK(NV097_SET_COMBINER_ALPHA_ICW_D_MAP, 0x0));
p += 2;
pb_push1(p, NV097_SET_COMBINER_ALPHA_OCW + 0 * 4,
    MASK(NV097_SET_COMBINER_ALPHA_OCW_AB_DST, 0x0)
    | MASK(NV097_SET_COMBINER_ALPHA_OCW_CD_DST, 0x0)
    | MASK(NV097_SET_COMBINER_ALPHA_OCW_SUM_DST, 0x0)
    | MASK(NV097_SET_COMBINER_ALPHA_OCW_MUX_ENABLE, 0)
    | MASK(NV097_SET_COMBINER_ALPHA_OCW_OP, NV097_SET_COMBINER_ALPHA_OCW_OP_NOSHIFT));
p += 2;
pb_push1(p, NV097_SET_COMBINER_CONTROL,
    MASK(NV097_SET_COMBINER_CONTROL_FACTOR0, NV097_SET_COMBINER_CONTROL_FACTOR0_SAME_FACTOR_ALL)
    | MASK(NV097_SET_COMBINER_CONTROL_FACTOR1, NV097_SET_COMBINER_CONTROL_FACTOR1_SAME_FACTOR_ALL)
    | MASK(NV097_SET_COMBINER_CONTROL_ITERATION_COUNT, 1));
p += 2;
pb_push1(p, NV097_SET_COMBINER_SPECULAR_FOG_CW0,
    MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_ALPHA, 0) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_INVERSE, 0)
    | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_ALPHA, 0) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_INVERSE, 0)
    | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_ALPHA, 0) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_INVERSE, 0)
    | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE, 0x1) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_ALPHA, 0) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_INVERSE, 0));
p += 2;
pb_push1(p, NV097_SET_COMBINER_SPECULAR_FOG_CW1,
    MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW1_E_ALPHA, 0) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW1_E_INVERSE, 0)
    | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW1_F_ALPHA, 0) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW1_F_INVERSE, 0)
    | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE, 0x0) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW1_G_ALPHA, 1) | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW1_G_INVERSE, 1)
    | MASK(NV097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_CLAMP, 0));
p += 2;

#pragma pop_macro("MASK")
