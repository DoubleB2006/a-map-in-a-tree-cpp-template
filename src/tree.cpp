#include <iostream>
#include "tree.hpp"

int run() {
    TreeMap map;

    map.insert("keyOne", "valueOne");
    map.insert("keyTwo", "valueTwo");
    map.insert("keyThree", "valueThree");

    std::cout << map.get("keyOne") << "\n";
    std::cout << map.get("keyThree") << "\n";
    std::cout << "[" << map.get("keyDoesNotExist") << "]\n";

    map.deleteKey("keyOne");

    return 0;
}
