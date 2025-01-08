#include "timer_util.h"
#include "nums.h"
#include "pbkit/pbkit.h"
#include "profileapi.h"
#include "sysinfoapi.h"

static LARGE_INTEGER win_clock_frequency, win_clock_start;
static u64 m_last_loop_time;
static f64 m_delta;

void timer_init(void) {
    QueryPerformanceFrequency(&win_clock_frequency); // Get the frequency of the counter
    QueryPerformanceCounter(&win_clock_start);      // Record start time
}

u64 timer_now_millis(void) {
    SYSTEMTIME now;
    GetSystemTime(&now);
    return now.wMilliseconds;
}

u64 timer_now_nanos(void) {
    LARGE_INTEGER win_clock_end;
    QueryPerformanceCounter(&win_clock_end); // Record end time
    double elapsed = (double)(win_clock_end.QuadPart - win_clock_start.QuadPart) / win_clock_frequency.QuadPart * 1e9; // Convert to nanoseconds
    return (u64) elapsed;
}

void timer_update_delta(void) {
    u64 time = timer_now_nanos();
    u64 delta = time - m_last_loop_time;
    m_last_loop_time = time;
    m_delta = (f64)delta / 1000000000.0;
}

f64 timer_delta(void) {
    return m_delta;
}

void timer_stamp_print(const char *title, LARGE_INTEGER *clock_start) {
    LARGE_INTEGER win_clock_end;
    QueryPerformanceCounter(&win_clock_end); // Record end time
    double elapsed = (double)(win_clock_end.QuadPart - clock_start->QuadPart) / win_clock_frequency.QuadPart * 1e9; // Convert to nanoseconds
    pb_print("%s ns: %d\n", title, (long) elapsed);
}

