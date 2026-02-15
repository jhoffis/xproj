#include <math.h>
#include <assert.h>

double round(double x)
{
    unsigned short fcw;

    __asm__ ("fnstcw %2;"     // backup FPU control word
             "mov %2, %%eax;"
             "andl %3, %2;"   // clear rounding bits in control word ("round to nearest or even" mode)
             "fldcw %2;"      // load modified control word
             "frndint;"       // round the float
             "mov %%eax, %2;"
             "fldcw %2;"      // restore original control word
             : "=t"(x) : "0"(x), "m"(fcw), "r"(~0xc00) : "eax");
    return x;
}

float roundf(float x)
{
    unsigned short fcw;

    __asm__ ("fnstcw %2;"     // backup FPU control word
             "mov %2, %%eax;"
             "andl %3, %2;"   // clear rounding bits in control word ("round to nearest or even" mode)
             "fldcw %2;"      // load modified control word
             "frndint;"       // round the float
             "mov %%eax, %2;"
             "fldcw %2;"      // restore original control word
             : "=t"(x) : "0"(x), "m"(fcw), "r"(~0xc00) : "eax");
    return x;
}

long double roundl(long double x)
{
    unsigned short fcw;

    __asm__ ("fnstcw %2;"     // backup FPU control word
             "mov %2, %%eax;"
             "andl %3, %2;"   // clear rounding bits in control word ("round to nearest or even" mode)
             "fldcw %2;"      // load modified control word
             "frndint;"       // round the float
             "mov %%eax, %2;"
             "fldcw %2;"      // restore original control word
             : "=t"(x) : "0"(x), "m"(fcw), "r"(~0xc00) : "eax");
    return x;
}
