#include "json.h"
#include <cstdio>
#include <cstdlib>

// utilities
bool is_digit_char(char c)
{
    return (c >= '0' && c <= '9') || (c == '-');
}

// parser class
struct JsonParser {
    std::string source;
    u32 current = 0;

    void skip_whitespace()
    {
        while (true)
        {
            char c = source[current];
            if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
                current += 1;
            else
                break;
        }
    }

    std::string scan_string()
    {
        // skip the opening quote
        current += 1;

        u32 count = 0;
        while (source[current + count] != '"')
        {
            count++;
        }

        auto str = source.substr(current, count);

        current += count;
        current += 1;

        return str;
    }

    JsonNode* parse_any()
    {
        skip_whitespace();
        JsonNode* result = nullptr;

        char c = source[current];
        switch (c)
        {
            case '{': {
                result = parse_object();
            }
            break;
            case '[': {
                result = parse_array();
            }
            break;
            default: {
                if (is_digit_char(c))
                {
                    result = parse_number();
                }
            }
        }

        return result;
    }

    JsonNode* parse_array()
    {
        // skip opening bracket
        current += 1;

        JsonNode* array_node = new JsonNode{
            .type = JsonType::ARRAY,
        };

        JsonNode* previous_item = nullptr;
        while (source[current] != ']')
        {
            skip_whitespace();
            JsonNode* current_item = parse_any();

            if (array_node->child == nullptr) // true for first item
            {
                array_node->child = current_item;
            }

            if (previous_item != nullptr) // true for subsequent items
            {
                previous_item->sibling = current_item;
            }

            skip_whitespace();
            if (source[current] == ']')
                continue;

            if (source[current] != ',')
            {
                printf("WARNING : MISSING COMMA SEPARATOR IN ARRAY");
            }
            else
            {
                current += 1;
            }

            previous_item = current_item;
        }

        // skip the closing bracket
        current += 1;

        return array_node;
    }

    JsonNode* parse_object()
    {
        // skip opening brace
        current += 1;

        JsonNode* object_node = new JsonNode{
            .type = JsonType::OBJECT,
        };

        JsonNode* previous_item = nullptr;
        while (source[current] != '}')
        {
            skip_whitespace();
            std::string key = scan_string();
            skip_whitespace();
            if (source[current] != ':')
            {
                printf("WARNING : NO COLON BETWEEN KEY/VALUE IN OBJECT");
            }
            else
            {
                current++;
            }
            skip_whitespace();

            JsonNode* current_item = parse_any();
            current_item->label = key;

            if (object_node->child == nullptr) // true for first item
            {
                object_node->child = current_item;
            }

            if (previous_item != nullptr) // true for subsequent items
            {
                previous_item->sibling = current_item;
            }

            skip_whitespace();
            if (source[current] == '}')
                continue;

            if (source[current] != ',')
            {
                printf("WARNING : MISSING COMMA SEPARATOR IN ARRAY");
            }
            else
            {
                current += 1;
            }

            previous_item = current_item;
        }

        // skip the closing bracket
        current += 1;

        return object_node;
    }

    JsonNode* parse_number()
    {
        u32 start = current;

        // natural part
        while (is_digit_char(source[current]))
            current += 1;

        // eventual decimal part
        if (source[current] == '.')
        {
            current += 1;
            while (is_digit_char(source[current]))
                current += 1;
        }

        f64 number = std::atof(source.substr(start, current - start).c_str());

        return new JsonNode{
            .type = JsonType::NUMBER,
            .data_float = number,
        };
    }
};

JsonNode* json_parse(std::string source) // copy here, should be fine
{
    JsonParser parser = JsonParser{
        .source = std::move(source),
    };

    return parser.parse_any();
}

void json_free(JsonNode* root)
{
    return;
}

void print_indent(u32 depth)
{
    for (int d = 0; d < depth; d++)
    {
        printf("\t");
    }
}

void json_print_recursive(JsonNode* root, u32 depth)
{
    if (root == nullptr)
    {
        printf("PRINT ERROR : NODE IS NULL");
        return;
    }

    JsonNode* node = root;
    while (node != nullptr)
    {
        print_indent(depth);

        // print the label for objects members
        if (!node->label.empty())
        {
            printf("\"%s\" :\n", node->label.c_str());
            print_indent(depth);
        }

        // print the actual value
        switch (node->type)
        {
            case JsonType::NIL: {
                printf("NULL\n");
            }
            break;
            case JsonType::BOOLEAN: {
                printf("%s\n", node->data_bool ? "TRUE" : "FALSE");
            }
            break;
            case JsonType::NUMBER: {
                printf("%.2f\n", node->data_float);
            }
            break;
            case JsonType::STRING: {
                printf("%s\n", node->data_str.c_str());
            }
            break;
            case JsonType::ARRAY: {
                printf("[\n");
                json_print_recursive(node->child, depth + 1);
                print_indent(depth);
                printf("]\n");
            }
            break;
            case JsonType::OBJECT: {
                printf("{\n");
                json_print_recursive(node->child, depth + 1);
                print_indent(depth);
                printf("}\n");
            }
            break;
        }
        node = node->sibling;
    }
}

void json_pretty_print(JsonNode* root)
{
    json_print_recursive(root, 0);
}

JsonNode* json_find_node(JsonNode* root, std::string name)
{
    if (root->label == name)
    {
        return root;
    }

    if (root->child)
    {
        auto node = json_find_node(root->child, name);
        if (node)
            return node;
    }
    
    if (root->sibling)
    {
        auto node = json_find_node(root->sibling, name);
        if (node)
            return node;
    }

    return nullptr;
}
