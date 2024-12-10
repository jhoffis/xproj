#include "png_loader.h"
#include "xboxkrnl/xboxkrnl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef NULL
#define NULL 0
#endif
#define UINT_MAX 4294967295
#define INT_MAX 2147483647 
#define PNG_TYPE(a,b,c,d)  (((unsigned) (a) << 24) + ((unsigned) (b) << 16) + ((unsigned) (c) << 8) + (unsigned) (d))
#define MAX_DIMENSIONS (1 << 24)
#ifdef _MSC_VER
#define NOTUSED(v)  (void)(v)
#else
#define NOTUSED(v)  (void)sizeof(v)
#endif
#define MALLOC(sz)           malloc(sz)
#define REALLOC(p,newsz)     realloc(p,newsz)
#define FREE(p)              free(p)
#define REALLOC_SIZED(p,oldsz,newsz) REALLOC(p,newsz)
#define ZFAST_BITS  9 // accelerate all cases in default tables
#define ZFAST_MASK  ((1 << ZFAST_BITS) - 1)
#define ZNSYMS 288 // number of symbols in literal/length alphabet
#define STBI__BYTECAST(x)  ((u8) ((x) & 255))  // truncate int to byte without warnings

typedef struct
{
   int bits_per_channel;
   int num_channels;
} result_info;

typedef struct
{
   int      (*read)  (void *user,char *data,int size);   // fill 'data' with 'size' bytes.  return number of bytes actually read
   void     (*skip)  (void *user,int n);                 // skip the next 'n' bytes, or 'unget' the last -n bytes if negative
   int      (*eof)   (void *user);                       // returns nonzero if we are at end of file/data
} io_callbacks;

// stbi__context structure is our basic context used by all images, so it
// contains all the IO context, plus some basic image information
typedef struct
{
   u32 img_x, img_y;
   i32 img_n, img_out_n;

   io_callbacks io;
   void *io_user_data;

   int read_from_callbacks;
   int buflen;
   u8 buffer_start[128];
   int callback_already_read;

   u8 *img_buffer, *img_buffer_end;
   u8 *img_buffer_original, *img_buffer_original_end;
} context;

typedef struct
{
   context *s;
   u8 *idata, *expanded, *out;
   i32 depth;
} Png;

typedef struct
{
   u32 length;
   u32 type;
} pngchunk;

typedef struct
{
   u16 fast[1 << ZFAST_BITS];
   u16 firstcode[16];
   i32 maxcode[17];
   u16 firstsymbol[16];
   u8  size[ZNSYMS];
   u16 value[ZNSYMS];
} zhuffman;

typedef struct
{
   u8 *zbuffer, *zbuffer_end;
   i32 num_bits;
   i32 hit_zeof_once;
   u32 code_buffer;

   i8 *zout;
   i8 *zout_start;
   i8 *zout_end;
   i32   z_expandable;

   zhuffman z_length, z_distance;
} zbuf;

static const u8 depth_scale_table[9] = { 0, 0xff, 0x55, 0, 0x11, 0,0,0, 0x01 };

static int paeth(int a, int b, int c)
{
   // This formulation looks very different from the reference in the PNG spec, but is
   // actually equivalent and has favorable data dependencies and admits straightforward
   // generation of branch-free code, which helps performance significantly.
   int thresh = c*3 - (a + b);
   int lo = a < b ? a : b;
   int hi = a < b ? b : a;
   int t0 = (hi <= thresh) ? lo : c;
   int t1 = (thresh <= lo) ? hi : t0;
   return t1;
}

// adds an extra all-255 alpha channel
// dest == src is legal
// img_n must be 1 or 3
static void create_png_alpha_expand8(u8 *dest, u8 *src, u32 x, int img_n)
{
   int i;
   // must process data backwards since we allow dest==src
   if (img_n == 1) {
      for (i=x-1; i >= 0; --i) {
         dest[i*2+1] = 255;
         dest[i*2+0] = src[i];
      }
   } else {
      // FIXME STBI_ASSERT(img_n == 3);
      for (i=x-1; i >= 0; --i) {
         dest[i*4+3] = 255;
         dest[i*4+2] = src[i*3+2];
         dest[i*4+1] = src[i*3+1];
         dest[i*4+0] = src[i*3+0];
      }
   }
}

enum {
   STBI__F_none=0,
   STBI__F_sub=1,
   STBI__F_up=2,
   STBI__F_avg=3,
   STBI__F_paeth=4,
   // synthetic filter used for first scanline to avoid needing a dummy row of 0s
   STBI__F_avg_first
};

static u8 first_row_filter[5] =
{
   STBI__F_none,
   STBI__F_sub,
   STBI__F_none,
   STBI__F_avg_first,
   STBI__F_sub // Paeth with b=c=0 turns out to be equivalent to sub
};
// stb_image uses ints pervasively, including for offset calculations.
// therefore the largest decoded image size we can support with the
// current code, even on 64-bit targets, is INT_MAX. this is not a
// significant limitation for the intended use case.
//
// we do, however, need to make sure our size calculations don't
// overflow. hence a few helper functions for size calculations that
// multiply integers together, making sure that they're non-negative
// and no overflow occurs.

// return 1 if the sum is valid, 0 on overflow.
// negative terms are considered invalid.
static int addsizes_valid(int a, int b)
{
   if (b < 0) return 0;
   // now 0 <= b <= INT_MAX, hence also
   // 0 <= INT_MAX - b <= INTMAX.
   // And "a + b <= INT_MAX" (which might overflow) is the
   // same as a <= INT_MAX - b (no overflow)
   return a <= INT_MAX - b;
}

// returns 1 if the product is valid, 0 on overflow.
// negative factors are considered invalid.
static int mul2sizes_valid(int a, int b)
{
   if (a < 0 || b < 0) return 0;
   if (b == 0) return 1; // mul-by-0 is always safe
   // portable way to check for no overflows in a*b
   return a <= INT_MAX/b;
}

// returns 1 if "a*b + add" has no negative terms/factors and doesn't overflow
static int mad2sizes_valid(int a, int b, int add)
{
   return mul2sizes_valid(a, b) && addsizes_valid(a*b, add);
}

// returns 1 if "a*b*c + add" has no negative terms/factors and doesn't overflow
static int mad3sizes_valid(int a, int b, int c, int add)
{
   return mul2sizes_valid(a, b) && mul2sizes_valid(a*b, c) &&
      addsizes_valid(a*b*c, add);
}

// returns 1 if "a*b*c*d + add" has no negative terms/factors and doesn't overflow
static int mad4sizes_valid(int a, int b, int c, int d, int add)
{
   return mul2sizes_valid(a, b) && mul2sizes_valid(a*b, c) &&
      mul2sizes_valid(a*b*c, d) && addsizes_valid(a*b*c*d, add);
}

// mallocs with size overflow checking
static void *malloc_mad2(int a, int b, int add)
{
   if (!mad2sizes_valid(a, b, add)) return NULL;
   return malloc(a*b + add);
}

static void *malloc_mad3(int a, int b, int c, int add)
{
   if (!mad3sizes_valid(a, b, c, add)) return NULL;
   return malloc(a*b*c + add);
}

static void *malloc_mad4(int a, int b, int c, int d, int add)
{
   if (!mad4sizes_valid(a, b, c, d, add)) return NULL;
   return malloc(a*b*c*d + add);
}


