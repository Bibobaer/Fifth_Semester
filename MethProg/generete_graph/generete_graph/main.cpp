#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <ranges>
#include <optional>

using namespace std;

struct Graph {
    int n;
    vector<vector<int>> adj;

    Graph(int vertices) : n(vertices), adj(vertices) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void addDoubleEdge(int u, int v) {
        addEdge(u, v);
        addEdge(u, v);
    }

    bool hasEdge(int u, int v) const {
        return ranges::any_of(adj[u], [v](int neighbor) {
            return neighbor == v;
            });
    }

    int degree(int vertex) const {
        return adj[vertex].size();
    }

    int countEdgesBetween(int u, int v) const {
        return ranges::count(adj[u], v);
    }

    void removeEdge(int u, int v) {
        auto remove_from_list = [](vector<int>& list, int value) {
            auto it = ranges::find(list, value);
            if (it != list.end()) {
                list.erase(it);
            }
            };

        remove_from_list(adj[u], v);
        remove_from_list(adj[v], u);
    }

    void removeDoubleEdge(int u, int v) {
        removeEdge(u, v);
        removeEdge(u, v);
    }
};

bool isConnected(const Graph& graph) {
    if (graph.n == 0) return true;

    vector<bool> visited(graph.n, false);
    vector<int> stack;

    auto first_non_empty = ranges::find_if(graph.adj, [](const auto& neighbors) {
        return !neighbors.empty();
        });

    if (first_non_empty == graph.adj.end()) return false;

    int start = distance(graph.adj.begin(), first_non_empty);
    stack.push_back(start);
    visited[start] = true;

    while (!stack.empty()) {
        int current = stack.back();
        stack.pop_back();

        for (int neighbor : graph.adj[current]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                stack.push_back(neighbor);
            }
        }
    }

    return ranges::all_of(views::iota(0, graph.n), [&](int i) {
        return graph.adj[i].empty() || visited[i];
        });
}

bool isEulerian(const Graph& graph) {
    if (graph.n == 0) return true;

    int odd_count = ranges::count_if(views::iota(0, graph.n), [&](int i) {
        return graph.degree(i) % 2 != 0;
        });

    return (odd_count == 0 || odd_count == 2) && isConnected(graph);
}

int findMinDegreeVertex(const Graph& graph, const unordered_set<int>& excluded = {}) {
    auto vertices = views::iota(0, graph.n)
        | views::filter([&](int v) { return !excluded.contains(v); });

    return *ranges::min_element(vertices, [&](int a, int b) {
        return graph.degree(a) < graph.degree(b);
        });
}

optional<int> findSuitableVertex(const Graph& graph, int u) {
    auto candidates = views::iota(0, graph.n)
        | views::filter([&](int v) {
        return v != u && !graph.hasEdge(u, v);
            });

    auto it = ranges::min_element(candidates, [&](int a, int b) {
        return graph.degree(a) < graph.degree(b);
        });

    return it != candidates.end() ? optional(*it) : nullopt;
}

Graph generateRandomEulerianGraph(int n, int m) {
    if (n <= 0) throw invalid_argument("n must be positive");
    if (m < 0 || m > n * (n - 1) / 2) {
        throw invalid_argument("Invalid number of edges");
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> vertex_dist(0, n - 1);

    Graph graph(n);

    // Создать цикл
    vector<int> vertices(n);
    iota(vertices.begin(), vertices.end(), 0);
    ranges::shuffle(vertices, gen);

    for (int i = 0; i < n; ++i) {
        graph.addEdge(vertices[i], vertices[(i + 1) % n]);
    }

    int currentEdges = n;

    // Докинуть пары ребер
    while (currentEdges < m) {
        int u = findMinDegreeVertex(graph);

        if (auto v = findSuitableVertex(graph, u); v.has_value()) {
            graph.addDoubleEdge(u, *v);
            currentEdges += 2;
        }
        else {
            // Нет походящий - докидываем случайные вершины
            int u_rand = vertex_dist(gen);
            int v_rand = vertex_dist(gen);

            if (u_rand != v_rand && !graph.hasEdge(u_rand, v_rand)) {
                graph.addDoubleEdge(u_rand, v_rand);
                currentEdges += 2;
            }
        }
    }

    // Убираем лишние
    while (currentEdges > m) {
        bool removed = false;

        // Ищем пару вершин с кратными рёбрами
        for (int u = 0; u < n && !removed; ++u) {
            for (int v = u + 1; v < n && !removed; ++v) {
                if (graph.countEdgesBetween(u, v) >= 2) {
                    graph.removeDoubleEdge(u, v);
                    currentEdges -= 2;
                    removed = true;
                }
            }
        }

        if (!removed) break;
    }

    return graph;
}

void printGraph(const Graph& graph) {
    cout << "Graph with " << graph.n << " vertices:\n";
    for (int i = 0; i < graph.n; ++i) {
        cout << "Vertex " << i << " (degree " << graph.degree(i) << "): ";
        ranges::copy(graph.adj[i], ostream_iterator<int>(cout, " "));
        cout << "\n";
    }
}

int countTotalEdges(const Graph& graph) {
    int total = transform_reduce(
        graph.adj.begin(), graph.adj.end(), 0, plus<>(),
        [](const auto& neighbors) { return neighbors.size(); }
    );
    return total / 2;
}

int main() {
    try {
        int n = 6;
        int m = 9;

        if (m % 2 != 0) ++m;

        cout << "Generating random Eulerian graph with " << n
            << " vertices and " << m << " edges...\n";

        Graph graph = generateRandomEulerianGraph(n, m);

        printGraph(graph);

        cout << "\nTotal edges: " << countTotalEdges(graph) << "\n";
        cout << "Is Eulerian: " << (isEulerian(graph) ? "YES" : "NO") << "\n";

    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}