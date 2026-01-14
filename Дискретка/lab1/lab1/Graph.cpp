#include "Graph.h"

#include <fstream>
#include <iostream>
#include <numeric>
#include <chrono>

Graph::MatrixGraph::MatrixGraph(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cant open file");

    file >> this->n;

    graph.resize(this->n, std::vector<int>(this->n));

    for (auto& row : graph)
        for (auto& element : row)
            file >> element;
    
    file.close();
}

void Graph::MatrixGraph::FloydWashall() {
    adjacency_matrix out(graph);

    auto start = std::chrono::high_resolution_clock::now();
    for (int k = 0; k < this->n; k++) {
        for (int i = 0; i < this->n; i++) {
            for (int j = 0; j < this->n; j++) {
                if ((out[i][j] == 0 || out[i][j] > (out[i][k] + out[k][j])) && (out[k][j] != 0 && out[i][k] != 0) && i != j) {
                    out[i][j] = out[i][k] + out[k][j];
                }
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto working_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    auto total_ms = working_time.count();
    long long minutes = total_ms / 60000;
    long long seconds = (total_ms % 60000) / 1000;
    long long milliseconds = total_ms % 1000;

    std::ofstream output("Floyd-Washall_output.txt");
    if (!output.is_open())
        return;

    output << "Time working: " << minutes << "m " << seconds << "s " << milliseconds << "ms\n";
    for (int i = 0; i < this->n; i++) {
        output << "Vertex " << i << ": ";
        for (int j = 0; j < this->n; j++) {
            if (out[i][j] == 0 && i != j)
                output << "inf ";
            output << out[i][j] << " ";
        }
        output << "\n";
    }

    output.close();
}

void Graph::MatrixGraph::PrintGraph() {
    for (const auto& row : graph) {
        for (const auto& vertex : row) {
            std::cout << vertex << " ";
        }
        std::cout << "\n";
    }
}
// ---------------------------------------------------------------
Graph::ListGraph::ListGraph(const std::string& filename) {
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
}

void Graph::ListGraph::Dijkstra(int vertex_index) {
    if (vertex_index < 0 || vertex_index >= this->n)
        throw std::runtime_error("Invalid Argument");

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<int> out(this->n, std::numeric_limits<int>::max());
    std::vector<bool> visited(this->n, false);

    out[vertex_index] = 0;

    for (int i = 0; i < this->n; i++) {
        int minimum = std::numeric_limits<int>::max();
        int min_vertex = -1;

        for (int j = 0; j < this->n; j++) {
            if (visited[j] == false && out[j] <= minimum) {
                minimum = out[j];
                min_vertex = j;
            }
        }

        if (min_vertex == -1) continue;

        visited[min_vertex] = true;

        for (const auto& neighbor : graph[min_vertex]) {
            const int& v = neighbor.first;
            const int& weight = neighbor.second;

            if (!visited[v] && out[v] > out[min_vertex] + weight) {
                out[v] = out[min_vertex] + weight;
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto working_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    auto total_ms = working_time.count();
    long long minutes = total_ms / 60000;
    long long seconds = (total_ms % 60000) / 1000;
    long long milliseconds = total_ms % 1000;

    std::ofstream output("Dijkstra_output.txt");
    if (!output.is_open())
        return;

    output << "Time working: " << minutes << "m " << seconds << "s " << milliseconds << "ms\n";
    output << "Vertex " << vertex_index << ": ";
    for (int i = 0; i < this->n; i++) {
        output << out[i] << " ";
    }
    output.close();
}

void Graph::ListGraph::FordBellman(int vertex_index) {
    if (vertex_index < 0 || vertex_index >= this->n)
        throw std::runtime_error("Invalid Argument");

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<int> dist(this->n, std::numeric_limits<int>::max());
    dist[vertex_index] = 0;

    bool changed;
    int iteration_count = 0;

    do {
        changed = false;
        iteration_count++;

        for (int u = 0; u < this->n; u++) {
            if (dist[u] != std::numeric_limits<int>::max()) {
                for (const auto& edge : graph[u]) {
                    const int& w = edge.first;
                    const int& weight = edge.second;

                    if (dist[u] + weight < dist[w]) {
                        dist[w] = dist[u] + weight;
                        changed = true;
                    }
                }
            }
        }

        if (!changed) {
            break;
        }
        if (iteration_count >= this->n - 1) {
            if (changed) {
                std::cout << "Has negative cycle\n";
            }
        }

    } while (changed && iteration_count < this->n - 1);

    auto end = std::chrono::high_resolution_clock::now();
    auto working_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    auto total_ms = working_time.count();
    long long minutes = total_ms / 60000;
    long long seconds = (total_ms % 60000) / 1000;
    long long milliseconds = total_ms % 1000;

    std::ofstream output("Ford-Bellman_output.txt");
    if (!output.is_open())
        return;

    output << "Time working: " << minutes << "m " << seconds << "s " << milliseconds << "ms\n";
    output << "Vertex " << vertex_index << ": ";
    for (int i = 0; i < this->n; i++) {
        output << dist[i] << " ";
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