static void refill_buffer(context *s)
{
   int n = (s->io.read)(s->io_user_data,(char*)s->buffer_start,s->buflen);
   s->callback_already_read += (int) (s->img_buffer - s->img_buffer_original);
   if (n == 0) {
      // at end of file, treat same as if from memory, but need to handle case
      // where s->img_buffer isn't pointing to safe memory, e.g. 0-byte file
      s->read_from_callbacks = 0;
      s->img_buffer = s->buffer_start;
      s->img_buffer_end = s->buffer_start+1;
      *s->img_buffer = 0;
   } else {
      s->img_buffer = s->buffer_start;
      s->img_buffer_end = s->buffer_start + n;
   }
}

static void skip(context *s, int n)
{
   if (n == 0) return;  // already there!
   if (n < 0) {
      s->img_buffer = s->img_buffer_end;
      return;
   }
   if (s->io.read) {
      int blen = (int) (s->img_buffer_end - s->img_buffer);
      if (blen < n) {
         s->img_buffer = s->img_buffer_end;
         (s->io.skip)(s->io_user_data, n - blen);
         return;
      }
   }
   s->img_buffer += n;
}

// TODO look at this forceinline...
__forceinline static u8 get8(context *s)
{
   if (s->img_buffer < s->img_buffer_end)
      return *s->img_buffer++;
   if (s->read_from_callbacks) {
      refill_buffer(s);
      return *s->img_buffer++;
   }
   return 0;
}

static int get16be(context *s)
{
   int z = get8(s);
   return (z << 8) + get8(s);
}

static u32 get32be(context *s)
{
   u32 z = get16be(s);
   return (z << 16) + get16be(s);
}

static int getn(context *s, u8 *buffer, int n)
{
   if (s->io.read) {
      int blen = (int) (s->img_buffer_end - s->img_buffer);
      if (blen < n) {
         int res, count;

         memcpy(buffer, s->img_buffer, blen);

         count = (s->io.read)(s->io_user_data, (char*) buffer + blen, n - blen);
         res = (count == (n-blen));
         s->img_buffer = s->img_buffer_end;
         return res;
      }
   }

   if (s->img_buffer+n <= s->img_buffer_end) {
      memcpy(buffer, s->img_buffer, n);
      s->img_buffer += n;
      return 1;
   } else
      return 0;
}

static void *stbi__malloc(size_t size)
{
    return MALLOC(size);
}

static pngchunk get_chunk_header(context *s)
{
    pngchunk c;
    c.length = get32be(s);
    c.type   = get32be(s);
    return c;
}

__forceinline static int bitreverse16(int n)
{
  n = ((n & 0xAAAA) >>  1) | ((n & 0x5555) << 1);
  n = ((n & 0xCCCC) >>  2) | ((n & 0x3333) << 2);
  n = ((n & 0xF0F0) >>  4) | ((n & 0x0F0F) << 4);
  n = ((n & 0xFF00) >>  8) | ((n & 0x00FF) << 8);
  return n;
}

__forceinline static int bit_reverse(int v, int bits)
{
   // FIXME assert(bits <= 16);
   // to bit reverse n bits, reverse 16 and shift
   // e.g. 11 bits, bit reverse and shift away 5
   return bitreverse16(v) >> (16-bits);
}

__forceinline static int zeof(zbuf *z)
{
   return (z->zbuffer >= z->zbuffer_end);
}

__forceinline static u8 zget8(zbuf *z)
{
   return zeof(z) ? 0 : *z->zbuffer++;
}

static void fill_bits(zbuf *z)
{
   do {
      if (z->code_buffer >= (1U << z->num_bits)) {
        z->zbuffer = z->zbuffer_end;  /* treat this as EOF so we fail. */
        return;
      }
      z->code_buffer |= (unsigned int) zget8(z) << z->num_bits;
      z->num_bits += 8;
   } while (z->num_bits <= 24);
}

__forceinline static unsigned int zreceive(zbuf *z, int n)
{
   unsigned int k;
   if (z->num_bits < n) fill_bits(z);
   k = z->code_buffer & ((1 << n) - 1);
   z->code_buffer >>= n;
   z->num_bits -= n;
   return k;
}

static int zhuffman_decode_slowpath(zbuf *a, zhuffman *z)
{
   int b,s,k;
   // not resolved by fast table, so compute it the slow way
   // use jpeg approach, which requires MSbits at top
   k = bit_reverse(a->code_buffer, 16);
   for (s=ZFAST_BITS+1; ; ++s)
      if (k < z->maxcode[s])
         break;
   if (s >= 16) return -1; // invalid code!
   // code size is s, so:
   b = (k >> (16-s)) - z->firstcode[s] + z->firstsymbol[s];
   if (b >= ZNSYMS) return -1; // some data was corrupt somewhere!
   if (z->size[b] != s) return -1;  // was originally an assert, but report failure instead.
   a->code_buffer >>= s;
   a->num_bits -= s;
   return z->value[b];
}

__forceinline static int zhuffman_decode(zbuf *a, zhuffman *z)
{
   int b,s;
   if (a->num_bits < 16) {
      if (zeof(a)) {
         if (!a->hit_zeof_once) {
            // This is the first time we hit eof, insert 16 extra padding btis
            // to allow us to keep going; if we actually consume any of them
            // though, that is invalid data. This is caught later.
            a->hit_zeof_once = 1;
            a->num_bits += 16; // add 16 implicit zero bits
         } else {
            // We already inserted our extra 16 padding bits and are again
            // out, this stream is actually prematurely terminated.
            return -1;
         }
      } else {
         fill_bits(a);
      }
   }
   b = z->fast[a->code_buffer & ZFAST_MASK];
   if (b) {
      s = b >> 9;
      a->code_buffer >>= s;
      a->num_bits -= s;
      return b & 511;
   }
   return zhuffman_decode_slowpath(a, z);
}

static int zexpand(zbuf *z, char *zout, int n)  // need to make room for n bytes
{
   char *q;
   unsigned int cur, limit, old_limit;
   z->zout = zout;
   if (!z->z_expandable) return 0; // FIXME stbi__err("output buffer limit","Corrupt PNG");
   cur   = (unsigned int) (z->zout - z->zout_start);
   limit = old_limit = (unsigned) (z->zout_end - z->zout_start);
   if (UINT_MAX - cur < (unsigned) n) return 0; // FIXME stbi__err("outofmem", "Out of memory");
   while (cur + n > limit) {
      if(limit > UINT_MAX / 2) return 0; // FIXME  stbi__err("outofmem", "Out of memory");
      limit *= 2;
   }
   q = (char *) REALLOC_SIZED(z->zout_start, old_limit, limit);
   NOTUSED(old_limit);
   if (q == NULL) return 0; // FIXME  stbi__err("outofmem", "Out of memory");
   z->zout_start = q;
   z->zout       = q + cur;
   z->zout_end   = q + limit;
   return 1;
}

static const int zlength_base[31] = {
   3,4,5,6,7,8,9,10,11,13,
   15,17,19,23,27,31,35,43,51,59,
   67,83,99,115,131,163,195,227,258,0,0 };

static const int zlength_extra[31]=
{ 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0,0,0 };

