// ReSharper disable CppUnnecessaryWhitespace
#ifndef JSON_PARSER_LIBRARY_H
#define JSON_PARSER_LIBRARY_H


#include <string>
#include <utility>
#include <vector>
#include <cctype>
#include <filesystem>
#include <map>
#include <format>
#include <unordered_map>
#include <variant>

// FIXME graph json exporters use capital first letter string versions of true and false, to be fixed



size_t rfind_character(const std::string& str, char symbol, int n);

inline std::string stripWhitespace(std::string str) {
    (void)str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}


namespace json {

    struct JSON_Node;

    // TODO cleanup this enum
    enum json_value_type {UNDEFINED, STRING, INTEGER, FLOAT, DICT, NONE, ARR, BOOLEAN, CHAR, NULL_TYPE};
    inline std::map<json_value_type, std::string> type_map {
        { UNDEFINED, "undefined" },
        { STRING, "str" },
        { INTEGER, "int" },
        { FLOAT, "float" },
        { DICT, "dict" },
        { ARR, "arr" },
        { NONE, "none" },
        { BOOLEAN, "bool" },
        { CHAR, "char" },
        { NULL_TYPE, "null" },
        // {ROOT, "root"}
    };



    struct EmptyValue {};
    using json_entry_datatype = std::variant<int, float, bool, char, std::string, JSON_Node*, EmptyValue*>;
    std::map<int, std::string> json_entry_typenames {
        {0, "int"},
        {1, "float"},
        {2, "bool"},
        {3, "char"},
        {4, "string"},
        {5, "dict"},
        {6, "emptyval"}
    };

    json_entry_datatype convert(std::string value, json_value_type to_type, JSON_Node* node_pointer = nullptr);



    struct JSON_Node {
    std::unordered_map<std::string, std::pair<json_entry_datatype, json_value_type>> value_map;
    int array_iterator = 0;

    void addDictEntry(std::string &key, std::string &val, json_value_type &to_type, JSON_Node *node_pointer) {
        this->value_map[key] = std::make_pair(convert(val, to_type, node_pointer), to_type);
        key = "";
        val = "";
        node_pointer = nullptr;
        to_type = INTEGER;
    }

    void addArrEntry(std::string &val, json_value_type &to_type, JSON_Node *node_pointer) {
        this->value_map[std::to_string( array_iterator++ )] = std::make_pair(convert(val, to_type, node_pointer), to_type);
        val = "";
        node_pointer = nullptr;
        to_type = INTEGER;
    }


    std::string display(int depth = 0) {
        // std::cout << "<At node " << this << ">" << std::endl;
        // std::cout << this->value_map.size() << std::endl;

        std::string result;

        if (!this->value_map.empty()) {
            if (depth!=-1) result += '\n';

            for ( auto [k, val] : value_map ) {
                for (int tab_it = 0; tab_it<depth; ++tab_it) result+='\t';
                if (depth!=-1) {
                    if (val.second != NONE) result += "str(" + k + ") : ";
                    else result += "none(";
                }
                result += type_map[val.second] + "(";

                // std::cout << "first index: " << json_entry_typenames[val.first.index()] << std::endl;
                switch (val.second) {
                    case STRING:
                        // std::cout << "first value(str): " << get<std::string>(val.first) << std::endl;
                        result += get<std::string>(val.first);
                        break;
                    case INTEGER:
                        // std::cout << "first value(int): " << get<int>(val.first) << std::endl;
                        result += std::to_string( get<int>(val.first) );
                        break;
                    case FLOAT:
                        // std::cout << "first value(float): " << get<float>(val.first) << std::endl;
                        result += std::to_string( get<float>(val.first) );
                        break;
                    case BOOLEAN:
                        // std::cout << "first value(bool): " << get<bool>(val.first) << std::endl;
                        if ( get<bool>(val.first) ) result += "true";
                        else result += "false";
                        break;
                        // result += std::to_string( get<bool>(val.first) );
                    case NULL_TYPE:
                        break;
                    case ARR:
                    // case ROOT:
                    case DICT:
                        // std::cout << "first value(dict): " << get<::JSON_Node*>(val.first) << std::endl;
                        result += get<JSON_Node*>(val.first)->display(depth+1);
                        break;
                    default:

                        throw std::runtime_error("Undefined node value type");
                }

                result += "),\n";
            }
            for (int tab_it = 0; tab_it<depth-1; ++tab_it) result+='\t';
            result += "";
        }
        return result;
    }
};



    class JSON_Tree {
        std::string json_directory = "json";
        std::string single_line_json;
        std::vector<std::string> remainder_backtrack;
        JSON_Node* root{};

    public:
        JSON_Tree() = default;

        void setDirectory(const std::string& path);
        void importFile(const std::string& filename);
        void importString(const std::string& str);
        [[nodiscard]] std::string display() const;

        JSON_Node* parse(std::string str, int depth = 0);
        void parse();

    };
}

#endif // JSON_PARSER_LIBRARY_H
