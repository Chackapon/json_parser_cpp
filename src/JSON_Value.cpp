//
// Created by Mateusz M on 12/06/2026.
//

#include "../include/JSON_Value.hpp"

#include <iostream>

namespace json {
    JSON_Value::JSON_Value( const std::string& str_val, const json_value_type to_type ) {
        this->type = to_type;
        DEBUG_LOG('>'+str_val)
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
            case NONE:
            case DICT_TYPE:
            case ARR_TYPE:
                break;
            default:
                throw std::runtime_error("Undefined JSON_Value value type");
        }
    }
    /*
    JSON_Value *JSON_Value::setValue(const std::string &str_value, const json_value_type to_type)  {
        this->type = to_type;

        switch (to_type) {
            case STRING:
                this->value = str_value;
                break;
            case INTEGER:
                try {
                    this->value = std::stoi( str_value );
                } catch ( std::invalid_argument& e ) {
                    std::cout << "Probably empty record: " << e.what();
                    exit(-1);
                }
                break;
            case FLOAT:
                try {
                    this->value = std::stof( str_value );
                } catch ( std::invalid_argument& ) {
                    throw std::runtime_error("Value \""+str_value+"\" is not a valid float");
                }
                break;
            case BOOLEAN:
                this->value = str_value == "true";
                break;
            case NULL_TYPE:
                this->value = new JSON_Null;
                break;
            default:
                throw std::runtime_error("Trying to create a JSON_Value struct with a primitive of undefined data type.");
        }
        return this;
    }

    JSON_Value *JSON_Value::setValue(JSON_Node *dict_node, const json_value_type to_type) {
        this->type = to_type;
        this->value = dict_node;
        return this;
    }
    */
} // json