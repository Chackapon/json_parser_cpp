//
// Created by Mateusz Miliutin on 08/06/2026.
//

#include "../include/library.h"
#include <iostream>

using namespace json;





int main() {
    // auto test_int = new JSON_Pair<JSON_Pair<float>*>("test", new JSON_Pair<float>("value", 5.2) );
    // std::cout << test_int->getString() << std::endl;

    auto parser = JSON_Tree(true);
    parser.setDirectory("json");
    parser.importFile("sample_graph.json");
    // parser.importString("{\"test\":{\"value\": {\"5\": 0.5}},\"flt\":6.7}");
    // parser.importString("{\"value\":5}");
    parser.parse();
    std::cout << parser.display() << std::endl;

    // auto test = parser["graph"];
    const std::string str_test = parser["graph"].value<std::string>("implementation");
    std::cout << "value: " << str_test << std::endl;
    const bool bool_test = parser["graph"].value<bool>("directed");
    std::cout << "value: " << bool_test << std::endl;
    const float float_test = parser["graph"]["content"]["2"].value<float>("0");
    std::cout << "value: " << float_test << std::endl;
    // auto value = test["@root"];
    // delete test_int;
}
