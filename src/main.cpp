//
// Created by Mateusz M on 08/06/2026.
//

#include "../../JSON_Parser/include/JsonParserLib/JSON_Parser.hpp"
#include "../include/GraphLib/ListGraph.hpp"
#include <iostream>

using namespace json;





int main() {

    auto parser = JSON_Parser(true);
    parser.setDirectory("json");
    parser.importFile("GrafZeStronyZProjektami.json");
    parser.parse();
    std::cout << parser.display() << std::endl;

    const auto graph_one = ListGraph<std::string>( parser, true );
    graph_one.display();

    parser.importFile("ListGraph.json");
    parser.parse();
    std::cout << parser.display() << std::endl;

    const auto graph_two = ListGraph<std::string>( parser );
    graph_two.display();

    // parser.importFile("ListGraph.json");
    // parser.parse();
    // std::cout << parser.display() << std::endl;
    // auto graph_two = ListGraph<std::string>( parser );

}