static const int zdist_base[32] = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,
257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577,0,0};

static const int zdist_extra[32] =
{ 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};

static int parse_huffman_block(zbuf *a)
{
   char *zout = a->zout;
   for(;;) {
      int z = zhuffman_decode(a, &a->z_length);
      if (z < 256) {
         if (z < 0) return 0; // FIXME stbi__err("bad huffman code","Corrupt PNG"); // error in huffman codes
         if (zout >= a->zout_end) {
            if (!zexpand(a, zout, 1)) return 0;
            zout = a->zout;
         }
         *zout++ = (char) z;
      } else {
         u8 *p;
         int len,dist;
         if (z == 256) {
            a->zout = zout;
            if (a->hit_zeof_once && a->num_bits < 16) {
               // The first time we hit zeof, we inserted 16 extra zero bits into our bit
               // buffer so the decoder can just do its speculative decoding. But if we
               // actually consumed any of those bits (which is the case when num_bits < 16),
               // the stream actually read past the end so it is malformed.
               return 0; // FIXME stbi__err("unexpected end","Corrupt PNG");
            }
            return 1;
         }
         if (z >= 286) return 0; // FIXME stbi__err("bad huffman code","Corrupt PNG"); // per DEFLATE, length codes 286 and 287 must not appear in compressed data
         z -= 257;
         len = zlength_base[z];
         if (zlength_extra[z]) len += zreceive(a, zlength_extra[z]);
         z = zhuffman_decode(a, &a->z_distance);
         if (z < 0 || z >= 30) return 0; // FIXME stbi__err("bad huffman code","Corrupt PNG"); // per DEFLATE, distance codes 30 and 31 must not appear in compressed data
         dist = zdist_base[z];
         if (zdist_extra[z]) dist += zreceive(a, zdist_extra[z]);
         if (zout - a->zout_start < dist) return 0; // FIXME stbi__err("bad dist","Corrupt PNG");
         if (len > a->zout_end - zout) {
            if (!zexpand(a, zout, len)) return 0; // FIXME
            zout = a->zout;
         }
         p = (u8 *) (zout - dist);
         if (dist == 1) { // run of one byte; common in images.
            u8 v = *p;
            if (len) { do *zout++ = v; while (--len); }
         } else {
            if (len) { do *zout++ = *p++; while (--len); }
         }
      }
   }
}

static int parse_uncompressed_block(zbuf *a)
{
   u8 header[4];
   int len,nlen,k;
   if (a->num_bits & 7)
      zreceive(a, a->num_bits & 7); // discard
   // drain the bit-packed data into header
   k = 0;
   while (a->num_bits > 0) {
      header[k++] = (u8) (a->code_buffer & 255); // suppress MSVC run-time check
      a->code_buffer >>= 8;
      a->num_bits -= 8;
   }
   if (a->num_bits < 0) return 0; // FIXME stbi__err("zlib corrupt","Corrupt PNG");
   // now fill header the normal way
   while (k < 4)
      header[k++] = zget8(a);
   len  = header[1] * 256 + header[0];
   nlen = header[3] * 256 + header[2];
   if (nlen != (len ^ 0xffff)) return 0; // FIXME stbi__err("zlib corrupt","Corrupt PNG");
   if (a->zbuffer + len > a->zbuffer_end) return 0; // FIXME stbi__err("read past buffer","Corrupt PNG");
   if (a->zout + len > a->zout_end)
      if (!zexpand(a, a->zout, len)) return 0;
   memcpy(a->zout, a->zbuffer, len);
   a->zbuffer += len;
   a->zout += len;
   return 1;
}

static int parse_zlib_header(zbuf *a)
{
   int cmf   = zget8(a);
   int cm    = cmf & 15;
   /* int cinfo = cmf >> 4; */
   int flg   = zget8(a);
   if (zeof(a)) return 0; // FIXME stbi__err("bad zlib header","Corrupt PNG"); // zlib spec
   if ((cmf*256+flg) % 31 != 0) return  0; // FIXME stbi__err("bad zlib header","Corrupt PNG"); // zlib spec
   if (flg & 32) return 0; // FIXME stbi__err("no preset dict","Corrupt PNG"); // preset dictionary not allowed in png
   if (cm != 8) return 0; // FIXME stbi__err("bad compression","Corrupt PNG"); // DEFLATE required for png
   // window = 1 << (8 + cinfo)... but who cares, we fully buffer output
   return 1;
}


static int zbuild_huffman(zhuffman *z, const u8 *sizelist, int num)
{
   int i,k=0;
   int code, next_code[16], sizes[17];

   // DEFLATE spec for generating codes
   memset(sizes, 0, sizeof(sizes));
   memset(z->fast, 0, sizeof(z->fast));
   for (i=0; i < num; ++i)
      ++sizes[sizelist[i]];
   sizes[0] = 0;
   for (i=1; i < 16; ++i)
      if (sizes[i] > (1 << i))
         return  0; // FIXME stbi__err("bad sizes", "Corrupt PNG");
   code = 0;
   for (i=1; i < 16; ++i) {
      next_code[i] = code;
      z->firstcode[i] = (u16) code;
      z->firstsymbol[i] = (u16) k;
      code = (code + sizes[i]);
      if (sizes[i])
         if (code-1 >= (1 << i)) return 0; // FIXME stbi__err("bad codelengths","Corrupt PNG");
      z->maxcode[i] = code << (16-i); // preshift for inner loop
      code <<= 1;
      k += sizes[i];
   }
   z->maxcode[16] = 0x10000; // sentinel
   for (i=0; i < num; ++i) {
      int s = sizelist[i];
      if (s) {
         int c = next_code[s] - z->firstcode[s] + z->firstsymbol[s];
         u16 fastv = (u16) ((s << 9) | i);
         z->size [c] = (u8     ) s;
         z->value[c] = (u16) i;
         if (s <= ZFAST_BITS) {
            int j = bit_reverse(next_code[s],s);
            while (j < (1 << ZFAST_BITS)) {
               z->fast[j] = fastv;
               j += (1 << s);
            }
         }
         ++next_code[s];
      }
   }
   return 1;
}

static int compute_huffman_codes(zbuf *a)
{
   static const u8 length_dezigzag[19] = { 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };
   zhuffman z_codelength;
   u8 lencodes[286+32+137];//padding for maximum single op
   u8 codelength_sizes[19];
   int i,n;

   int hlit  = zreceive(a,5) + 257;
   int hdist = zreceive(a,5) + 1;
   int hclen = zreceive(a,4) + 4;
   int ntot  = hlit + hdist;

   memset(codelength_sizes, 0, sizeof(codelength_sizes));
   for (i=0; i < hclen; ++i) {
      int s = zreceive(a,3);
      codelength_sizes[length_dezigzag[i]] = (u8) s;
   }
   if (!zbuild_huffman(&z_codelength, codelength_sizes, 19)) return 0; // FIXME

   n = 0;
   while (n < ntot) {
      int c = zhuffman_decode(a, &z_codelength);
      if (c < 0 || c >= 19) return 0; // FIXME stbi__err("bad codelengths", "Corrupt PNG");
      if (c < 16)
         lencodes[n++] = (u8) c;
      else {
         u8 fill = 0;
         if (c == 16) {
            c = zreceive(a,2)+3;
            if (n == 0) return 0; // FIXME stbi__err("bad codelengths", "Corrupt PNG");
            fill = lencodes[n-1];
         } else if (c == 17) {
            c = zreceive(a,3)+3;
         } else if (c == 18) {
            c = zreceive(a,7)+11;
         } else {
            return 0; // FIXME stbi__err("bad codelengths", "Corrupt PNG");
         }
         if (ntot - n < c) return 0; // FIXME stbi__err("bad codelengths", "Corrupt PNG");
         memset(lencodes+n, fill, c);
         n += c;
      }
   }
   if (n != ntot) return 0; // FIXME stbi__err("bad codelengths","Corrupt PNG");
   if (!zbuild_huffman(&a->z_length, lencodes, hlit)) return 0;
   if (!zbuild_huffman(&a->z_distance, lencodes+hlit, hdist)) return 0;
   return 1;
}

