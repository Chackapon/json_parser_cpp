#include "../include/library.h"

#include <iostream>
#include <cassert>
#include <fstream>

size_t rfind_character(const std::string& str, const char symbol, const int n) {
    assert(n > 0);
    size_t current_bracket_position = 0;

    for ( int i = 1; i <= n; i++ ) {
        if (i <= 1) current_bracket_position = str.rfind(symbol);
        else current_bracket_position = str.rfind(symbol, current_bracket_position - 1);

        if (current_bracket_position == std::string::npos) {
            throw std::runtime_error("Bracket not found");
        }
    }
    return current_bracket_position;
}


namespace json {

    //region === JSON_Tree ===
    void JSON_Tree::setDirectory(const std::string &path) {
        this->json_directory = path;
    }

    void JSON_Tree::importFile(const std::string &filename)  {
        std::ifstream input_file(json_directory+"/"+filename);
        std::string line;

        while ( getline(input_file, line) ) {
            single_line_json += stripWhitespace(line); // FIXME this function probably strips spaces in strings
        }
    }

    void JSON_Tree::importString(const std::string &str) {
        single_line_json += stripWhitespace(str);
    }

    std::string JSON_Tree::display() const { return root->display(); }

    JSON_Object *JSON_Tree::parse(std::string str, int depth)  {

            this->remainder_backtrack.clear();
            auto* node = new JSON_Object();

            std::cout << "@ JSON_Node at " << (void*)node << std::endl;
            std::cout << "@ Depth " << depth << ": " << str << std::endl;
        // TODO not all of those enum values are used, cleanup
            enum state {CURIOUS, END, DICT_BEGIN, DICT_END, DICTIONARY, ARR_BEGIN, ARR_END, ARRAY, KEY, VALUE}; //TODO move outside function
            state state = KEY;
            std::string key, value;
            json_value_type type = INTEGER; // default should be int


            JSON_Object* child = nullptr;

            for ( int i = 0; i < str.length(); i++ ) {
                std::cout << std::format("> [DEPTH={}] Character at {}/{}: \'{}\' (state={})", depth, i, str.length(), str[i], std::to_string(state)) << std::endl;



                switch ( str[i] ) {
                    case '{': {
                        state = DICT_BEGIN;
                        std::cout << "Opening dictionary object at depth " << depth+1 << std::endl;

                        std::size_t end_bracket_pos = rfind_character(str, '}', 1);
                        std::string sub_string = str.substr(i+1, end_bracket_pos - i - 1);
                        std::cout << "* Subsection: " << sub_string << std::endl;

                        std::string remainder = str.substr(0, i +1) + "@@@" + str.substr(end_bracket_pos+1 -1, str.length()-1);
                        std::cout << "* Remainder: " << remainder << std::endl;

                        child = parse(sub_string, depth+1);

                        remainder = str.substr(0, i +1) + this->remainder_backtrack.back() + str.substr(end_bracket_pos+1 -1, str.length()-1);
                        this->remainder_backtrack.pop_back();
                        std::cout << "* New Remainder: " << remainder << std::endl;
                        str = remainder;
                        type = DICT;

                        break;
                    }

                    case '}': {
                        if (state != DICT_BEGIN) {
                            state = DICT_END;
                        }
                        else state = VALUE;

                        // type = INTEGER;

                        break;
                    }
                    case ':': {
                        std::cout << "Semicolon, changing to VALUE if KEY" << std::endl;
                        if (state == KEY) {
                            state = VALUE;
                        }
                        break;
                    }
                    case ',': {
                        if (state == VALUE) {
                            // if (value == "true" or value == "false") {type = BOOLEAN;} // FIXME what the hell
                            std::cout << "Comma spotted after value, adding entry to node" << std::endl;
                            if (type!= STRING and (value == "true" or value == "false")) {type = BOOLEAN;}
                            if (type!= STRING and value == "null") {type = NULL_TYPE;}
                            std::cout << std::format("read key={} and value={} of type {} with child={}", key, value, type_map[type], static_cast<void *>(child)) << std::endl;
                            node->addDictEntry( key, value, type, child );

                            state = KEY;
                        }
                        break;
                    }
                    case '\"': {
                        if (state == VALUE) {
                            // if (!key_validated) throw std::runtime_error("Key error");
                            type = STRING;
                        }
                        break;
                    }
                    case '.': {
                        if (state == KEY) { key+=str[i]; }
                        if (state == VALUE and type != STRING) {
                            type = FLOAT;
                            value += str[i];
                        }
                        break;
                    }

                    default: {
                        switch (state) {
                            case KEY: {
                                key += str[i];
                                break;
                            }
                            case VALUE: {
                                value += str[i];
                                break;
                            }
                            default: {}
                        }
                    }

                }

                if ( state == DICT_END || i == str.length()-1 ) {
                    std::cout << "DICTIONARY END at " << i << "/" << str.length()-1 << std::endl;

                    std::cout << "! Remainder: " << str.substr(i) << std::endl;

                    if ( i != str.length()-1 ) this->remainder_backtrack.push_back( str.substr(i) );
                    else this->remainder_backtrack.push_back("");
                    // this->remainder_backtrack.push_back( str.substr(i));

                    if ( depth == 0 ) {
                        key = "@root";
                        // type = ROOT;
                        type = DICT;
                    }

                    if (type != NONE and i!= 0) { // TODO change this, none type shouldn't be possible
                        if (type!= STRING and (value == "true" or value == "false")) {type = BOOLEAN;}
                        if (type!= STRING and value == "null") {type = NULL_TYPE;}
                        std::cout << std::format("read key={} and value={} of type {} with child={}", key, value, type_map[type], static_cast<void *>(child)) << std::endl;
                        node->addDictEntry( key, value, type, child );
                    }
                    break;
                }
            }


            std::cout <<"RETURNING NODE " << node << std::endl;
            return node;
        }
    void JSON_Tree::parse() {
        root = parse(single_line_json );
        std::cout << "PARSING COMPLETED" << std::endl;
    }
    //endregion


