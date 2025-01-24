#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>
#include "common.h"
#include "json.h"
#include "profiler.h"

f64 compute_average(JsonNode* pairs)
{
    f64 sum = 0.0;
    u32 count = 0;

    JsonNode* current_pair = pairs->child;
    while (current_pair != nullptr)
    {
        f64 x0 = json_find_node(current_pair, "x0")->data_float;
        f64 y0 = json_find_node(current_pair, "y0")->data_float;
        f64 x1 = json_find_node(current_pair, "x1")->data_float;
        f64 y1 = json_find_node(current_pair, "y1")->data_float;

        sum += reference_haversine(x0, y0, x1, y1);
        count += 1;
        current_pair = current_pair->sibling;
    }

    return sum / (f64)count;
}

f64 compute_reference_average(std::ifstream& reference_file)
{
    reference_file.seekg(0, std::ios::end);
    u32 size = (u32)reference_file.tellg();
    u32 count = size / sizeof(f64);
    reference_file.seekg(0, std::ios::beg);

    std::vector<f64> distances(count);
    reference_file.read(reinterpret_cast<char*>(distances.data()), size);

    f64 sum = 0.0;
    for (f64 distance : distances)
    {
        sum += distance;
    }

    return sum / (f64)count;
}

int main(int argc, char* argv[])
{
    u64 time_start = read_cpu_timer();

    if (argc < 2)
    {
        return 1;
    }

    // it is kinda ridiculous to use two different templated
    // classes just for reading a
    // file into a string... c++...
    std::ifstream input_file(argv[1], std::ios::binary | std::ios::ate);
    if (!input_file)
    {
        printf("Couldn't read input file.\n");
        return 1;
    }
    u64 input_size = (u64) input_file.tellg();
    input_file.seekg(0, std::ios::beg);
    std::vector<u8> buffer(input_size);
    input_file.read((char *)buffer.data(), input_size);
    u64 time_read = read_cpu_timer();

    JsonNode* data = json_parse(buffer);
    // json_pretty_print(data);

    u64 time_parse = read_cpu_timer();

    JsonNode* pairs = json_find_node(data, std::string("pairs"));
    if (!pairs)
    {
        printf("No \"pairs\" array in input json file.\n");
        return 1;
    }

    u64 time_misc = read_cpu_timer();

    f64 average = compute_average(pairs);
    printf("Average   : %f\n", average);

    u64 time_computation = read_cpu_timer();

    u64 time_check = 0;
    if (argc == 3)
    {
        std::ifstream reference_file(argv[2], std::ios::binary);
        if (!reference_file)
        {
            printf("Couldn't read reference file.\n");
            return 1;
        }
        f64 reference_average = compute_reference_average(reference_file);
        printf("Reference : %f\n", reference_average);
        printf("Diff.     : %f\n", average - reference_average);
        time_check = read_cpu_timer();
    }

    // for(u64 m = 1000; m >= 10; m /= 2)
    // {
    //     printf("CPU Freq : %llu (Estimated in %llu millis.)\n", estimate_cpu_freq(m), m);
    // }

    u64 time_total = read_cpu_timer() - time_start;
    u64 cpu_freq = estimate_cpu_freq(100);
    printf("\nTotal : %.2f s (CPU Freq : %llu)\n", rdtsc_to_seconds(time_total, cpu_freq), cpu_freq);
    printf("-----------------\n");
    printf("\tRead : %.2f ms (%.2f%%)\n", rdtsc_to_milliseconds(time_read - time_start, cpu_freq), ((f64)(time_read - time_start) / (f64)time_total) * 100.);
    printf("\tParse : %.2f ms (%.2f%%)\n", rdtsc_to_milliseconds(time_parse - time_read, cpu_freq), ((f64)(time_parse - time_read) / (f64)time_total) * 100.);
    printf("\tMisc : %.2f ms (%.2f%%)\n", rdtsc_to_milliseconds(time_misc - time_parse, cpu_freq), ((f64)(time_misc - time_parse) / (f64)time_total) * 100.);
    printf("\tComputation : %.2f ms (%.2f%%)\n", rdtsc_to_milliseconds(time_computation - time_misc, cpu_freq), ((f64)(time_computation - time_misc) / (f64)time_total) * 100.);
    if (time_check != 0)
        printf("\tChecking : %.2f ms (%.2f%%)\n", rdtsc_to_milliseconds(time_check - time_computation, cpu_freq), ((f64)(time_check - time_computation) / (f64)time_total) * 100.);

    return 0;
}