static const u8 zdefault_length[ZNSYMS] =
{
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8
};
static const u8 zdefault_distance[32] =
{
   5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5
};
/*
Init algorithm:
{
   int i;   // use <= to match clearly with spec
   for (i=0; i <= 143; ++i)     stbi__zdefault_length[i]   = 8;
   for (   ; i <= 255; ++i)     stbi__zdefault_length[i]   = 9;
   for (   ; i <= 279; ++i)     stbi__zdefault_length[i]   = 7;
   for (   ; i <= 287; ++i)     stbi__zdefault_length[i]   = 8;

   for (i=0; i <=  31; ++i)     stbi__zdefault_distance[i] = 5;
}
*/

static int parse_zlib(zbuf *a, int parse_header)
{
   int final, type;
   if (parse_header)
      if (!parse_zlib_header(a)) return 0;
   a->num_bits = 0;
   a->code_buffer = 0;
   a->hit_zeof_once = 0;
   do {
      final = zreceive(a,1);
      type = zreceive(a,2);
      if (type == 0) {
         if (!parse_uncompressed_block(a)) return 0;
      } else if (type == 3) {
         return 0;
      } else {
         if (type == 1) {
            // use fixed code lengths
            if (!zbuild_huffman(&a->z_length  , zdefault_length  , ZNSYMS)) return 0;
            if (!zbuild_huffman(&a->z_distance, zdefault_distance,  32)) return 0;
         } else {
            if (!compute_huffman_codes(a)) return 0;
         }
         if (!parse_huffman_block(a)) return 0;
      }
   } while (!final);
   return 1;
}

static int do_zlib(zbuf *a, char *obuf, int olen, int exp, int parse_header)
{
   a->zout_start = obuf;
   a->zout       = obuf;
   a->zout_end   = obuf + olen;
   a->z_expandable = exp;

   return parse_zlib(a, parse_header);
}



char *zlib_decode_malloc_guesssize_headerflag(const char *buffer, int len, int initial_size, int *outlen, int parse_header)
{
   zbuf a;
   char *p = (char *) stbi__malloc(initial_size);
   if (p == NULL) return NULL;
   a.zbuffer = (u8 *) buffer;
   a.zbuffer_end = (u8 *) buffer + len;
   if (do_zlib(&a, p, initial_size, 1, parse_header)) {
      if (outlen) *outlen = (int) (a.zout - a.zout_start);
      return a.zout_start;
   } else {
      FREE(a.zout_start);
      return NULL;
   }
}

// create the png data from post-deflated data
static int create_png_image_raw(Png *a, u8 *raw, u32 raw_len, int out_n, u32 x, u32 y, int depth, int color)
{
   int bytes = (depth == 16 ? 2 : 1);
   context *s = a->s;
   u32 i,j,stride = x*out_n*bytes;
   u32 img_len, img_width_bytes;
   u8 *filter_buf;
   int all_ok = 1;
   int k;
   int img_n = s->img_n; // copy it into a local for later

   int output_bytes = out_n*bytes;
   int filter_bytes = img_n*bytes;
   int width = x;

   // FIXME STBI_ASSERT(out_n == s->img_n || out_n == s->img_n+1);
   a->out = (u8 *) malloc_mad3(x, y, output_bytes, 0); // extra bytes to write off the end into
   if (!a->out) return 0; // FIXME stbi__err("outofmem", "Out of memory");

   // note: error exits here don't need to clean up a->out individually,
   // stbi__do_png always does on error.
   if (!mad3sizes_valid(img_n, x, depth, 7)) return 0; // FIXME stbi__err("too large", "Corrupt PNG");
   img_width_bytes = (((img_n * x * depth) + 7) >> 3);
   if (!mad2sizes_valid(img_width_bytes, y, img_width_bytes)) return 0; // FIXME stbi__err("too large", "Corrupt PNG");
   img_len = (img_width_bytes + 1) * y;

   // we used to check for exact match between raw_len and img_len on non-interlaced PNGs,
   // but issue #276 reported a PNG in the wild that had extra data at the end (all zeros),
   // so just check for raw_len < img_len always.
   if (raw_len < img_len) return 0; // FIXME stbi__err("not enough pixels","Corrupt PNG");

   // Allocate two scan lines worth of filter workspace buffer.
   filter_buf = (u8 *) malloc_mad2(img_width_bytes, 2, 0);
   if (!filter_buf) return 0; // FIXME stbi__err("outofmem", "Out of memory");

   // Filtering for low-bit-depth images
   if (depth < 8) {
      filter_bytes = 1;
      width = img_width_bytes;
   }

   for (j=0; j < y; ++j) {
      // cur/prior filter buffers alternate
      u8 *cur = filter_buf + (j & 1)*img_width_bytes;
      u8 *prior = filter_buf + (~j & 1)*img_width_bytes;
      u8 *dest = a->out + stride*j;
      int nk = width * filter_bytes;
      int filter = *raw++;

      // check filter type
      if (filter > 4) {
         all_ok = 0; // FIXME stbi__err("invalid filter","Corrupt PNG");
         break;
      }

      // if first row, use special filter that doesn't sample previous row
      if (j == 0) filter = first_row_filter[filter];

      // perform actual filtering
      switch (filter) {
      case STBI__F_none:
         memcpy(cur, raw, nk);
         break;
      case STBI__F_sub:
         memcpy(cur, raw, filter_bytes);
         for (k = filter_bytes; k < nk; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + cur[k-filter_bytes]);
         break;
      case STBI__F_up:
         for (k = 0; k < nk; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + prior[k]);
         break;
      case STBI__F_avg:
         for (k = 0; k < filter_bytes; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + (prior[k]>>1));
         for (k = filter_bytes; k < nk; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + ((prior[k] + cur[k-filter_bytes])>>1));
         break;
      case STBI__F_paeth:
         for (k = 0; k < filter_bytes; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + prior[k]); // prior[k] == stbi__paeth(0,prior[k],0)
         for (k = filter_bytes; k < nk; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + paeth(cur[k-filter_bytes], prior[k], prior[k-filter_bytes]));
         break;
      case STBI__F_avg_first:
         memcpy(cur, raw, filter_bytes);
         for (k = filter_bytes; k < nk; ++k)
            cur[k] = STBI__BYTECAST(raw[k] + (cur[k-filter_bytes] >> 1));
         break;
      }

      raw += nk;

      // expand decoded bits in cur to dest, also adding an extra alpha channel if desired
      if (depth < 8) {
         u8 scale = (color == 0) ? depth_scale_table[depth] : 1; // scale grayscale values to 0..255 range
         u8 *in = cur;
         u8 *out = dest;
         u8 inb = 0;
         u32 nsmp = x*img_n;

         // expand bits to bytes first
         if (depth == 4) {
            for (i=0; i < nsmp; ++i) {
               if ((i & 1) == 0) inb = *in++;
               *out++ = scale * (inb >> 4);
               inb <<= 4;
            }
         } else if (depth == 2) {
            for (i=0; i < nsmp; ++i) {
               if ((i & 3) == 0) inb = *in++;
               *out++ = scale * (inb >> 6);
               inb <<= 2;
            }
         } else {
            // FIXME STBI_ASSERT(depth == 1);
            for (i=0; i < nsmp; ++i) {
               if ((i & 7) == 0) inb = *in++;
               *out++ = scale * (inb >> 7);
               inb <<= 1;
            }
         }

         // insert alpha=255 values if desired
         if (img_n != out_n)
            create_png_alpha_expand8(dest, dest, x, img_n);
      } else if (depth == 8) {
         if (img_n == out_n)
            memcpy(dest, cur, x*img_n);
         else
            create_png_alpha_expand8(dest, cur, x, img_n);
      } else if (depth == 16) {
         // convert the image data from big-endian to platform-native
         u16 *dest16 = (u16*)dest;
         u32 nsmp = x*img_n;

         if (img_n == out_n) {
            for (i = 0; i < nsmp; ++i, ++dest16, cur += 2)
               *dest16 = (cur[0] << 8) | cur[1];
         } else {
            // FIXME STBI_ASSERT(img_n+1 == out_n);
            if (img_n == 1) {
               for (i = 0; i < x; ++i, dest16 += 2, cur += 2) {
                  dest16[0] = (cur[0] << 8) | cur[1];
                  dest16[1] = 0xffff;
               }
            } else {
               // FIXME STBI_ASSERT(img_n == 3);
               for (i = 0; i < x; ++i, dest16 += 4, cur += 6) {
                  dest16[0] = (cur[0] << 8) | cur[1];
                  dest16[1] = (cur[2] << 8) | cur[3];
                  dest16[2] = (cur[4] << 8) | cur[5];
                  dest16[3] = 0xffff;
               }
            }
         }
      }
   }

   FREE(filter_buf);
   if (!all_ok) return 0;

   return 1;
}

