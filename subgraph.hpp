#include <queue>
#include <set>
#include <vector>

#include "graph.hpp"

using VertexId = int;
using VertexSet = int;

VertexId GetVertexId(Vertex vertex) {
    return (1 << vertex);
}

int VerticesCount(VertexSet set) {
    return __builtin_popcount(set);
}

VertexSet GetVertexSetFromVector(const std::vector<Vertex>& vertices) {
    VertexSet set = 0;
    for (auto vertex : vertices) {
        set &= (~GetVertexId(vertex));
    }
    return set;
}



struct SubGraph {
    VertexSet set;
    Graph &graph;

    int VerticesCount() const {
        return __builtin_popcount(set);
    }

    bool IsVertexInSet(Vertex vertex) const {
        return (GetVertexId(vertex) & set);
    }

    void AddVertexToSet(Vertex vertex) {
        set ^= GetVertexId(vertex);
    }

    SubGraph GetSubGraph(VertexSet other) const{
        return SubGraph{set & other, graph};
    }

    std::vector<Vertex> GetChildren(Vertex vertex) const {
        auto children = graph.GetChildren(vertex);
        std::vector<Vertex> children_in_set;
        for (auto child : children) {
            if (IsVertexInSet(child)) {
                children_in_set.push_back(child);
            }
        }
        return children_in_set;
    }

    int GetVertexDegree(Vertex vertex) const {
        return GetChildren(vertex).size();
    }

    std::vector<std::pair<int, Vertex>> GetVerticesDegrees() const {
        std::vector<std::pair<int, Vertex>> degrees;
        for (int vertex = 0; vertex < MAX_VERTICES_COUNT; ++vertex) {
            if (!IsVertexInSet(vertex)) {
                continue;
            }
            int degree = 0;
            for (auto next : graph.GetChildren(vertex)) {
                if (IsVertexInSet(next)) {
                    ++degree;
                }
            }
            degrees.emplace_back(degree, vertex);
        }
        return degrees;
    }

    std::pair<Vertex, Vertex> GetPair() {
        int first = set - (set & (set - 1));
        set -= first;
        int second = set - (set & (set - 1));
        set += first;
        return std::make_pair(first, second);
    }

    std::vector<SubGraph> GetConnectedSubGraphs() const {
        std::vector<int> used(MAX_VERTICES_COUNT, 0);
        std::vector<SubGraph> connected_sets;
        for (Vertex vertex = 0; vertex < MAX_VERTICES_COUNT; ++vertex) {
            if (used[vertex] || !IsVertexInSet(vertex)) {
                continue;
            }

            std::queue<Vertex> queue;
            queue.push(vertex);
            while (!queue.empty()) {
                int v = queue.front();
                queue.pop();
                used[v] = 1;
                for (auto u : graph.GetChildren(v)) {
                    if (!used[u] && IsVertexInSet(u)) {
                        queue.push(u);
                    }
                }
            }

            SubGraph current_graph{0, graph};
            for (int i = 0; i < MAX_VERTICES_COUNT; ++i) {
                if (used[i] == 1) {
                    current_graph.AddVertexToSet(i);
                    used[i] = 2;
                }
            }

            connected_sets.push_back(current_graph);
        }

        return connected_sets;
    }

};







