#include "../include/library.h"

#include <iostream>
#include <cassert>
#include <fstream>

// #define DEBUG_LOG(x)

#ifndef DEBUG
#define DEBUG_LOG(x) std::cout << x << std::endl;
#endif



// must do this until it finds a bracket that's not in the value section
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
    size_t find_closing_quote_distance(const std::string& str, const size_t pos) {

        int spotted_new_brackets = 0;
        size_t current_bracket_position = pos;
        bool quote_opened = false;

        while ( current_bracket_position < str.length() ) {
            // DEBUG_LOG(std::format("{} char at pos i={} is '{}'", spotted_new_brackets, current_bracket_position, str[current_bracket_position]))

            if ( str[current_bracket_position] == '\"' ) {
                if ( quote_opened ) {
                    --spotted_new_brackets;
                    if ( spotted_new_brackets == 0 ) return current_bracket_position;
                }
                else {
                    ++spotted_new_brackets;
                }
                quote_opened = !quote_opened;
            }
            ++current_bracket_position;
        }
        throw std::runtime_error(std::format("closing quote not found"));
    }

    size_t find_closing_symbol_distance(const std::string& str, const size_t pos, const char symbol) {
        char closing_symbol;
        if (symbol == '{') { closing_symbol = '}'; }
        else if (symbol == '[') { closing_symbol = ']'; }
        else throw std::runtime_error("invalid character");

        int spotted_new_brackets = 0;
        size_t current_bracket_position = pos;
        bool string_mode = false;

        while ( current_bracket_position < str.length() ) {
            // DEBUG_LOG(std::format("{} char at pos i={} is '{}'", spotted_new_brackets, current_bracket_position, str[current_bracket_position]))

            if ( str[current_bracket_position] == '\"' ) string_mode = !string_mode;
            else if ( str[current_bracket_position] == symbol and !string_mode ) {
                ++spotted_new_brackets;
            }
            else if ( str[current_bracket_position] == closing_symbol and !string_mode ) {
                --spotted_new_brackets;
                if ( spotted_new_brackets == 0 ) return current_bracket_position;
            }

            ++current_bracket_position;
        }
        throw std::runtime_error(std::format("closing symbol for {} not found", symbol));
    }

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

    JSON_Object* JSON_Tree::parse(std::string str, int depth)  {
        this->remainder_backtrack.clear();
            auto* node = new JSON_Object();

            std::cout << "@ JSON_Node at " << (void*)node << std::endl;
            std::cout << "@ Depth " << depth << ": " << str << std::endl;
        // TODO not all of those enum values are used, cleanup
            enum state {CURIOUS, END, DICT_OPENED, DICT_CLOSED, DICTIONARY, ARR_OPENED, ARR_CLOSED, ARRAY, KEY, VALUE}; //TODO move outside function
            state state = KEY;
            bool string_key_validation = true;
            std::string key, value;
            json_value_type type = INTEGER; // default should be int


            JSON_Object* child = nullptr;

            for ( int i = 0; i < str.length(); i++ ) {
                std::cout << std::format("> [DEPTH={}] Character at {}/{}: \'{}\' (state={}); (type={})", depth, i, str.length(), str[i], std::to_string(state), json_entry_typenames[type]) << std::endl;



                switch ( str[i] ) {
                    case '[':
                    case '{': {
                        if (str[i] == '{') state = DICT_OPENED;
                        else state = ARR_OPENED;

                        std::size_t end_bracket_pos;
                        if (str[i] == '{') end_bracket_pos = find_closing_symbol_distance(str, i, '{');
                        else end_bracket_pos = find_closing_symbol_distance(str, i, '[');

                        std::string sub_string = str.substr(i+1, end_bracket_pos - i - 1);
                        std::string remainder;

                        remainder = str.substr(0, i+1) + "@@@" + str.substr(end_bracket_pos, str.length()-1);

                        if (str[i] == '{') {DEBUG_LOG("Opening dictionary object at depth " << depth+1);}
                        else {DEBUG_LOG("Opening array object at depth " << depth+1);}

                        DEBUG_LOG("* Subsection: " << sub_string)
                        DEBUG_LOG("* Remainder: " << remainder)

                        child = parse(sub_string, depth+1);

                        remainder = str.substr(0, i+1) + this->remainder_backtrack.back() + str.substr(end_bracket_pos, str.length()-1);
                        this->remainder_backtrack.pop_back();
                        DEBUG_LOG("* New remainder: " << remainder)
                        str = remainder;

                        if (str[i] == '{') type = DICT_TYPE;
                        else type = ARR;

                        break;
                    }
                    case ']':
                    case '}': {
                        if (state == DICT_OPENED) {
                            state = DICT_CLOSED;
                        }
                        else if (state == ARR_OPENED) {
                            state = ARR_CLOSED;
                        }
                        else state = VALUE;
                        break;
                    }
                    case ':': {
                        std::cout << "Semicolon, changing to VALUE if KEY" << std::endl;
                        if (state == KEY) {
                            state = VALUE;
                        }
                        type = INTEGER;
                        break;
                    }
                    case ',': {
                        // After a VALUE for DICT
                        // FIXME currently can only add dicts and arrays as dict entry to previous object
                        if (state == VALUE or state == DICT_CLOSED or state == ARR_CLOSED) {
                            // if (value == "true" or value == "false") {type = BOOLEAN;} // FIXME what the hell
                            std::cout << "Comma spotted after value, adding entry to dict" << std::endl;
                            if (type!= STRING and (value == "true" or value == "false")) {type = BOOLEAN;}
                            if (type!= STRING and value == "null") {type = NULL_TYPE;}
                            std::cout << std::format("read key={} and value={} of type {} with child={}", key, value, type_map[type], static_cast<void *>(child)) << std::endl;
                            node->addDictEntry( key, value, type, child );

                            state = KEY;
                        // In arrays values are read to the key variable and during the KEY state, so it's a sign it should be added to array
                        } else if (state == KEY) {
                            std::cout << "Comma spotted after key, adding entry to array" << std::endl;
                            if (type!= STRING and (key == "true" or key == "false")) {type = BOOLEAN;}
                            if (type!= STRING and key == "null") {type = NULL_TYPE;}
                            std::cout << std::format("read key={} and value={} of type {} with child={}", std::to_string(node->array_iterator), key, type_map[type], static_cast<void *>(child)) << std::endl;
                            node->addArrEntry( key, type, child );

                        } else {
                            throw std::runtime_error("Unexpected parser state at ',' character");
                        }

                        break;
                    }
                    case '\"': {
                        std::size_t end_bracket_pos = find_closing_quote_distance(str, i);
                        std::string sub_string = str.substr(i+1, end_bracket_pos - i - 1);
                        DEBUG_LOG(sub_string);

                        if (state == KEY) {
                            std::cout << "Quotation mark spotted when reading key" << std::endl;
                            key = sub_string;
                            type = STRING;
                        }
                        if (state == VALUE) {
                            std::cout << "Quotation mark spotted when reading value" << std::endl;
                            value = sub_string;

                        }
                        type = STRING;

                        i = static_cast<int>(end_bracket_pos);
                        break;
                    }
                    case '.': {
                        if (state == VALUE and type != STRING) {
                            type = FLOAT;
                            value += str[i];
                        }
                        else throw std::runtime_error("Unexpected parser state at '.' character");
                        break;
                    }

                    default: {
                        switch (state) {
                            case KEY: {
                                key += str[i]; // rewrite as a different set of variables, maybe "key" and "word"
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

                if ( i == str.length()-1 ) {
                    std::cout << "SUBSECTION END at " << i << "/" << str.length()-1 << std::endl;

                    std::cout << "! Remainder: " << str.substr(i) << std::endl;

                    if ( i != str.length()-1 ) this->remainder_backtrack.push_back( str.substr(i) );
                    else this->remainder_backtrack.push_back("");
                    // this->remainder_backtrack.push_back( str.substr(i));

                    if ( depth == 0 ) {
                        key = "@root";
                        // type = ROOT;
                        type = DICT_TYPE;
                    }

                    if (type != NONE and i!= 0) { // TODO change this, none type shouldn't be possible
                        if (state == VALUE or state == DICT_CLOSED or state == ARR_CLOSED) {
                            std::cout << "Comma spotted after value, adding entry to dict" << std::endl;
                            if (type!= STRING and (value == "true" or value == "false")) {type = BOOLEAN;}
                            if (type!= STRING and value == "null") {type = NULL_TYPE;}
                            std::cout << std::format("read key={} and value={} of type {} with child={}", key, value, type_map[type], static_cast<void *>(child)) << std::endl;
                            node->addDictEntry( key, value, type, child );

                            state = KEY;
                        } else if (state == KEY) {
                            std::cout << "Comma spotted after key, adding entry to array" << std::endl;
                            if (type!= STRING and (key == "true" or key == "false")) {type = BOOLEAN;}
                            if (type!= STRING and key == "null") {type = NULL_TYPE;}
                            std::cout << std::format("read key={} and value={} of type {} with child={}", std::to_string(node->array_iterator), key, type_map[type], static_cast<void *>(child)) << std::endl;
                            node->addArrEntry( key, type, child );

                        } else {
                            throw std::runtime_error("Unexpected parser state at the end of a subsection");
                        }
                        // if (type!= STRING and (value == "true" or value == "false")) {type = BOOLEAN;}
                        // if (type!= STRING and value == "null") {type = NULL_TYPE;}
                        // std::cout << std::format("read key={} and value={} of type {} with child={}", key, value, type_map[type], static_cast<void *>(child)) << std::endl;
                        // node->addDictEntry( key, value, type, child );
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
                this->value = std::stof( str_value );
                break;
            case BOOLEAN:
                this->value = str_value == "true";
                break;
            case ARR:
                // throw std::runtime_error("Arrays are not implemented yet");
                this->value = node_pointer;
                break;
            case DICT_TYPE:
                this->value = node_pointer;
                break;
            case NULL_TYPE:
                this->value = new JSON_Null;
                break;
            default:
                throw std::runtime_error("Trying to create a JSON_Value struct with undefined data type.");
        }
    }

    JSON_Value::~JSON_Value() {
        if ( this->type == DICT_TYPE or this->type == ARR ) delete get<JSON_Object*>(this->value);
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
                    case DICT_TYPE:
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
