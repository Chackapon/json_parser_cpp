// ReSharper disable CppUnnecessaryWhitespace
#ifndef JSON_PARSER_LIBRARY_H
#define JSON_PARSER_LIBRARY_H




#include <string>
#include <filesystem>

#include "JSON_Types.hpp"
#include "JSON_Value.hpp"
#include "JSON_Node.hpp"
#include "JSON_Parser.hpp"

// FIXME graph json exporters use capital first letter string versions of true and false, to be fixed

namespace json {


    // JSON_Value generate_json_value(state state, std::string value, json_value_type type, JSON_Node *temp_node_holder);


    /*
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
                if (requested_entry_it->second->type == DICT_TYPE or requested_entry_it->second->type == ARR_TYPE) {
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
    */

    size_t find_closing_symbol_distance(const std::string& str, size_t pos, char symbol);
}

#endif // JSON_PARSER_LIBRARY_H