static int create_png_image(Png *a, u8 *image_data, u32 image_data_len, int out_n, int depth, int color, int interlaced)
{
   int bytes = (depth == 16 ? 2 : 1);
   int out_bytes = out_n * bytes;
   u8 *final;
   int p;
   if (!interlaced)
      return create_png_image_raw(a, image_data, image_data_len, out_n, a->s->img_x, a->s->img_y, depth, color);

   // de-interlacing
   final = (u8 *) malloc_mad3(a->s->img_x, a->s->img_y, out_bytes, 0);
   if (!final) return 0; // FIXME stbi__err("outofmem", "Out of memory");
   for (p=0; p < 7; ++p) {
      int xorig[] = { 0,4,0,2,0,1,0 };
      int yorig[] = { 0,0,4,0,2,0,1 };
      int xspc[]  = { 8,8,4,4,2,2,1 };
      int yspc[]  = { 8,8,8,4,4,2,2 };
      int i,j,x,y;
      // pass1_x[4] = 0, pass1_x[5] = 1, pass1_x[12] = 1
      x = (a->s->img_x - xorig[p] + xspc[p]-1) / xspc[p];
      y = (a->s->img_y - yorig[p] + yspc[p]-1) / yspc[p];
      if (x && y) {
         u32 img_len = ((((a->s->img_n * x * depth) + 7) >> 3) + 1) * y;
         if (!create_png_image_raw(a, image_data, image_data_len, out_n, x, y, depth, color)) {
            FREE(final);
            return 0;
         }
         for (j=0; j < y; ++j) {
            for (i=0; i < x; ++i) {
               int out_y = j*yspc[p]+yorig[p];
               int out_x = i*xspc[p]+xorig[p];
               memcpy(final + out_y*a->s->img_x*out_bytes + out_x*out_bytes,
                      a->out + (j*x+i)*out_bytes, out_bytes);
            }
         }
         FREE(a->out);
         image_data += img_len;
         image_data_len -= img_len;
      }
   }
   a->out = final;

   return 1;
}

static int compute_transparency(Png *z, u8 tc[3], int out_n)
{
   context *s = z->s;
   u32 i, pixel_count = s->img_x * s->img_y;
   u8 *p = z->out;

   // compute color-based transparency, assuming we've
   // already got 255 as the alpha value in the output
   // FIXME STBI_ASSERT(out_n == 2 || out_n == 4);

   if (out_n == 2) {
      for (i=0; i < pixel_count; ++i) {
         p[1] = (p[0] == tc[0] ? 0 : 255);
         p += 2;
      }
   } else {
      for (i=0; i < pixel_count; ++i) {
         if (p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2])
            p[3] = 0;
         p += 4;
      }
   }
   return 1;
}

static int compute_transparency16(Png *z, u16 tc[3], int out_n)
{
   context *s = z->s;
   u32 i, pixel_count = s->img_x * s->img_y;
   u16 *p = (u16*) z->out;

   // compute color-based transparency, assuming we've
   // already got 65535 as the alpha value in the output
   // FIXME STBI_ASSERT(out_n == 2 || out_n == 4);

   if (out_n == 2) {
      for (i = 0; i < pixel_count; ++i) {
         p[1] = (p[0] == tc[0] ? 0 : 65535);
         p += 2;
      }
   } else {
      for (i = 0; i < pixel_count; ++i) {
         if (p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2])
            p[3] = 0;
         p += 4;
      }
   }
   return 1;
}

static int expand_png_palette(Png *a, u8 *palette, int len, int pal_img_n)
{
   u32 i, pixel_count = a->s->img_x * a->s->img_y;
   u8 *p, *temp_out, *orig = a->out;

   p = (u8 *) malloc_mad2(pixel_count, pal_img_n, 0);
   if (p == NULL) return 0; // FIXME stbi__err("outofmem", "Out of memory");

   // between here and free(out) below, exitting would leak
   temp_out = p;

   if (pal_img_n == 3) {
      for (i=0; i < pixel_count; ++i) {
         int n = orig[i]*4;
         p[0] = palette[n  ];
         p[1] = palette[n+1];
         p[2] = palette[n+2];
         p += 3;
      }
   } else {
      for (i=0; i < pixel_count; ++i) {
         int n = orig[i]*4;
         p[0] = palette[n  ];
         p[1] = palette[n+1];
         p[2] = palette[n+2];
         p[3] = palette[n+3];
         p += 4;
      }
   }
   FREE(a->out);
   a->out = temp_out;

   NOTUSED(len);

   return 1;
}

