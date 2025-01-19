#pragma once

#include <string>
#include "better_types.h"

/*
TODO : currently doesn't support :
- strings
- booleans
- null
since it isn't strictly required for the exercice
*/

enum class JsonType {
    OBJECT,
    ARRAY,
    STRING,
    NUMBER,
    BOOLEAN,
    NIL,
};

struct JsonNode {
    JsonType type;
    std::string label;
    JsonNode* child;
    JsonNode* sibling;
    std::string data_str;
    f64 data_float;
    bool data_bool;
};

JsonNode* json_parse(std::string source);
void json_free(JsonNode* root);
void json_pretty_print(JsonNode* root);
JsonNode* json_find_node(JsonNode* root, std::string name);
