#pragma once

#include <set>
#include <vector>
#include <map>
#include <stack>
#include <queue>

namespace godzilla {

/// Template for capturing dependencies via a directed acyclic graph
template <typename T>
class DependencyGraph {
public:
    DependencyGraph() {}

    /// Add node into the graph.
    ///
    /// @param node Node to add to the graph
    void
    add_node(const T & node)
    {
        if (this->adj.find(node) == this->adj.end())
            this->adj[node] = {};
    }

    /// Add an edge into the graph (`a` depends on `b`)
    ///
    /// @param a Node A of an edge
    /// @param b Node B of an edge
    void
    add_edge(const T & a, const T & b)
    {
        add_node(a);
        add_node(b);
        this->adj[a].insert(b);
    }

    /// Check that node exists
    ///
    /// @param a Node to check
    /// @return `true` if node exists in the graph, `false` otherwise
    bool
    has_node(const T & a)
    {
        return this->adj.find(a) != this->adj.end();
    }

    /// Check that edge exists
    ///
    /// @param a Start node
    /// @param b End node
    /// @return `true` if there is an edge from `a` to `b`, `false` otherwise
    bool
    has_edge(const T & a, const T & b)
    {
        auto it = this->adj.find(a);
        if (it != this->adj.end()) {
            const auto & set = it->second;
            return set.find(b) != set.end();
        }
        else
            return false;
    }

    /// Clear the graph
    void
    clear()
    {
        this->adj.clear();
    }

    /// Depth-first search
    const std::vector<T> &
    dfs(const std::vector<T> & nodes)
    {
        this->rec_stack.clear();
        this->sorted_vector.clear();
        std::map<T, bool> visited;
        std::stack<T> stack;
        for (auto n : nodes) {
            if (has_node(n)) {
                stack.push(n);
                this->rec_stack.insert(n);
            }
        }
        while (!stack.empty()) {
            T v = stack.top();
            this->sorted_vector.push_back(v);
            stack.pop();
            if (visited.find(v) == visited.end()) {
                visited[v] = true;
                for (auto & w : this->adj[v]) {
                    if (this->rec_stack.find(w) == this->rec_stack.end()) {
                        stack.push(w);
                        this->rec_stack.insert(w);
                    }
                    else
                        throw std::runtime_error("Cyclic dependency detected");
                }
            }
        }
        return this->sorted_vector;
    }

    /// Breadth-first search
    const std::vector<T> &
    bfs(const std::vector<T> & nodes)
    {
        this->rec_stack.clear();
        this->sorted_vector.clear();
        std::map<T, bool> explored;
        std::queue<T> queue;
        for (auto n : nodes) {
            if (has_node(n)) {
                queue.push(n);
                this->rec_stack.insert(n);
            }
        }
        while (!queue.empty()) {
            T v = queue.front();
            this->sorted_vector.push_back(v);
            queue.pop();
            for (auto & w : this->adj[v]) {
                if (explored.find(w) == explored.end()) {
                    explored[w] = true;
                    if (this->rec_stack.find(w) == this->rec_stack.end()) {
                        queue.push(w);
                        this->rec_stack.insert(w);
                    }
                    else
                        throw std::runtime_error("Cyclic dependency detected");
                }
            }
        }
        return this->sorted_vector;
    }

protected:
    /// adjacency
    std::map<T, std::set<T>> adj;
    /// "sorted" vector of nodes
    std::vector<T> sorted_vector;
    /// recursive stack
    std::set<T> rec_stack;
};

} // namespace godzilla