    //region === JSON_Value ===
    JSON_Value::JSON_Value(std::string &str_value, json_value_type to_type, JSON_Object *node_pointer) {
        this->type = to_type;

        switch (to_type) {
            case STRING:
                this->value = str_value;
            case INTEGER:
                try {
                    this->value = std::stoi( str_value );
                } catch ( std::invalid_argument& e ) {
                    std::cout << "Probably empty record: " << e.what();
                    exit(-1);
                }
            case FLOAT:
                this->value = std::stof( str_value );
            case BOOLEAN:
                this->value = str_value == "true";
            case ARR:
                throw std::runtime_error("Arrays are not implemented yet");
            case DICT:
                this->value = node_pointer;
            case NULL_TYPE:
                this->value = new JSON_Null;
            default:
                throw std::runtime_error("Trying to create a JSON_Value struct with undefined data type.");
        }
    }

    JSON_Value::~JSON_Value() {
        if ( this->type == DICT or this->type == ARR ) delete get<JSON_Object*>(this->value);
        if ( this->type == NULL_TYPE ) delete get<JSON_Null*>(this->value);
    }

    //endregion

    //region === JSON_Object ===
    void JSON_Object::addDictEntry(std::string &key, std::string &val, json_value_type &to_type, JSON_Object *node_pointer) {
        this->value_map[key] = new JSON_Value( val, to_type, node_pointer );
        key = "";
        val = "";
        node_pointer = nullptr;
        to_type = INTEGER;
    }

    void JSON_Object::addArrEntry(std::string &val, json_value_type &to_type, JSON_Object *node_pointer) {
        this->value_map[std::to_string( this->array_iterator++ )] = new JSON_Value( val, to_type, node_pointer );
        val = "";
        node_pointer = nullptr;
        to_type = INTEGER;
    }

    std::string JSON_Object::display(int depth)  {
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

    //endregion



}
