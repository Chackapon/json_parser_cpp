//
// Created by Mateusz M on 08/06/2026.
//

#include "../include/library.h"
#include <iostream>

using namespace json;





int main() {
    // auto test_int = new JSON_Pair<JSON_Pair<float>*>("test", new JSON_Pair<float>("value", 5.2) );
    // std::cout << test_int->getString() << std::endl;

    auto parser = JSON_Parser(true);
    parser.setDirectory("json");
    parser.importFile("sample_graph.json");
    // parser.importString("{\"test\":{\"value\": {\"5\": 0.5}}, \"flt\":[\"lorem\",\"test\", \"meow\"]}");
    // // parser.importString("{\"value\":5}");
    parser.parse();
    std::cout << parser.display() << std::endl;



    std::cout << 2*parser["test"]["meow"].as<int>() << std::endl;

}
