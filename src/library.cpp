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


    JSON_Node *JSON_Tree::parse(std::string str, int depth)  {

            this->remainder_backtrack.clear();
            auto* node = new JSON_Node();

            std::cout << "@ JSON_Node at " << (void*)node << std::endl;
            std::cout << "@ Depth " << depth << ": " << str << std::endl;
        // TODO not all of those enum values are used, cleanup
            enum state {CURIOUS, END, DICT_BEGIN, DICT_END, DICTIONARY, ARR_BEGIN, ARR_END, ARRAY, KEY, VALUE}; //TODO move outside function
            state state = KEY;
            std::string key, value;
            json_value_type type = INTEGER; // default should be int


            JSON_Node* child = nullptr;

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


    json_entry_datatype convert(std::string value, const json_value_type to_type, JSON_Node* node_pointer) {
        switch (to_type) {
            case STRING:
                return value;
            case INTEGER:
                try {
                    return std::stoi(value);
                } catch ( std::invalid_argument& e ) {
                    std::cout << "Probably empty record: " << e.what();
                    exit(-1);
                }
            case FLOAT:
                return std::stof(value);
            case CHAR:
                return static_cast<char>(value[0]); // NOTE is it safe to do it like this?
            case BOOLEAN:
                return static_cast<bool>(value == "true");
            case ARR:
                // case ROOT:
            case DICT:
                return node_pointer;
            case NULL_TYPE:
                return new EmptyValue;
            default:
                throw std::runtime_error("Undefined node type");
        }
    }


}
