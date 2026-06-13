//
// Created by Mateusz M on 12/06/2026.
//

#include "../include/JsonParserLib/JSON_Node.hpp"

#include <iostream>

// #define DEBUG_DISPLAY

#ifdef DEBUG_DISPLAY
#define DEBUG_DISPLAY(...) std::print(std::cout, __VA_ARGS__)
#endif

#ifdef DEBUG_BRACKET
#define DEBUG_BRACKET(...) std::print(std::cout, __VA_ARGS__)
#endif


namespace json {

    std::string JSON_Node::display( const int depth ) {


            // DEBUG_DISPLAY("ENTERED NODE {}, type={} children size={}\n", static_cast<void *>(this),
            // type_map[this->node_value.type], this->children.size());

            std::string result;

            for ( auto [key, child] : this->children ) {
                // DEBUG_DISPLAY("* CHILD NODE {}, type={}\n", (void*)(child), type_map[child->node_value.type]);
            }

            result += std::format("{}(", type_map[this->node_value.type]);

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
                        break;
                    default:
                        throw std::runtime_error("Undefined node value type");
                }
            }
            else {
                if (!this->children.empty()) result += "\n";
                for ( auto [key, value] : this->children ) {
                    for (int tab_it = 0; tab_it<depth; ++tab_it) result+='\t';
                    if (is_dictionary_v) result += std::format("\tstr({}) : {}\n", get<std::string>(key), value->display(depth+1));
                    else result += std::format("\tint({}) : {}\n", get<int>(key), value->display(depth+1));
                }
            }

            if (!is_primitive_v) for (int tab_it = 0; tab_it<depth; ++tab_it) result+='\t';
            result += "),";

            return result;
        }

    //region ===== OPERATOR[] =====
    JSON_Node* JSON_Node::find_node( const std::variant<int, std::string> &key ) {
        if (const auto it = this->children.find( key ); it != this->children.end()) {
            return it->second;
        }
        return nullptr;
    }

    JSON_Node JSON_Node::operator[]( const int index ) {
        if ( this->children.empty() ) throw std::runtime_error("Can't index an empty JSON node");
        if ( is_primitive_v ) throw NotSubscribtable();
        if ( is_dictionary_v ) throw std::runtime_error("Can't index dictionary with an integer index: " + std::to_string(index));

        const std::variant<int, std::string> key_wrapper = index;

        if (auto result = find_node( key_wrapper ); result != nullptr) return *result;
        throw std::runtime_error("index not found");
    }

    JSON_Node JSON_Node::operator[](const char* key) {
        if ( this->children.empty() ) throw std::runtime_error("Can't index an empty JSON node");
        if ( is_primitive_v ) throw NotSubscribtable();
        if ( is_array_v ) throw std::runtime_error("Can't index array with a string key: " + std::string(key));

        const std::variant<int, std::string> key_wrapper = key;

        if (auto result = find_node( key_wrapper ); result != nullptr) return *result;
        throw std::runtime_error("index not found");
        //for debug
        // std::cout << "[JSON_Node] entered key: ";
        // if ( is_dictionary_v ) std::cout << get<std::string>(key) << std::endl;
        // if ( is_array_v ) std::cout << std::to_string(get<int>(key)) << std::endl;
        // for (const auto& [child_key, child_node] : this->children) {
        //     if ( is_array_v ) std::cout << "[JSON_Node] current node keys: " << std::to_string( get<int>(child_key) ) << " of type " << type_map[child_node->node_value.type] << std::endl;
        //     if ( is_dictionary_v ) std::cout << "[JSON_Node] current node keys: " << get<std::string>(child_key) << " of type " << type_map[child_node->node_value.type] << std::endl;
        // }
    }
    //endregion

    void JSON_Node::addEntry(const std::string &dict_key, JSON_Node *dict_val)  {
        if ( is_primitive_v ) throw std::runtime_error( "Node is a primitive" );
        if ( is_dictionary_v ) {
            children[dict_key] = dict_val;
        }
        if ( is_array_v ) {
            children[array_element_it++] = dict_val;
        }
    }

    bool JSON_Node::has( const char* key) {
        // if ( !this->is_dictionary_v ) throw std::runtime_error("Trying to find a key in a non dictionary object");

        // for ( auto [key, child] : this->children ) {
        //     std::cout << get<std::string>(key) << std::endl;
        // }

        const auto it = std::ranges::find_if(this->children, [key](const std::pair<json_key_datatype, JSON_Node*> &p) { return get<std::string>(p.first) == key; } );
        return it != this->children.end();
    }
} // json