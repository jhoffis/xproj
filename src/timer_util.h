#pragma once
#include "nums.h"

void timer_init(void);
u64 timer_now_millis(void);
u64 timer_now_nanos(void);

void timer_update_delta(void);
f64 timer_delta(void);

