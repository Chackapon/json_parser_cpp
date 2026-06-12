//
// Created by Mateusz M on 12/06/2026.
//

#ifndef JSON_PARSER_JSON_VALUE_HPP
#define JSON_PARSER_JSON_VALUE_HPP

#include <string>

#include "JSON_Types.hpp"

namespace json {
    struct JSON_Value {
        json_entry_datatype value;
        json_value_type type{};

        JSON_Value( const std::string& str_val, json_value_type to_type );
        ~JSON_Value() = default;

        // JSON_Value* setValue( const std::string& str_value, json_value_type to_type );
        // JSON_Value* setValue( JSON_Node* dict_node, json_value_type to_type );
    };
} // json

#endif //JSON_PARSER_JSON_VALUE_HPP
