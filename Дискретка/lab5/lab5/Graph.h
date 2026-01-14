#pragma once
#include <vector>
#include <string>

namespace Graph {

using adjancecy_matrix = std::vector<std::vector<int>>;

class Graph {
 public:
    int GetVertexies() const {
        return n;
    }
 protected:
    int n = 0;
};

class HungarianAlgorithm : public Graph{
 public:
     HungarianAlgorithm(std::string filename);

     int solve();
     std::vector<std::pair<int, int>> getMatching();
     void write_result();
 private:
    adjancecy_matrix cost;
    std::vector<int> Xa, Xb;
    std::vector<int> p, way;
};

}