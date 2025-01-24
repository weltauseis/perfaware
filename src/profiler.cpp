#include "profiler.h"

#ifdef _WIN32
#include <Windows.h>
#include <intrin.h>

u64 read_os_timer()
{
    LARGE_INTEGER value;
    QueryPerformanceCounter(&value);
    return value.QuadPart;
}

u64 get_os_timer_freq()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return freq.QuadPart;
}

u64 read_cpu_timer()
{
    return __rdtsc();
}

#else // TODO : use the unix functions here

u64 read_os_timer()
{
    return 10;
}

u64 get_os_timer_freq()
{
    return 10;
}

u64 read_cpu_timer()
{
    return 50;
}

#endif

u64 estimate_cpu_freq(u64 millis)
{
    u64 os_freq = get_os_timer_freq();
    u64 os_wait_time = (os_freq * millis) / 1000;
    
    u64 os_start = read_os_timer();
    u64 cpu_start = read_cpu_timer();
    while (read_os_timer() - os_start < os_wait_time)
    {
    }
    u64 os_end = read_os_timer();
    u64 cpu_end = read_cpu_timer();

    u64 os_elapsed = os_end - os_start;
    u64 cpu_elapsed = cpu_end - cpu_start;

    u64 freq = (cpu_elapsed * 1000) / millis;
    return freq;
}

f64 rdtsc_to_seconds(u64 t, u64 freq)
{
    return (f64) t / (f64) freq;
}

f64 rdtsc_to_milliseconds(u64 t, u64 freq)
{
    return (f64) t / (f64) freq * 1000.;
}
