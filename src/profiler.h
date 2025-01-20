#pragma once

#include "better_types.h"

u64 read_os_timer();
u64 get_os_timer_freq();
u64 read_cpu_timer();
u64 estimate_cpu_freq(u64 millis);
