#pragma once

#include "better_types.h"

u64 read_os_timer();
u64 get_os_timer_freq();
u64 read_cpu_timer();
u64 estimate_cpu_freq(u64 millis);
f64 rdtsc_to_seconds(u64 t, u64 freq);
f64 rdtsc_to_milliseconds(u64 t, u64 freq);
