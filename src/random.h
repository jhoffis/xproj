#pragma once
#include "nums.h"
// source: https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h


static u32 lehmer64(u64 seed) {
  seed *= 0xda942042e4dd58b5;
  return seed >> 32;
}

static u32 wyrand_lim(u64 seed, u32 lim) {
    u32 ran_num = lehmer64(seed);
    return ran_num - (ran_num / lim) * lim; 
}
