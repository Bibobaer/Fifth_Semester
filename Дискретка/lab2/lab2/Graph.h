#pragma once
#include <string>
#include <vector>
#include <utility>

namespace Graph {

using adjacency_list = std::vector<std::vector<std::pair<int, int>>>;
class Graph {
 public:
    virtual ~Graph() = default;

    int GetCountVertex() const {
        return this->n;
    }

    virtual bool IsConnected() = 0;
    virtual void PrintGraph() = 0;

 protected:
    int n = 0;
};

class ListGraph : public Graph {
public:
    ListGraph(const std::string& filename);

    void Kraskal();
    void Prima();

    void PrintGraph() override;

    bool IsConnected() override;
private:
    adjacency_list graph;
};

}

