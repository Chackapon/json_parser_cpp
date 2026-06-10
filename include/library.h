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
#include <iostream>
#include <unordered_map>
#include <variant>

// FIXME graph json exporters use capital first letter string versions of true and false, to be fixed



size_t rfind_character(const std::string& str, char symbol, int n);

inline std::string stripWhitespace(std::string str) {
    (void)str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}


namespace json {

    struct JSON_Object;

    // TODO cleanup this enum
    enum json_value_type {INTEGER, FLOAT, BOOLEAN, STRING, NULL_TYPE, DICT_TYPE, ARR, NONE, UNDEFINED};
    inline std::map<json_value_type, std::string> type_map {
        { UNDEFINED, "undefined" },
        { STRING, "str" },
        { INTEGER, "int" },
        { FLOAT, "float" },
        { DICT_TYPE, "dict" },
        { ARR, "arr" },
        { NONE, "none" },
        { BOOLEAN, "bool" },
        { NULL_TYPE, "null" }
    };



    struct JSON_Null {};
    struct JSON_RootKey {};
    using json_entry_datatype = std::variant<int, float, bool, std::string, JSON_Null*, JSON_Object*>;
    inline std::map<int, std::string> json_entry_typenames {
        {0, "int"},
        {1, "float"},
        {2, "bool"},
        {3, "string"},
        {4, "null"},
        {5, "dict"},
        {6, "arr"},
    };



    using JSON_Key = std::variant<int, std::string>;
    struct JSON_Value {
        json_entry_datatype value;
        json_value_type type;

        JSON_Value( std::string& str_value, json_value_type to_type, JSON_Object* node_pointer = nullptr );
        ~JSON_Value();
    };

    struct JSON_Node { };

    using json_return_type = std::variant<JSON_Object, json_entry_datatype>;

    struct JSON_Object {
        std::unordered_map<std::string, JSON_Value*> value_map;
        int array_iterator = 0;

        void addDictEntry(std::string &key, std::string &val, json_value_type &to_type, JSON_Object *node_pointer);
        void addArrEntry( std::string &val, json_value_type &to_type, JSON_Object *node_pointer);

        JSON_Object operator[]( const std::string& key ) {
            std::cout << "[JSON_Node] entered key: "+key << std::endl;
            auto requested_entry_it = this->value_map.find(key);

            for (auto e : this->value_map) {
                // std::cout << "current node keys: " << e.first << " of type " << type_map[e.second.second] << std::endl;
                std::cout << "[JSON_Node] current node keys: " << e.first << " of type " << type_map[e.second->type] << std::endl;
            }

            if (requested_entry_it != this->value_map.end()) {
                // if (requested_entry_it->second.second == DICT) {
                if (requested_entry_it->second->type == DICT_TYPE or requested_entry_it->second->type == ARR) {
                    // auto current_node = get<JSON_Node*>( requested_entry_it->second.first );
                    auto current_node = get<JSON_Object*>( requested_entry_it->second->value );
                    for (auto e :current_node->value_map) {
                        // std::cout << "found node keys: " << e.first << " of type " << type_map[e.second.second] << std::endl;
                        std::cout << "[JSON_Node] found node keys: " << e.first << " of type " << type_map[e.second->type] << std::endl;
                    }
                    std::cout << std::endl;
                    return *( get<JSON_Object*>( requested_entry_it->second->value ) );
                }
                throw std::runtime_error("not a dict or arr");
            }
            throw std::runtime_error("key not found");
        }

        template <typename T>
        T as(const std::string& key) {
            auto value_it = this->value_map.find(key);
            if ( value_it != this->value_map.end() ) {
                std::cout << "[JSON_Value] key " << value_it->first << std::endl;
                // std::cout << "found value of type " << type_map[value_it->second.second] << std::endl;
                std::cout << "[JSON_Value] value of type " << type_map[value_it->second->type] << std::endl;
                try {
                    // return get<T>(value_it->second.first);
                    return get<T>(value_it->second->value);
                } catch ( std::bad_variant_access& ) {
                    // throw std::runtime_error("requested value type for key \"" + value_it->first + "\" doesn't match the one being accessed: "+type_map[value_it->second.second]);
                    throw std::runtime_error("requested value type for key \"" + value_it->first + "\" doesn't match the one being accessed: "+type_map[value_it->second->type]);
                }
            }
            throw std::runtime_error("key not found");
    }

        std::string display(int depth = 0);
    };



    class JSON_Tree {
        std::string json_directory = "json";
        std::string single_line_json;
        std::vector<std::string> remainder_backtrack;
        JSON_Object* root{};

        bool read_only_;

    public:
        JSON_Tree( bool read_only ): read_only_(read_only) {}

        void setDirectory(const std::string& path);
        void importFile(const std::string& filename);
        void importString(const std::string& str);
        [[nodiscard]] std::string display() const;

        JSON_Object* parse(std::string str, int depth = 0);
        void parse();

        JSON_Object operator[]( const std::string& key ) {
            return root->operator[]("@root").operator[](key);
        }

    };

    size_t find_closing_symbol_distance(const std::string& str, size_t pos, char symbol);
}

#endif // JSON_PARSER_LIBRARY_H
