#include <cstdio>
#include <cstdlib>
#include <format>
#include <fstream>
#include <string>
#include <vector>
#include "../better_types.h"
#include "../common.h"

std::vector<f64> generate_uniform(int nb)
{
    std::vector<f64> output = {};
    for (int i = 0; i < nb; i++)
    {
        output.push_back(((f64)rand() / (f64)RAND_MAX) * 360.0 - 180.0); // x0
        output.push_back(((f64)rand() / (f64)RAND_MAX) * 180.0 - -90.0); // y0
        output.push_back(((f64)rand() / (f64)RAND_MAX) * 360.0 - 180.0); // x1
        output.push_back(((f64)rand() / (f64)RAND_MAX) * 180.0 - -90.0); // y1
    }

    return output;
}

std::vector<f64> generate_cluster(int nb)
{
    std::vector<f64> output = {};

    for (int cluster = 0; cluster < 2; cluster++)
    {
        f64 cluster_x = ((f64)rand() / (f64)RAND_MAX) * 360.0 - 180.0;
        f64 cluster_y = ((f64)rand() / (f64)RAND_MAX) * 180.0 - 90.0;

        for (int i = 0; i < (nb / 2); i++)
        {
            output.push_back(cluster_x + ((f64)rand() / (f64)RAND_MAX) * 60.0); // x0
            output.push_back(cluster_y + ((f64)rand() / (f64)RAND_MAX) * 30.0); // y0
            output.push_back(cluster_x + ((f64)rand() / (f64)RAND_MAX) * 60.0); // x1
            output.push_back(cluster_y + ((f64)rand() / (f64)RAND_MAX) * 30.0); // y1
        }
    }

    return output;
}

void run(bool clusters, int nb)
{
    auto coords = clusters ? generate_cluster(nb) : generate_uniform(nb);

    const char* mode_suffix = clusters ? "cluster" : "unif";
    std::string json_filename = std::string("output_") + std::to_string(coords.size() / 4) + "_" + mode_suffix + ".json";
    std::string bin_filename = std::string("output_") + std::to_string(coords.size() / 4) + "_" + mode_suffix + ".bin";

    std::ofstream json_output(json_filename);
    std::ofstream bin_output(bin_filename);

    json_output << "{\"pairs\":[\n";
    for (int i = 0; i < coords.size() / 4; i++)
    {
        json_output << std::format("\t{{ \"x0\":{}, \"y0\":{},\"x1\":{}, \"y1\":{} }} {}\n", coords[i * 4], coords[i * 4 + 1], coords[i * 4 + 2], coords[i * 4 + 3], (i != (coords.size() / 4 - 1) ? "," : ""));
    }
    json_output << "]}";

    f64 total = 0.0;
    for (int i = 0; i < coords.size() / 4; i++)
    {
        f64 dist = reference_haversine(coords[i * 4], coords[i * 4 + 1], coords[i * 4 + 2], coords[i * 4 + 3]);
        total += dist / ((f64)coords.size() / 4.0);
        bin_output.write(reinterpret_cast<char*>(&dist), sizeof(f64));
    }

    printf("expected average: %f", total);
}

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        printf("Usage : generator [uniform/cluster] [random seed] [number of pairs to generate]");
    }

    srand(atoi(argv[2]));
    std::string mode = std::string(argv[1]);
    int nb = atoi(argv[3]);

    if (mode == "uniform")
    {
        run(false, nb);
    }
    else if (mode == "cluster")
    {
        run(true, nb);
    }
    else
    {
        printf("unknown generation mode, should be either uniform or cluster\n");
        return -1;
    }

    return 0;
}
