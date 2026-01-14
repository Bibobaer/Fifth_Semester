#include "Graph.h"

#include <fstream>
#include <iostream>
#include <numeric>
#include <chrono>
#include <queue>

// ---------------------------------------------------------------
Graph::ListGraph::ListGraph(const std::string& filename) {
    std::cout << "Start const\n";
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cant open file");

    file >> this->n;

    graph.resize(this->n);

    int weight;
    for (int i = 0; i < this->n; i++) {
        for (int j = 0; j < this->n; j++) {
            file >> weight;
            if (weight != 0)
                graph[i].push_back(std::make_pair(j, std::move(weight)));
        }
    }
    std::cout << "End const\n";
}

std::vector<std::pair<int, int>> Graph::ListGraph::BFS(int src, int dst, std::vector<std::vector<int>>& flow) {
    if (src == dst) return {};

    std::queue<int> Q;
    Q.push(src);

    std::unordered_map<int, std::vector<std::pair<int, int>>> paths;
    paths[src] = {};

    while (Q.size()) {
        int u = Q.front(); Q.pop();

        for (auto& neighbor : graph[u]) {
            int& v = neighbor.first;
            int& capacity = neighbor.second;

            if (capacity - flow[u][v] > 0 && paths.find(v) == paths.end()) {
                paths[v] = paths[u];
                paths[v].emplace_back(u, v);

                if (v == dst) {
                    return paths[v];
                }

                Q.push(v);
            }
        }
    }

    return {};
}


void Graph::ListGraph::EdmondsKarp(int src, int dst) {
    int n = graph.size();

    std::vector<std::vector<int>> flow(n, std::vector<int>(n, 0));
    int max_flow = 0;

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::pair<int, int>> path = BFS(src, dst, flow);

    // пока существует увеличивающий путь
    while (!path.empty()) {
        int path_flow = std::numeric_limits<int>::max();
        // находим минимальный путь от src до dst
        for (auto& [u, v] : path) {
            path_flow = std::min(path_flow, [&]() {
                auto it = std::find_if(graph[u].begin(), graph[u].end(),
                                        [v](const auto& e) 
                                        { return e.first == v; });
                return (it != graph[u].end() ? it->second : 0) - flow[u][v];
                }());
        }

        // обновляем потоки в пути
        for (const auto& edge : path) {
            int u = edge.first;
            int v = edge.second;

            flow[u][v] += path_flow;
            flow[v][u] -= path_flow;
        }

        max_flow += path_flow;

        // ищем следующий путь
        path = BFS(src, dst, flow);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto working_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << max_flow << "\n";

    auto total_ms = working_time.count();
    long long minutes = total_ms / 60000;
    long long seconds = (total_ms % 60000) / 1000;
    long long milliseconds = total_ms % 1000;

    std::ofstream output("Edmonds-Karp.txt");
    if (!output.is_open())
        return;

    std::cout << "Time working: " << minutes << "m " << seconds << "s " << milliseconds << "ms\n";
    output << "Time working: " << minutes << "m " << seconds << "s " << milliseconds << "ms\n";

    int fn = flow.size();
    for (int i = 0; i < fn; i++) {
        for (int j = 0; j < fn; j++) {
            output << flow[i][j];
            if (j < fn - 1) output << " ";
        }
        if (i < fn - 1) output << "\n";
    }
    output.close();
}

void Graph::ListGraph::PrintGraph() {
    std::cout << "[ ";
    for (const auto& vertex : graph) {
        std::cout << "[ ";

        for (const auto& connected : vertex) {
            std::cout << "(" << connected.first << ", " << connected.second << ") ";
        }

        std::cout << "], ";
    }
    std::cout << " ]\n";
}
