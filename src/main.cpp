//
// Created by Mateusz M on 08/06/2026.
//

#include "../include/JsonParserLib/JSON_Parser.hpp"
#include <iostream>
#include <list>
#include <variant>

using namespace json;

using json_primitive_type = std::variant<int, float, bool, std::string, json::JSON_Null*>;
using instruction_type = std::variant<json_entry_datatype>;

template <typename T>
void do_work(json::JSON_Node current_node, std::list<json_entry_datatype> instructions) {

        while ( !instructions.empty() ) {

            DEBUG_LOG(std::format("@CURRENT NODE val={} type={}", current_node.node_value.as_str, json::type_map[current_node.node_value.type]))
            DEBUG_LOG(std::format("@INSTRUCTIONS LEFT: {}", instructions.size()))


            if ( std::holds_alternative<std::string>( instructions.front() ) ) {
                auto instruction = std::get<std::string>( instructions.front() );
                DEBUG_LOG(std::format("* instruction {}", instruction))

                if ( instruction[0] == '@') {
                    DEBUG_LOG(std::format(" > function {}:", instruction))
                    if ( instruction == "@forelements" ) {
                        for ( auto [key, child] : current_node.children ) {

                            // if ( current_node.is_dictionary_v ) DEBUG_LOG(std::format(" > key={}, val={}, type={}", get<std::string>(key), child->node_value.as_str, json::type_map[child->node_value.type]))
                            // if ( current_node.is_array_v ) DEBUG_LOG(std::format(" > key={}, val={}, type={}", get<int>(key), child->node_value.as_str, json::type_map[child->node_value.type]))

                            std::list<json_entry_datatype> new_instructions;

                            new_instructions = {child};

                            for ( auto it = ++instructions.begin(); it != instructions.end(); ++it ) {
                                new_instructions.push_back(*it);
                            }
                            // DEBUG_LOG(new_instructions.size());
                            do_work<T>(current_node, new_instructions);
                        }
                        return;
                    }
                    else if ( instruction == "@keys" ) {
                        for ( auto [key, child] : current_node.children ) {
                            DEBUG_LOG(std::format("# RETURNING KEY {}", get<std::string>(key)));
                        }
                    }
                    else if ( instruction == "@value" ) {
                        DEBUG_LOG(std::format("# RETURNING VALUE {}", get<T>(current_node.node_value.value)));
                        // DEBUG_LOG(instructions.size());
                    }
                    else if ( instruction == "@key" ) {
                        if ( current_node.is_dictionary_v ) {
                            DEBUG_LOG(std::format("# RETURNING KEY {}", get<std::string>(current_node.node_value.value)));
                        }
                        else throw std::runtime_error("Not a dictionary theres no key");
                    }
                }
                else {
                    DEBUG_LOG(std::format(" >  string entry"))
                    current_node = current_node[get<std::string>(instructions.front()).c_str()];
                }
            }
            else if ( std::holds_alternative<json::JSON_Node*>( instructions.front() ) ) {
                DEBUG_LOG("* json::JSON_Node as an instruction")
                current_node = *get<json::JSON_Node*>( instructions.front() );
            }
            instructions.pop_front();
        }

        // std::cout << "RESULT" << current_node.as<std::string>() << std::endl;
    }

template <typename T>
void ListGraph( const json::JSON_Parser& json_tree ) {
    std::cout << json_tree.has("@build") << std::endl;
    if ( json_tree.has("@build") ) {
        auto build_node = json_tree["@build"];

        if ( build_node.has("@nodes") ) {

            auto current_node = json_tree();
            std::cout << "Initiating node creation, steps:" << std::endl;
            std::list<json_entry_datatype> instructions;
            for ( auto instruction : build_node["@nodes"].as_list<std::string>() ) {
                instructions.push_back( instruction );
            }
            do_work<T>(current_node, instructions);
        }
    }
}




int main() {

    auto parser = JSON_Parser(true);
    parser.setDirectory("json");
    parser.importFile("sample_graph.json");
    parser.parse();
    std::cout << parser.display() << std::endl;

    ListGraph<std::string>( parser );

}
