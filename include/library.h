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



    struct JSON_Null {};
    using json_entry_datatype = std::variant<int, float, bool, char, std::string, JSON_Null*, JSON_Object*>;
    inline std::map<int, std::string> json_entry_typenames {
        {0, "int"},
        {1, "float"},
        {2, "bool"},
        {3, "char"},
        {4, "string"},
        {5, "dict"},
        {6, "emptyval"}
    };

    json_entry_datatype convert(std::string value, json_value_type to_type, JSON_Object* node_pointer = nullptr);

    struct JSON_Node {
        JSON_Node operator[]( const std::string& key );
    };

    struct JSON_Value {
        json_entry_datatype value;
        json_value_type type;

        JSON_Value( std::string& str_value, json_value_type to_type, JSON_Object* node_pointer = nullptr ) {
            this->type = to_type;
            this->value = convert( str_value, to_type, node_pointer );
        }

        JSON_Node operator[]( const std::string& key ) {
            std::cout << "[JSON_Value] entered key: "+key << std::endl;
            return JSON_Node();
        }
    };

    struct JSON_Object {
    // std::unordered_map<std::string, std::pair<json_entry_datatype, json_value_type>> value_map;
    std::unordered_map<std::string, JSON_Value*> value_map;
    int array_iterator = 0;

    void addDictEntry(std::string &key, std::string &val, json_value_type &to_type, JSON_Object *node_pointer) {
        // this->value_map[key] = std::make_pair(convert(val, to_type, node_pointer), to_type);
        this->value_map[key] = new JSON_Value( val, to_type, node_pointer );
        key = "";
        val = "";
        node_pointer = nullptr;
        to_type = INTEGER;
    }

    void addArrEntry( std::string &val, json_value_type &to_type, JSON_Object *node_pointer) {
        // this->value_map[std::to_string( array_iterator++ )] = std::make_pair(convert(val, to_type, node_pointer), to_type);
        this->value_map[std::to_string( array_iterator++ )] = new JSON_Value( val, to_type, node_pointer );
        val = "";
        node_pointer = nullptr;
        to_type = INTEGER;
    }

    JSON_Object operator[]( const std::string& key ) {
        std::cout << "[JSON_Node] entered key: "+key << std::endl;
        auto requested_entry_it = this->value_map.find(key);

        for (auto e : this->value_map) {
            // std::cout << "current node keys: " << e.first << " of type " << type_map[e.second.second] << std::endl;
            std::cout << "[JSON_Node] current node keys: " << e.first << " of type " << type_map[e.second->type] << std::endl;
        }

        if (requested_entry_it != this->value_map.end()) {
            // if (requested_entry_it->second.second == DICT) {
            if (requested_entry_it->second->type == DICT) {
                // auto current_node = get<JSON_Node*>( requested_entry_it->second.first );
                auto current_node = get<JSON_Object*>( requested_entry_it->second->value );
                for (auto e :current_node->value_map) {
                    // std::cout << "found node keys: " << e.first << " of type " << type_map[e.second.second] << std::endl;
                    std::cout << "[JSON_Node] found node keys: " << e.first << " of type " << type_map[e.second->type] << std::endl;
                }
                std::cout << std::endl;
                return *( get<JSON_Object*>( requested_entry_it->second->value ) );
            }
            throw std::runtime_error("not a dict");
        }
        throw std::runtime_error("key not found");
    }

    template <typename T>
    T value(const std::string& key) {
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




    std::string display(int depth = 0) {
        // std::cout << "<At node " << this << ">" << std::endl;
        // std::cout << this->value_map.size() << std::endl;

        std::string result;

        if (!this->value_map.empty()) {
            if (depth!=-1) result += '\n';

            for ( auto [k, val] : value_map ) {
                for (int tab_it = 0; tab_it<depth; ++tab_it) result+='\t';
                if (depth!=-1) {
                    // if (val.second != NONE) result += "str(" + k + ") : ";
                    if (val->type != NONE) result += "str(" + k + ") : ";
                    else result += "none(";
                }
                // result += type_map[val.second] + "(";
                result += type_map[val->type] + "(";

                // std::cout << "first index: " << json_entry_typenames[val.first.index()] << std::endl;
                switch (val->type) {
                // switch (val.second) {
                    case STRING:
                        // std::cout << "first value(str): " << get<std::string>(val.first) << std::endl;
                        result += get<std::string>(val->value);
                        break;
                    case INTEGER:
                        // std::cout << "first value(int): " << get<int>(val.first) << std::endl;
                        result += std::to_string( get<int>(val->value) );
                        break;
                    case FLOAT:
                        // std::cout << "first value(float): " << get<float>(val.first) << std::endl;
                        result += std::to_string( get<float>(val->value) );
                        break;
                    case BOOLEAN:
                        // std::cout << "first value(bool): " << get<bool>(val.first) << std::endl;
                        if ( get<bool>(val->value) ) result += "true";
                        else result += "false";
                        break;
                        // result += std::to_string( get<bool>(val.first) );
                    case NULL_TYPE:
                        break;
                    case ARR:
                    // case ROOT:
                    case DICT:
                        // std::cout << "first value(dict): " << get<::JSON_Node*>(val.first) << std::endl;
                        result += get<JSON_Object*>(val->value)->display(depth+1);
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
}

#endif // JSON_PARSER_LIBRARY_H
