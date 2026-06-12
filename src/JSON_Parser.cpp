//
// Created by Mateusz Miliutin on 12/06/2026.
//

#include "../include/JSON_Parser.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <format>

std::string stripWhitespace(std::string str) {
    (void)str.erase(std::ranges::remove_if(str, ::isspace).begin(), str.end());
    return str;
}

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

namespace json {
    //region === JSON_Parser ===
    void JSON_Parser::setDirectory(const std::string &path) {
        this->json_directory = path;
    }

    void JSON_Parser::importFile(const std::string &filename)  {
        std::ifstream input_file(json_directory+"/"+filename);
        std::string line;

        while ( getline(input_file, line) ) {
            single_line_json += stripWhitespace(line); // FIXME this function probably strips spaces in strings
        }
    }

    void JSON_Parser::importString(const std::string &str) {
        single_line_json += stripWhitespace(str);
    }

    std::string JSON_Parser::display() const { return root->display(0); }

    JSON_Node* JSON_Parser::parse(std::string str, int depth, const json_value_type to_type)  {
        JSON_Node* node;
        JSON_Node* temp_node;

        if ( to_type == DICT_TYPE ) node = new JSON_Node( DICT_TYPE );
        else if ( to_type == ARR_TYPE ) node = new JSON_Node( ARR_TYPE );
        else throw std::runtime_error("illegal node type parsing data");


        std::cout << "@ Depth " << depth << ": " << str << std::endl;
        // TODO not all of those enum values are used, cleanup

        state state = KEY;
        std::string key, value;
        json_value_type type = INTEGER; // default should be int

        for ( int i = 0; i < str.length(); i++ ) {
                std::cout << std::format("> [DEPTH={}] Character at {}/{}: \'{}\' (state={}); (type={})", depth, i, str.length(), str[i], state_names[state], json_entry_typenames[type]) << std::endl;



                switch ( str[i] ) {
                    case '[':
                    case '{': {
                        if (str[i] == '{') state = DICT_OPENED;
                        else state = ARR_OPENED;

                        std::size_t end_bracket_pos;
                        if (str[i] == '{') end_bracket_pos = find_closing_symbol_distance(str, i, '{');
                        else end_bracket_pos = find_closing_symbol_distance(str, i, '[');

                        std::string sub_string = str.substr(i+1, end_bracket_pos - i - 1) + ',';
                        std::string remainder;

                        remainder = str.substr(0, i+1) + "@@@" + str.substr(end_bracket_pos, str.length()-1);

                        if (str[i] == '{') {DEBUG_LOG("Opening dictionary object at depth " << depth+1);}
                        else {DEBUG_LOG("Opening array object at depth " << depth+1);}

                        DEBUG_LOG("* Subsection: " << sub_string)
                        DEBUG_LOG("* Remainder: " << remainder)

                        // temp_node_holder =
                        // DUBUG_LOG(state_names[state]);
                        DEBUG_LOG((void*)node);

                        if ( state == DICT_OPENED ) temp_node = parse(sub_string, depth+1, DICT_TYPE);

                        remainder = str.substr(0, i+1) + this->remainder_backtrack.back() + str.substr(end_bracket_pos, str.length()-1);
                        this->remainder_backtrack.pop_back();
                        DEBUG_LOG("* New remainder: " << remainder)
                        str = remainder;

                        if (str[i] == '{') type = DICT_TYPE;
                        else type = ARR_TYPE;

                        break;
                    }
                    case ']':
                    case '}': {
                        if (state == DICT_OPENED) {
                            node->addEntry( key, temp_node );
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
                         // parsed_value = ;

                        // if ( node->node_value.type == DICT_TYPE ) node->addEntry( key, parsed_value );
                        // if ( node->node_value.type == ARR_TYPE ) node->addEntry( parsed_value );
                        if ( node->is_dictionary_v ) {
                            if ( state == VALUE ) {
                                node->addEntry( key, new JSON_Node( JSON_Value(value, type) ) );
                            }
                            value = "";
                            state = KEY;
                        }
                        else throw std::runtime_error("invalid node type at ',' symbol");

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

                    // if ( depth == 0 ) {
                    //     key = "@root";
                    //     // type = ROOT;
                    //     type = DICT_TYPE;
                    // }

                    // parsed_value = generate_json_value(state, value, type, temp_node_holder);
                    // if ( node->node_value.type == DICT_TYPE ) node->addEntry( key, parsed_value );
                    // if ( node->node_value.type == ARR_TYPE ) node->addEntry( parsed_value );
                    /*
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
                    */
                    break;
                }
            }

        DEBUG_LOG(std::format("Current node children size: {}", node->children.size()));
        std::cout <<"RETURNING NODE " << node << std::endl;
        return node;
    }

    void JSON_Parser::parse() {
        auto to_parse = single_line_json.substr(1, find_closing_symbol_distance(single_line_json,0,'{')-1);
        root = parse( to_parse+',', 0, DICT_TYPE); // TODO bruh what is this
        std::cout << "PARSING COMPLETED" << std::endl;
    }
    //endregion
}
