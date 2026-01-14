#pragma once
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

namespace Graph {

using adjacency_list = std::vector<std::vector<std::pair<int, int>>>; // Вершина-Вес

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

class ListGraph : public Graph {
public:
    ListGraph(const std::string& filename);

    std::vector<std::pair<int, int>> BFS(int src, int dst, std::vector<std::vector<int>>& flow);
    void EdmondsKarp(int src, int dst);

    void PrintGraph() override;
private:
    adjacency_list graph;
};
}
