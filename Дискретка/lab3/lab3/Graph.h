#pragma once
#include <string>
#include <vector>
#include <utility>
#include <set>

namespace Graph {

    using adjacency_list = std::vector<std::vector<int>>;

    class Graph {
    public:
        virtual ~Graph() = default;

        int GetCountVertex() const {
            return this->n;
        }

        virtual bool AreAdjacent(int u, int v) = 0;

    protected:
        int n = 0;
    };

    class ListGraph : public Graph {
    public:
        ListGraph(const std::string& filename);

        bool AreAdjacent(int u, int v) override;

        void Print();
        void BroneKerbosh();

        // Геттеры для результатов
        const std::vector<std::set<int>>& getMaximalIndependentSets() const { return maximalIndependentSets; }
        int getMaxIndependentSetSize() const { return maxIndependentSetSize; }

    private:
        void extend(std::set<int>& S, std::set<int> Q_plus, std::set<int> Q_minus);
        adjacency_list graph;

        // Члены класса для хранения результатов
        std::vector<std::set<int>> maximalIndependentSets;
        int maxIndependentSetSize = 0;
    };

}