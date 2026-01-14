#include "Pruffer.h"
#include <queue>
#include <iostream>
#include <algorithm>
#include <ranges>

std::vector<int> BFS(const Pruffer::adjacency_list& graph, int start_vertex) {
    if (start_vertex < 0 || start_vertex >= graph.size())
        throw std::runtime_error("Error ");

    std::queue<int> Q;
    std::vector<int> parents(graph.size(), -1);

    Q.push(start_vertex);
    parents[start_vertex] = start_vertex;

    while (Q.size()) {
        auto u = Q.front();
        Q.pop();

        for (auto& v : graph[u]) {
            if (parents[v] == -1) {
                parents[v] = u;
                Q.push(v);
            }
        }
    }
    return parents;
}
// -------------------------------------------------------------------------------
std::vector<int> Pruffer::Encoder::EncodePruffer(const adjacency_list& graph) {
    std::vector<int> code;
    size_t graph_size = graph.size();

    auto parents = BFS(graph, static_cast<int>(graph_size) - 1);
    int candidate = -1;
    int current = 0;
    std::vector<int> degree(graph_size);

    std::transform(graph.begin(), graph.end(), degree.begin(), [](auto& adj) {return static_cast<int>(adj.size()); });

    auto push_to_code = [&](const int& index) {
        code.push_back(parents[index]);
        degree[parents[index]]--;
        candidate = (degree[parents[index]] == 1 && parents[index] < current) ? parents[index] : -1;
        };

    while ((candidate >= 0 || current < graph_size) && code.size() < graph_size - 2) {
        if (candidate >= 0) {
            push_to_code(candidate);
        }
        else {
            while (degree[current] > 1) current++;
            push_to_code(current);
            current++;
        }
    }
    return code;
}

std::vector<int> Pruffer::Encoder::EncodePrufferPQ(const adjacency_list& graph) {
    std::vector<int> code;
    size_t graph_size = graph.size();

    std::priority_queue<int, std::vector<int>, std::greater<int>> Q;

    std::vector<int> degree(graph_size);
    std::transform(graph.begin(), graph.end(), degree.begin(), [](auto& adj) {return static_cast<int>(adj.size());});

    std::vector<bool> deleted(graph_size, false);

    for (int i = 0; i < graph_size; i++)
        if (degree[i] == 1) 
            Q.push(i);

    for (int step = 0; step < graph_size - 2; step++) {
        int v = Q.top(); Q.pop();

        auto u = *std::find_if(graph[v].begin(), graph[v].end(), [&deleted](auto& ver){return !deleted[ver];});

        if (--degree[u] == 1) Q.push(u);
        code.push_back(u);
        deleted[v] = true;
    }

    return code;
}

Pruffer::adjacency_list Pruffer::Decoder::DecodePruffer(const std::vector<int>& code) {
    size_t n = code.size() + 2;
    adjacency_list graph(n);

    std::vector<int> degree(n, 1);
    std::for_each(code.begin(), code.end(), [&](auto& ver){ degree[ver]++; });

    int candidate = -1;
    int current = 0;

    auto add_edge = [&](const int& u, const int& v) {
        graph[u].push_back(v);
        graph[v].push_back(u);
        degree[u]--;
        degree[v]--;
        candidate = (degree[v] == 1 && v < current) ? v : -1;
    };

    for (auto& ver : code) {
        if (candidate >= 0) {
            add_edge(candidate, ver);
        } else {
            while (degree[current] != 1) current++;
            add_edge(current, ver);
            current++;
        }
    }

    auto u = std::find_if(degree.begin(), degree.end(), [](int d){return d == 1;}) - degree.begin();
    graph[u].push_back(n-1);
    graph[n-1].push_back(u);

    return graph;
}

Pruffer::adjacency_list Pruffer::Decoder::DecodePrufferPQ(const std::vector<int>& code) {
    if (code.size() == 2)
        return { {1}, {0} };

    int n = static_cast<int>(code.size()) + 2;
    adjacency_list graph(n);

    std::vector<int> degree(n);
    std::for_each(code.begin(), code.end(), [&](auto& ver){ degree[ver]++;});

    std::priority_queue<std::pair<int, int>, 
                        std::vector<std::pair<int, int>>, 
                        std::greater<std::pair<int, int>>> Q;

    for (int i = 0; i < n; i++)
        Q.push({degree[i], i});

    std::vector<bool> uses(n, false);

    for (auto& u : code) {
        while (Q.size() && uses[Q.top().second]) {
            Q.pop();
        }

        auto [deg_v, v] = Q.top();
        Q.pop();

        graph[u].push_back(v);
        graph[v].push_back(u);

        uses[v] = true;

        Q.push({--degree[u], u});
    }

    std::vector<int> remaining;
    std::ranges::copy_if(std::views::iota(0, n),
        std::back_inserter(remaining),
        [&](int i) { return !uses[i]; });
    
    graph[remaining[0]].push_back(remaining[1]);
    graph[remaining[1]].push_back(remaining[0]);

    return graph;
}