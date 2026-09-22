//
// Created by Mateusz M on 12/06/2026.
//

#include "../include/JsonParserLib/JSON_Value.hpp"

#include <iostream>

namespace json {
    JSON_Value::JSON_Value( const std::string& str_val, const json_value_type to_type ) {
        this->as_str = str_val;
        this->type = to_type;
        switch ( to_type ) {
            case STRING:
                this->value =  str_val;
                break;
            case INTEGER:
                try {
                    this->value = std::stoi(str_val);
                    break;
                } catch ( std::exception& e ) {
                    throw std::runtime_error( e.what() );
                }
            case FLOAT:
                try {
                    this->value = std::stof(str_val);
                    break;
                } catch ( std::exception& e ) {
                    throw std::runtime_error( e.what() );
                }
            case BOOLEAN:
                this->value = str_val == "true";
                break;
            case NULL_TYPE:
                this->value = new JSON_Null;
            case DICT_TYPE:
            case ARR_TYPE:
                break;
            default:
                throw std::runtime_error("Undefined JSON_Value value type");
        }
    }
} // json