static int parse_png_file(Png *z, int req_comp)
{
   u8 palette[1024], pal_img_n=0;
   u8 has_trans=0, tc[3]={0};
   u16 tc16[3];
   u32 ioff=0, idata_limit=0, i, pal_len=0;
   int first=1,k,interlace=0, color=0, is_iphone=0;
   context *s = z->s;

   z->expanded = NULL;
   z->idata = NULL;
   z->out = NULL;

   for (;;) {
      pngchunk c = get_chunk_header(s);
      switch (c.type) {
         case PNG_TYPE('C','g','B','I'):
            is_iphone = 1;
            skip(s, c.length);
            break;
         case PNG_TYPE('I','H','D','R'): {
            int comp,filter;
            if (!first) return 0; // FIXME stbi__err("multiple IHDR","Corrupt PNG");
            first = 0;
            if (c.length != 13) return 0; // FIXME  stbi__err("bad IHDR len","Corrupt PNG");
            s->img_x = get32be(s);
            s->img_y = get32be(s);
            if (s->img_y > MAX_DIMENSIONS) return 0; // FIXME  stbi__err("too large","Very large image (corrupt?)");
            if (s->img_x > MAX_DIMENSIONS) return 0; // FIXME stbi__err("too large","Very large image (corrupt?)");
            z->depth = get8(s);  if (z->depth != 1 && z->depth != 2 && z->depth != 4 && z->depth != 8 && z->depth != 16)  return 0; // FIXME stbi__err("1/2/4/8/16-bit only","PNG not supported: 1/2/4/8/16-bit only");
            color = get8(s);  if (color > 6)         return 0; // FIXME stbi__err("bad ctype","Corrupt PNG");
            if (color == 3 && z->depth == 16)                  return 0; // FIXME stbi__err("bad ctype","Corrupt PNG");
            if (color == 3) pal_img_n = 3; else if (color & 1) return 0; // FIXME stbi__err("bad ctype","Corrupt PNG");
            comp  = get8(s);  if (comp) return 0; // FIXME stbi__err("bad comp method","Corrupt PNG");
            filter= get8(s);  if (filter) return 0; // FIXME stbi__err("bad filter method","Corrupt PNG");
            interlace = get8(s); if (interlace>1) return 0; // FIXME stbi__err("bad interlace method","Corrupt PNG");
            if (!s->img_x || !s->img_y) return 0; // FIXME stbi__err("0-pixel image","Corrupt PNG");
            if (!pal_img_n) {
               s->img_n = (color & 2 ? 3 : 1) + (color & 4 ? 1 : 0);
               if ((1 << 30) / s->img_x / s->img_n < s->img_y) return 0; // FIXME stbi__err("too large", "Image too large to decode");
            } else {
               // if paletted, then pal_n is our final components, and
               // img_n is # components to decompress/filter.
               s->img_n = 1;
               if ((1 << 30) / s->img_x / 4 < s->img_y) return 0; // FIXME stbi__err("too large","Corrupt PNG");
            }
            // even with SCAN_header, have to scan to see if we have a tRNS
            break;
         }

         case PNG_TYPE('P','L','T','E'):  {
            if (first) return 0; // FIXME stbi__err("first not IHDR", "Corrupt PNG");
            if (c.length > 256*3) return 0; // FIXME stbi__err("invalid PLTE","Corrupt PNG");
            pal_len = c.length / 3;
            if (pal_len * 3 != c.length) return 0; // FIXME stbi__err("invalid PLTE","Corrupt PNG");
            for (i=0; i < pal_len; ++i) {
               palette[i*4+0] = get8(s);
               palette[i*4+1] = get8(s);
               palette[i*4+2] = get8(s);
               palette[i*4+3] = 255;
            }
            break;
         }

         case PNG_TYPE('t','R','N','S'): {
            if (first) return 0; // FIXME stbi__err("first not IHDR", "Corrupt PNG");
            if (z->idata) return 0; // FIXME stbi__err("tRNS after IDAT","Corrupt PNG");
            if (pal_img_n) {
               if (pal_len == 0) return 0; // FIXME stbi__err("tRNS before PLTE","Corrupt PNG");
               if (c.length > pal_len) return 0; // FIXME stbi__err("bad tRNS len","Corrupt PNG");
               pal_img_n = 4;
               for (i=0; i < c.length; ++i)
                  palette[i*4+3] = get8(s);
            } else {
               if (!(s->img_n & 1)) return 0; // FIXME stbi__err("tRNS with alpha","Corrupt PNG");
               if (c.length != (u32) s->img_n*2) return 0; // FIXME stbi__err("bad tRNS len","Corrupt PNG");
               has_trans = 1;
               if (z->depth == 16) {
                  for (k = 0; k < s->img_n && k < 3; ++k) // extra loop test to suppress false GCC warning
                     tc16[k] = (u16)get16be(s); // copy the values as-is
               } else {
                  for (k = 0; k < s->img_n && k < 3; ++k)
                     tc[k] = (u8)(get16be(s) & 255) * depth_scale_table[z->depth]; // non 8-bit images will be larger
               }
            }
            break;
         }

         case PNG_TYPE('I','D','A','T'): {
            if (first) return 0; // FIXME stbi__err("first not IHDR", "Corrupt PNG");
            if (pal_img_n && !pal_len) return 0; // FIXME stbi__err("no PLTE","Corrupt PNG");
            if (c.length > (1u << 30)) return 0; // FIXME stbi__err("IDAT size limit", "IDAT section larger than 2^30 bytes");
            if ((int)(ioff + c.length) < (int)ioff) return 0;
            if (ioff + c.length > idata_limit) {
               u32 idata_limit_old = idata_limit;
               u8 *p;
               if (idata_limit == 0) idata_limit = c.length > 4096 ? c.length : 4096;
               while (ioff + c.length > idata_limit)
                  idata_limit *= 2;
               NOTUSED(idata_limit_old);
               p = (u8 *) REALLOC_SIZED(z->idata, idata_limit_old, idata_limit); 
               if (p == NULL) return 0; // FIXME return stbi__err("outofmem", "Out of memory");
               z->idata = p;
            }
            if (!getn(s, z->idata+ioff,c.length)) return 0; // FIXME return stbi__err("outofdata","Corrupt PNG");
            ioff += c.length;
            break;
         }

         case PNG_TYPE('I','E','N','D'): {
            u32 raw_len, bpl;
            if (first) return 0; // FIXME stbi__err("first not IHDR", "Corrupt PNG");
            if (z->idata == NULL) return 0; // FIXME stbi__err("no IDAT","Corrupt PNG");
            // initial guess for decoded data size to avoid unnecessary reallocs
            bpl = (s->img_x * z->depth + 7) / 8; // bytes per line, per component
            raw_len = bpl * s->img_y * s->img_n /* pixels */ + s->img_y /* filter mode per row */;
            z->expanded = (u8 *) zlib_decode_malloc_guesssize_headerflag((char *) z->idata, ioff, raw_len, (int *) &raw_len, !is_iphone);
            if (z->expanded == NULL) return 0; // zlib should set error
            FREE(z->idata); z->idata = NULL;
            if ((req_comp == s->img_n+1 && req_comp != 3 && !pal_img_n) || has_trans)
               s->img_out_n = s->img_n+1;
            else
               s->img_out_n = s->img_n;
            if (!create_png_image(z, z->expanded, raw_len, s->img_out_n, z->depth, color, interlace)) return 0;
            if (has_trans) {
               if (z->depth == 16) {
                  if (!compute_transparency16(z, tc16, s->img_out_n)) return 0;
               } else {
                  if (!compute_transparency(z, tc, s->img_out_n)) return 0;
               }
            }
            // if (is_iphone && stbi__de_iphone_flag && s->img_out_n > 2)
            //    stbi__de_iphone(z);
            if (pal_img_n) {
               // pal_img_n == 3 or 4
               s->img_n = pal_img_n; // record the actual colors we had
               s->img_out_n = pal_img_n;
               if (req_comp >= 3) s->img_out_n = req_comp;
               if (!expand_png_palette(z, palette, pal_len, s->img_out_n))
                  return 0;
            } else if (has_trans) {
               // non-paletted image with tRNS -> source image has (constant) alpha
               ++s->img_n;
            }
            FREE(z->expanded); z->expanded = NULL;
            // end of PNG chunk, read and skip CRC
            get32be(s);
            return 1;
         }

         default:
            // if critical, fail
            if (first) return 0; // FIXME stbi__err("first not IHDR", "Corrupt PNG");
            if ((c.type & (1 << 29)) == 0) {
               // #ifndef STBI_NO_FAILURE_STRINGS
               // // not threadsafe
               // static char invalid_chunk[] = "XXXX PNG chunk not known";
               // invalid_chunk[0] = STBI__BYTECAST(c.type >> 24);
               // invalid_chunk[1] = STBI__BYTECAST(c.type >> 16);
               // invalid_chunk[2] = STBI__BYTECAST(c.type >>  8);
               // invalid_chunk[3] = STBI__BYTECAST(c.type >>  0);
               // #endif
               return 0; // FIXME stbi__err(invalid_chunk, "PNG not supported: unknown PNG chunk type");
            }
            skip(s, c.length);
            break;
      }
      // end of PNG chunk, read and skip CRC
      get32be(s);
   }
}

