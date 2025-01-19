#include <immintrin.h>
#include <exception>
#include <fstream>
#include <sstream>
#include <vector>
#include "common.h"
#include "json.h"

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

f64 compute_reference_average(char* reference_file_path)
{
    std::ifstream reference_file(reference_file_path, std::ios::binary);

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
    if (argc < 2)
    {
        return 1;
    }

    // it is kinda ridiculous to use two different templated
    // classes just for reading a
    // file into a string... c++...
    std::ifstream input_file(argv[1]);
    if (!input_file)
    {
        printf("Couldn't read input file.\n");
        return 1;
    }
    std::stringstream buffer;
    buffer << input_file.rdbuf();

    JsonNode* data = json_parse(buffer.str());
    // json_pretty_print(data);

    JsonNode* pairs = json_find_node(data, std::string("pairs"));
    if (!pairs)
    {
        printf("No \"pairs\" array in input json file.\n");
        return 1;
    }

    f64 average = compute_average(pairs);
    printf("Average : %f\n", average);

    if (argc == 3)
    {
        f64 reference_average = compute_reference_average(argv[2]);
        printf("Reference : %f\n", reference_average);
    }

    return 0;
}
