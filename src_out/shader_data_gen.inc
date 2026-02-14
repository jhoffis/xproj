/* Auto-generated from shader .cg sources - do not edit */

static const u32 g_vs_program_basic[] = {
#include "shaders/basic.vs.inl"
};

static void setup_fragment_shader_basic(u32 **pp) {
    u32 *p = *pp;
#include "shaders/basic.ps.inl"
    *pp = p;
}

static const u32 g_vs_program_terrain[] = {
#include "shaders/terrain.vs.inl"
};

static void setup_fragment_shader_terrain(u32 **pp) {
    u32 *p = *pp;
#include "shaders/terrain.ps.inl"
    *pp = p;
}