//////////////////////////////////////////////////////////////////////////////
//
//  generic converter from built-in img_n to req_comp
//    individual types do this automatically as much as possible (e.g. jpeg
//    does all cases internally since it needs to colorspace convert anyway,
//    and it never has alpha, so very few cases ). png can automatically
//    interleave an alpha=255 channel, but falls back to this for other cases
//
//  assume data buffer is malloced, so malloc a new one and free that one
//  only failure mode is malloc failing

static u8 compute_y(int r, int g, int b)
{
   return (u8) (((r*77) + (g*150) +  (29*b)) >> 8);
}

static u8 *convert_format(u8 *data, i32 img_n, i32 req_comp, u32 x, u32 y)
{
   int i,j;
   u8 *good;

   if (req_comp == img_n) return data;
   // FIXME STBI_ASSERT(req_comp >= 1 && req_comp <= 4);

   good = (unsigned char *) malloc_mad3(req_comp, x, y, 0);
   if (good == NULL) {
      FREE(data);
      return 0; // FIXME stbi__errpuc("outofmem", "Out of memory");
   }

   for (j=0; j < (int) y; ++j) {
      unsigned char *src  = data + j * x * img_n   ;
      unsigned char *dest = good + j * x * req_comp;

      #define STBI__COMBO(a,b)  ((a)*8+(b))
      #define STBI__CASE(a,b)   case STBI__COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
      // convert source image with img_n components to one with req_comp components;
      // avoid switch per pixel, so use switch per scanline and massive macros
      switch (STBI__COMBO(img_n, req_comp)) {
         STBI__CASE(1,2) { dest[0]=src[0]; dest[1]=255;                                     } break;
         STBI__CASE(1,3) { dest[0]=dest[1]=dest[2]=src[0];                                  } break;
         STBI__CASE(1,4) { dest[0]=dest[1]=dest[2]=src[0]; dest[3]=255;                     } break;
         STBI__CASE(2,1) { dest[0]=src[0];                                                  } break;
         STBI__CASE(2,3) { dest[0]=dest[1]=dest[2]=src[0];                                  } break;
         STBI__CASE(2,4) { dest[0]=dest[1]=dest[2]=src[0]; dest[3]=src[1];                  } break;
         STBI__CASE(3,4) { dest[0]=src[0];dest[1]=src[1];dest[2]=src[2];dest[3]=255;        } break;
         STBI__CASE(3,1) { dest[0]=compute_y(src[0],src[1],src[2]);                   } break;
         STBI__CASE(3,2) { dest[0]=compute_y(src[0],src[1],src[2]); dest[1] = 255;    } break;
         STBI__CASE(4,1) { dest[0]=compute_y(src[0],src[1],src[2]);                   } break;
         STBI__CASE(4,2) { dest[0]=compute_y(src[0],src[1],src[2]); dest[1] = src[3]; } break;
         STBI__CASE(4,3) { dest[0]=src[0];dest[1]=src[1];dest[2]=src[2];                    } break;
         default: 
         // FIXME ASSERT(0); 
         FREE(data); 
         FREE(good); 
         return 0; // FIXME stbi__errpuc("unsupported", "Unsupported format conversion");
      }
      #undef STBI__CASE
   }

   FREE(data);
   return good;
}

static u16 compute_y_16(int r, int g, int b)
{
   return (u16) (((r*77) + (g*150) +  (29*b)) >> 8);
}

static u16 *convert_format16(u16 *data, i32 img_n, i32 req_comp, u32 x, u32 y)
{
   int i,j;
   u16 *good;

   if (req_comp == img_n) return data;
   // FIXME STBI_ASSERT(req_comp >= 1 && req_comp <= 4);

   good = (u16 *) stbi__malloc(req_comp * x * y * 2);
   if (good == NULL) {
      FREE(data);
      return 0; // FIXME (u16 *) stbi__errpuc("outofmem", "Out of memory");
   }

   for (j=0; j < (int) y; ++j) {
      u16 *src  = data + j * x * img_n   ;
      u16 *dest = good + j * x * req_comp;

      #define STBI__COMBO(a,b)  ((a)*8+(b))
      #define STBI__CASE(a,b)   case STBI__COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
      // convert source image with img_n components to one with req_comp components;
      // avoid switch per pixel, so use switch per scanline and massive macros
      switch (STBI__COMBO(img_n, req_comp)) {
         STBI__CASE(1,2) { dest[0]=src[0]; dest[1]=0xffff;                                     } break;
         STBI__CASE(1,3) { dest[0]=dest[1]=dest[2]=src[0];                                     } break;
         STBI__CASE(1,4) { dest[0]=dest[1]=dest[2]=src[0]; dest[3]=0xffff;                     } break;
         STBI__CASE(2,1) { dest[0]=src[0];                                                     } break;
         STBI__CASE(2,3) { dest[0]=dest[1]=dest[2]=src[0];                                     } break;
         STBI__CASE(2,4) { dest[0]=dest[1]=dest[2]=src[0]; dest[3]=src[1];                     } break;
         STBI__CASE(3,4) { dest[0]=src[0];dest[1]=src[1];dest[2]=src[2];dest[3]=0xffff;        } break;
         STBI__CASE(3,1) { dest[0]=compute_y_16(src[0],src[1],src[2]);                   } break;
         STBI__CASE(3,2) { dest[0]=compute_y_16(src[0],src[1],src[2]); dest[1] = 0xffff; } break;
         STBI__CASE(4,1) { dest[0]=compute_y_16(src[0],src[1],src[2]);                   } break;
         STBI__CASE(4,2) { dest[0]=compute_y_16(src[0],src[1],src[2]); dest[1] = src[3]; } break;
         STBI__CASE(4,3) { dest[0]=src[0];dest[1]=src[1];dest[2]=src[2];                       } break;
         default: 
         // FIXME STBI_ASSERT(0); 
         FREE(data); 
         FREE(good); 
         return 0; // FIXME (u16*) stbi__errpuc("unsupported", "Unsupported format conversion");
      }
      #undef STBI__CASE
   }

   FREE(data);
   return good;
}

