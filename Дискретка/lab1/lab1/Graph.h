#pragma once
#include <string>
#include <vector>
#include <utility>

namespace Graph {

using adjacency_list = std::vector<std::vector<std::pair<int, int>>>; // Вершина-Вес
using adjacency_matrix = std::vector<std::vector<int>>;

class Graph {
 public:
    virtual ~Graph() = default;

    int GetCountVertex() const {
        return this->n;
    }

    virtual void PrintGraph() = 0;

 protected:
    int n = 0;
};

class MatrixGraph : public Graph {
 public:
     MatrixGraph(const std::string& filename);

     void FloydWashall();
     void PrintGraph() override;

 private:
     adjacency_matrix graph;
};

class ListGraph : public Graph {
public:
    ListGraph(const std::string& filename);

    void Dijkstra(int vertex_index);
    void FordBellman(int vertex_index);

    void PrintGraph() override;
private:
    adjacency_list graph;
};
}
