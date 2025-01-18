#include "json.h"
#include <fstream>
#include <sstream>

int main(int argc, char* argv[])
{
    if(argc < 2) {
        return 1;
    }

    // it is kinda ridiculous to use two different templated
    // classes just for reading a
    // file into a string... c++...
    std::ifstream input_file(argv[1]);
    if(!input_file){
        printf("Couldn't read input file.\n");
        return 1;
    }
    std::stringstream buffer;
    buffer << input_file.rdbuf();
    
    JsonNode* test = json_parse(buffer.str());
    json_pretty_print(test);
    
    return 0;
}
