#include <iostream>
#include "Graph.h"

int main(void) {
    Graph::ListGraph test("test/test3.txt");
    test.Kraskal();
    test.Prima();
    return 0;
}