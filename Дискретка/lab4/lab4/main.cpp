#include <iostream>
#include "Graph.h"

int main() {
    Graph::ListGraph test("test/4.txt");
    test.EdmondsKarp(0, 5);
    return 0;
}