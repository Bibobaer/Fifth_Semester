#include <iostream>
#include "Graph.h"

int main() {
    Graph::MatrixGraph test("test/test5.txt");
    test.FloydWashall();

    Graph::ListGraph test2("test/test5.txt");
    test2.Dijkstra(0);
    test2.FordBellman(0);
    return 0;
}