//
// Created by Mateusz M on 12/06/2026.
//

#ifndef JSON_PARSER_JSON_NODE_HPP
#define JSON_PARSER_JSON_NODE_HPP
#include <iostream>
#include <set>
#include <unordered_map>


#include "JSON_Exceptions.hpp"
#include "JSON_Types.hpp"
#include "JSON_Value.hpp"


namespace json {




    // template <json_key_type JSON_Key>
    struct JSON_Node {

        std::unordered_map<
            std::variant<int, std::string>,
            JSON_Node*
            > children;
        JSON_Value node_value;
        int array_element_it = 0;

        explicit JSON_Node( const JSON_Value& val ) : node_value(val) {}
        explicit JSON_Node(const json_value_type type ) : node_value( JSON_Value('@'+json_entry_typenames[type], type) ) {
        }

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

        void addEntry( const std::string& dict_key, JSON_Node* dict_val ) {
            if ( is_primitive_v ) throw std::runtime_error( "Node is a primitive" );
            if ( is_dictionary_v ) {
                children[dict_key] = dict_val;
            }
            if ( is_array_v ) {
                children[array_element_it++] = dict_val;
            }
        }

        JSON_Node operator[]( const std::variant<int, std::string> &key ) {
            if ( is_primitive_v ) throw NotSubscribtable();

            const auto requested_entry_it = this->children.find( key );

            //for debug
            std::cout << "[JSON_Node] entered key: ";
            if ( is_dictionary_v ) std::cout << get<std::string>(key) << std::endl;
            if ( is_array_v ) std::cout << std::to_string(get<int>(key)) << std::endl;
            for (const auto& [child_key, child_node] : this->children) {
                if ( is_array_v ) std::cout << "[JSON_Node] current node keys: " << std::to_string( get<int>(child_key) ) << " of type " << type_map[child_node->node_value.type] << std::endl;
                if ( is_dictionary_v ) std::cout << "[JSON_Node] current node keys: " << get<std::string>(child_key) << " of type " << type_map[child_node->node_value.type] << std::endl;
            }

            if (requested_entry_it != this->children.end()) {
                return *requested_entry_it->second;
            }

            if ( is_array_v ) throw std::runtime_error("index not found"); // IDEA replace with the out of bounds + add support for negative indexes
            throw std::runtime_error("key not found");
        }

        template <typename T>
        T as() {
            try {
                return get<T>( this->node_value.value );
            } catch ( std::bad_variant_access& ) {
                throw std::runtime_error("Value being accessed is of type " + type_map[this->node_value.type]);
            }
        }

        std::string display( const int depth ) {
            // std::print(std::cout, "ENTERED NODE {}, type={} children size={}\n", (void*)this, json_entry_typenames[this->node_value.type], this->children.size());
            std::string result;

            // for ( auto e : this->children ) {
            //     if (std::holds_alternative<JSON_Dict*>(e.second)) {
            //         std::print(std::cout, "* CHILD NODE {}, type={}\n", (void*)get<JSON_Dict*>(e.second), json_entry_typenames[get<JSON_Dict*>(e.second)->node_value.type]);
            //     }
            //     else if (std::holds_alternative<JSON_Prim*>(e.second)) {
            //         std::print(std::cout, "* CHILD NODE {}, type={}\n", (void*)get<JSON_Prim*>(e.second), json_entry_typenames[get<JSON_Prim*>(e.second)->node_value.type]);
            //     }
            // }

            result += std::format("{}(", json_entry_typenames[this->node_value.type]);

            if ( is_primitive_v ) {

                switch ( this->node_value.type ) {
                    case STRING:
                        result += get<std::string>(this->node_value.value);
                        break;
                    case INTEGER:
                        result += std::to_string( get<int>(this->node_value.value) );
                        break;
                    case FLOAT:
                        result += std::to_string( get<float>(this->node_value.value) );
                        break;
                    case BOOLEAN:
                        if ( get<bool>(this->node_value.value) ) result += "true";
                        else result += "false";
                        break;
                    case NULL_TYPE:
                        result += "null";
                    case NONE:
                        break;
                    default:
                        throw std::runtime_error("Undefined node value type");
                }
            }
            else {
                result += "\n";
                for ( auto [key, value] : this->children ) {
                    for (int tab_it = 0; tab_it<depth; ++tab_it) result+='\t';
                    result += std::format("\tstr({}) : {}\n", get<std::string>(key), value->display(depth+1));
                }
            }

            if (!is_primitive_v) for (int tab_it = 0; tab_it<depth; ++tab_it) result+='\t';
            result += "),";
            return result;
        }

    };
} // json

#endif //JSON_PARSER_JSON_NODE_HPP
