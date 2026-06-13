//
// Created by Mateusz M on 12/06/2026.
//

#ifndef JSON_PARSER_JSON_NODE_HPP
#define JSON_PARSER_JSON_NODE_HPP

#include <list>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <vector>


#include "JSON_Exceptions.hpp"
#include "JSON_Types.hpp"
#include "JSON_Value.hpp"


namespace json {




    // template <json_key_type JSON_Key>
    class JSON_Node {

    public:
        std::unordered_map< json_key_datatype, JSON_Node* > children;
        JSON_Value node_value;
        int array_element_it = 0;

        JSON_Node *find_node(const std::variant<int, std::string> &key);


        explicit JSON_Node( const JSON_Value& val ) : node_value(val) {}
        explicit JSON_Node(const json_value_type type ) : node_value( JSON_Value('@'+type_map[type], type) ) {}

        bool is_dictionary_v = node_value.type == DICT_TYPE;
        bool is_array_v = node_value.type == ARR_TYPE;
        bool is_primitive_v = ( !is_dictionary_v && !is_array_v );

        // TODO turn into a proper iterator class
        std::set<json_key_datatype> keys() {
            auto result = std::set<json_key_datatype>();
            if ( is_dictionary_v )
                for ( const auto& [key, value] : this->children ) {
                    result.insert( get<std::string>(key) );
                }
            else if ( is_array_v )
                for ( const auto& [key, value] : this->children ) {
                    result.insert( get<int>(key) );
                }
            else throw NotSubscribtable();
            return result;
        }

        void addEntry( const std::string& dict_key, JSON_Node* dict_val );

        JSON_Node operator[](int index);
        JSON_Node operator[](const char* key);

        template <typename T>
        T as() {
            if ( is_primitive_v) {
                try {
                    return get<T>( this->node_value.value );
                } catch ( std::bad_variant_access& ) {
                    throw std::runtime_error("Value being accessed is of type " + type_map[this->node_value.type]);
                }
            }
            throw std::runtime_error("Not a primitive value");
        }

        template <typename T>
        std::vector<T> as_vector() {
            auto result = std::vector<T>();
            if (is_array_v) {
                for ( int i = 0; i < this->children.size(); i++ ) {
                    auto child_node = this->children[i];
                    if ( std::holds_alternative<T>( child_node->node_value.value ) and child_node->is_primitive_v ) {
                        result.push_back( child_node->as<T>() );
                    }
                    else throw std::runtime_error("Cannot create a vector of different or non primitive types");
                }
                return result;
            }
            throw std::runtime_error("Not an array");
        }

        template <typename T>
        std::list<T> as_list() {
            auto result = std::list<T>();
            if (is_array_v) {
                for ( int i = 0; i < this->children.size(); i++ ) {
                    auto child_node = this->children[i];
                    if ( std::holds_alternative<T>( child_node->node_value.value ) and child_node->is_primitive_v ) {
                        result.push_back( child_node->as<T>() );
                    }
                    else throw std::runtime_error("Cannot create a vector of different or non primitive types");
                }
            }
            else throw std::runtime_error("Not an array");

            return result;
        }

        std::string display( int depth );

        bool has( const char* key );
    };
} // json

#endif //JSON_PARSER_JSON_NODE_HPP
