#include <iostream>
#include "main.hpp"

int main(void) {

	/*std::vector<Shell::Point> test = { Shell::Point{0, 0}, Shell::Point{0, 1}, Shell::Point{2, 2}, Shell::Point{4, -1}, Shell::Point{1, 1} };


	auto res = Shell::make_convex_shell(test);

	for (auto& el : res) {
		std::cout << "(" << el.first << ", " << el.second << "), ";
	}*/

	std::vector<std::vector<int>> undirGraph = { {1}, {0, 2}, {0, 1, 3}, {2} };
	auto path = Euler::findEulerPath(undirGraph);

	for (auto& el : path) {
		std::cout << el << " -> ";
	}

	return 0;
}