static void *do_png(Png *p, int *x, int *y, int *n, int req_comp, result_info *ri)
{
   void *result=NULL;
   if (req_comp < 0 || req_comp > 4) return  0; // FIXME stbi__errpuc("bad req_comp", "Internal error");
   if (parse_png_file(p, req_comp)) {
      if (p->depth <= 8)
         ri->bits_per_channel = 8;
      else if (p->depth == 16)
         ri->bits_per_channel = 16;
      else
         return  0; // FIXME stbi__errpuc("bad bits_per_channel", "PNG not supported: unsupported color depth");
      result = p->out;
      p->out = NULL;
      if (req_comp && req_comp != p->s->img_out_n) {
         if (ri->bits_per_channel == 8)
            result = convert_format((u8 *) result, p->s->img_out_n, req_comp, p->s->img_x, p->s->img_y);
         else
            result = convert_format16((u16 *) result, p->s->img_out_n, req_comp, p->s->img_x, p->s->img_y);
         p->s->img_out_n = req_comp;
         if (result == NULL) return result;
      }
      *x = p->s->img_x;
      *y = p->s->img_y;
      if (n) *n = p->s->img_n;
   }
   FREE(p->out);      p->out      = NULL;
   FREE(p->expanded); p->expanded = NULL;
   FREE(p->idata);    p->idata    = NULL;

   return result;
}

static void *png_load(context *s, int *x, int *y, int *comp, int req_comp, result_info *ri)
{
   Png p;
   p.s = s;
   return do_png(&p, x,y,comp,req_comp, ri);
}

static void *load_main(context *s, int *x, int *y, int *comp, int req_comp, result_info *ri, int bpc)
{
   memset(ri, 0, sizeof(*ri)); // make sure it's initialized if we add new fields
   ri->bits_per_channel = 8; // default is 8 so most paths don't have to be changed
   // ri->channel_order = 1; // FIXME STBI_ORDER_RGB; // all current input & output are this, but this is here so we can add BGR order
   ri->num_channels = 0;

   // test the formats with a very explicit header first (at least a FOURCC
   // or distinctive magic number first)


   static const u8 png_sig[8] = { 137,80,78,71,13,10,26,10 };
   for (int i = 0; i < 8; ++i) {
      if (get8(s) != png_sig[i]) {
          return 0; // FIXME stbi__errpuc("bad png sig","Not a PNG");
      }
   }

   return png_load(s,x,y,comp,req_comp, ri);
}

static u8 *load_and_postprocess_8bit(context *s, int *x, int *y, int *comp)
{
   result_info ri;
   void *result = load_main(s, x, y, comp, 4, &ri, 8);

   if (result == NULL)
      return NULL;

   // it is the responsibility of the loaders to make sure we get either 8 or 16 bit.
   // STBI_ASSERT(ri.bits_per_channel == 8 || ri.bits_per_channel == 16);
   //
   // if (ri.bits_per_channel != 8) {
   //    result = stbi__convert_16_to_8((stbi__uint16 *) result, *x, *y, req_comp == 0 ? *comp : req_comp);
   //    ri.bits_per_channel = 8;
   // }

   // @TODO: move stbi__convert_format to here

   // if (stbi__vertically_flip_on_load) {
   //    int channels = req_comp ? req_comp : *comp;
   //    stbi__vertical_flip(result, *x, *y, channels * sizeof(stbi_uc));
   // }

   return (unsigned char *) result;
}

static void start_callbacks(context *s, io_callbacks *c, void *user)
{
   s->io = *c;
   s->io_user_data = user;
   s->buflen = sizeof(s->buffer_start);
   s->read_from_callbacks = 1;
   s->callback_already_read = 0;
   s->img_buffer = s->img_buffer_original = s->buffer_start;
   refill_buffer(s);
   s->img_buffer_original_end = s->img_buffer_end;
}

static int stbi__stdio_read(void *user, char *data, int size)
{
   return (int) fread(data,1,size,(FILE*) user);
}

static void stbi__stdio_skip(void *user, int n)
{
   int ch;
   fseek((FILE*) user, n, SEEK_CUR);
   ch = fgetc((FILE*) user);  /* have to read a byte to reset feof()'s flag */
   if (ch != EOF) {
      ungetc(ch, (FILE *) user);  /* push byte back onto stream if valid. */
   }
}

static int stbi__stdio_eof(void *user)
{
   return feof((FILE*) user) || ferror((FILE *) user);
}

static io_callbacks stdio_callbacks =
{
   stbi__stdio_read,
   stbi__stdio_skip,
   stbi__stdio_eof,
};

static void start_file(context *s, FILE *f)
{
   start_callbacks(s, &stdio_callbacks, (void *) f);
}

static void load_from_file(image_data *img, FILE *f, int *x, int *y, int *comp)
{
   u8 *result;
   context s;
   start_file(&s,f);
   result = load_and_postprocess_8bit(&s,x,y,comp);
   if (result) {
      // need to 'unget' all the characters in the IO buffer
      fseek(f, - (int) (s.img_buffer_end - s.img_buffer), SEEK_CUR);
      // img->length = s.img_out_n;
      img->image = result;
   }
}

static char* path_name(const char *name) {
    char *path = malloc(100);
    strcpy(path, "D:\\");
    strcat(path, name);
    strcat(path, ".png");
    return path;
}

/*
 * Just write the name of the PNG file
 */
image_data load_image(const char *name) {
    image_data img = {0};
    char *fixed_name = path_name(name);
    FILE* file = fopen(fixed_name, "rb"); // requires a persistent char* appearently
    free((void*)fixed_name);
    if (!file) {
        // FIXME! debugPrint("failed at finding image!\n");
        // wait_then_cleanup();
        return img;
    }
    load_from_file(&img, file, (int*) &img.w, (int*) &img.h, &img.comp); // , STBI_rgb_alpha);
    fclose(file);
    if (!img.image) {
        // TODO add exception failure thing to the xbox UI
        //   throw std::runtime_error(
        //       std::string("Failed to load texture at ").append(realPath));
    }
    img.pitch = 4*img.w; // 4 bytes
    for (int i = 0; i < img.pitch * img.h; i += 4) {
        // Small endian flip from RGBA to ARGB (but backwards so memory-wise ABGR)
        u8 r = img.image[i];
        u8 b = img.image[i + 2];
        img.image[i] = b;
        img.image[i + 2] = r;
    }

    void *textureAddr = MmAllocateContiguousMemoryEx(img.pitch * img.h, 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    memcpy(textureAddr, img.image, img.pitch * img.h);
    img.addr26bits = (u32) textureAddr & 0x03ffffff; // Retain the lower 26 bits of the address
    free(img.image);
    img.image = textureAddr;

    return img;
}
