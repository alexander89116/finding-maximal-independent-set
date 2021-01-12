#include <set>
#include <vector>

const int MAX_VERTICES_COUNT = 32;

using Vertex = int;

struct Graph {
    std::vector<std::set<Vertex>> edges;

    bool HasEdge(Vertex from, Vertex to) const {
        return (edges[from].find(to) != edges[from].end());
    }

    std::vector<Vertex> GetChildren(Vertex vertex) const {
        std::vector<Vertex> children;
        for (auto child : edges[vertex]) {
            children.push_back(child);
        }
        return children;
    }

    int GetDegree(Vertex vertex) const {
        return edges[vertex].size();
    }
};

