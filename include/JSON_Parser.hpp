//
// Created by Mateusz M on 12/06/2026.
//

#ifndef JSON_PARSER_JSON_PARSER_HPP
#define JSON_PARSER_JSON_PARSER_HPP
#include <string>
#include <vector>

#include "JSON_Node.hpp"
#include "JSON_Value.hpp"

namespace json {
    class JSON_Parser {
        std::string json_directory = "json";
        std::string single_line_json;
        std::vector<std::string> remainder_backtrack;
        JSON_Dict* root{};
        JSON_RootKey* root_key{};

        bool read_only_;

    public:
        explicit JSON_Parser( const bool read_only ): read_only_(read_only) {
            // const auto root_value = JSON_Value("@root", DICT_TYPE);
            root = new JSON_Dict( DICT_TYPE );
            root->node_value.value = "@root";
        }

        void setDirectory(const std::string& path);
        void importFile(const std::string& filename);
        void importString(const std::string& str);
        [[nodiscard]] std::string display() const;


        template <json_key_type JSON_Key>
        JSON_Node<JSON_Key>* parse(std::string str, int depth, json_value_type to_type);
        void parse();

        template <json_key_type JSON_Key>
        JSON_Node<JSON_Key> operator[]( const std::string& key ) const {
            return root->operator[](key);
        }

    };
}

#endif //JSON_PARSER_JSON_PARSER_HPP
