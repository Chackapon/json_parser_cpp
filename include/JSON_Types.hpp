//
// Created by Mateusz M on 12/06/2026.
//

#ifndef JSON_PARSER_JSON_TYPES_HPP
#define JSON_PARSER_JSON_TYPES_HPP

// #define DEBUG_LOG(x)

#ifndef DEBUG
#define DEBUG_LOG(x) std::cout << x << std::endl;
#endif

#include <map>
#include <variant>

namespace json {

    template <typename T>
    concept json_key_type =
    std::is_same_v<T, std::string> ||
    std::is_same_v<T, int> ||
    std::is_same_v<T, std::monostate>;

    struct JSON_Value;



    template <json_key_type JSON_Key>
    struct JSON_Node;
    using JSON_Dict = JSON_Node<std::string>;
    using JSON_Prim = JSON_Node<std::monostate>;



    struct JSON_Null {};
    struct JSON_RootKey {};

    enum json_value_type {INTEGER, FLOAT, BOOLEAN, STRING, NULL_TYPE, DICT_TYPE, ARR_TYPE, NONE, UNDEFINED};
    inline std::map<json_value_type, std::string> type_map {
                { UNDEFINED, "undefined" },
                { STRING, "str" },
                { INTEGER, "int" },
                { FLOAT, "float" },
                { DICT_TYPE, "dict" },
                { ARR_TYPE, "arr" },
                { NONE, "none" },
                { BOOLEAN, "bool" },
                { NULL_TYPE, "null" }
    };


    using json_entry_datatype = std::variant< JSON_Null*, int, float, bool, std::string, JSON_Prim*, JSON_Dict* >;
    using json_key_datatype = std::variant< JSON_RootKey*, std::string, int >;

    inline std::map<int, std::string> json_entry_typenames {
            {NULL_TYPE, "null"},
            {INTEGER, "int"},
            {FLOAT, "float"},
            {BOOLEAN, "bool"},
            {STRING, "string"},
            {DICT_TYPE, "dict"},
            {ARR_TYPE, "arr"},
            {999, "prim"},
        };

    enum state {CURIOUS, END, DICT_OPENED, DICT_CLOSED, DICTIONARY, ARR_OPENED, ARR_CLOSED, ARRAY, KEY, VALUE};
    inline std::map<state, std::string> state_names {
            {VALUE, "value"},
            {KEY, "key"},
            {DICT_OPENED, "dict_opened"},
            {DICT_CLOSED, "dict_closed"},
            {ARR_OPENED, "arr_opened"},
            {ARR_CLOSED, "arr_closed"},
    };
}
#endif //JSON_PARSER_JSON_TYPES_HPP
