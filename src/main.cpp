#include <fstream>
#include <sstream>
#include "common.h"
#include "json.h"

f64 compute_average(JsonNode* pairs)
{
    f64 average = 0.0;
    u32 count = 0;

    JsonNode* current_pair = pairs->child;
    while (current_pair != nullptr)
    {
        f64 x0 = json_find_node(current_pair, "x0")->data_float;
        f64 y0 = json_find_node(current_pair, "y0")->data_float;
        f64 x1 = json_find_node(current_pair, "x1")->data_float;
        f64 y1 = json_find_node(current_pair, "y1")->data_float;

        average += reference_haversine(x0, y0, x1, y1);
        count += 1;
        current_pair = current_pair->sibling;
    }

    return average / (f64)count;
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
        printf("No \"pairs\" array in input json file.");
        return 1;
    }

    f64 average = compute_average(pairs);
    printf("Average : %f", average);

    return 0;
}
