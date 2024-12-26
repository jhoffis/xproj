#pragma once
#include "nums.h"
// source: https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h

static inline u64 _wyrot(u64 x) { 
    return (x>>32)|(x<<32); 
}

static inline void _wymum(u64 *A, u64 *B){
    u64 hh=(*A>>32)*(*B>>32), hl=(*A>>32)*(u32)*B, lh=(u32)*A*(*B>>32), ll=(u64)(u32)*A*(u32)*B;
    *A^=_wyrot(hl)^hh; *B^=_wyrot(lh)^ll;
}

static inline u64 _wymix(u64 A, u64 B){ 
    _wymum(&A,&B); 
    return A^B; 
}

/*
 * Use this one to reuse the same seed!
 */
static inline u64 wyrand(u64 *seed) { 
    *seed+=0x2d358dccaa6c78a5ull; 
    return _wymix(*seed,*seed^0x8bb84b93962eacc9ull);
}

/*
 * Copies the seed. Use this if you just need one or two random numbers.
 */
static inline u64 wyrand_copy(u64 seed) {
    u64 ran_num = wyrand(&seed);
    return ran_num;
}

/*
 * https://en.wikipedia.org/wiki/Lehmer_random_number_generator
 * TODO add a seeded that returns an array
 */
// static inline u32 lehmer64_seeded(u64 seed) {
//   seed *= 0xda942042e4dd58b5;
//   return seed >> 32;
// }

static inline u32 lehmer32_seeded(u64 seed) {
  seed *= 0xda942042e4dd58b5;
  return seed >> 32;
}

