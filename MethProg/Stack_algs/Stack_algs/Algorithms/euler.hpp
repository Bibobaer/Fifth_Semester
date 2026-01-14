#pragma once
#include <vector>
#include <stack>

namespace Euler {
	using adjacency_list = std::vector<std::vector<int>>;
	using vertex = std::vector<int>;

	std::vector<int> findEulerPath(const adjacency_list& graph) {
		if (graph.size() == 0) return {};

		std::stack<int> S;
		std::vector<int> out;

		std::vector<std::vector<bool>> used_edges( graph.size(), std::vector<bool>(graph.size(), false) );

		int count_odd_vertex = 0;
		int start_vertex = 0;

		for (int i = 0; i < graph.size(); i++) {
			if (graph[i].size() % 2 != 0) {
				count_odd_vertex++;
				start_vertex = i;
			}
		}

		if (count_odd_vertex != 0 && count_odd_vertex != 2) return {};
		S.push((count_odd_vertex == 2) ? start_vertex : 0);

		while (S.size()) {
			int v = S.top();
			bool found = false;

			for (auto& u : graph[v]) {
				if (!used_edges[v][u]) {
					used_edges[v][u] = true;
					used_edges[u][v] = true;

					S.push(u);

					found = true;
					break;
				}
			}

			if (!found) {
				out.push_back(v);
				S.pop();
			}
		}
		return out;
	}

	adjacency_list Generate_graph(int n, int m) {

	}
}