//
// Created by Mateusz M on 12/06/2026.
//

#ifndef JSON_PARSER_JSON_EXCEPTIONS_HPP
#define JSON_PARSER_JSON_EXCEPTIONS_HPP

#include <stdexcept>

struct NotSubscribtable : std::runtime_error {
    NotSubscribtable(): std::runtime_error("This node is not subscriptable") {};
};

struct BadArrayIndex : std::runtime_error {
    BadArrayIndex( const std::string& key ): std::runtime_error("Can't index array with a string key: " + key) {};
};

#endif //JSON_PARSER_JSON_EXCEPTIONS_HPP
