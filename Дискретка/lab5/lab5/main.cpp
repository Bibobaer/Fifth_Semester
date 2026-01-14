#include <iostream>
#include "Graph.h"

int main() {
    Graph::HungarianAlgorithm test("test.txt");
    test.write_result();
    return 0;
}