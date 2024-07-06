// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

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
    DependencyGraph() = default;

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
    has_node(const T & a) const
    {
        return this->adj.find(a) != this->adj.end();
    }

    /// Check that edge exists
    ///
    /// @param a Start node
    /// @param b End node
    /// @return `true` if there is an edge from `a` to `b`, `false` otherwise
    bool
    has_edge(const T & a, const T & b) const
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
    std::vector<T>
    dfs(const std::vector<T> & nodes) const
    {
        std::set<T> rec_stack;
        std::vector<T> sorted_vector;
        std::map<T, bool> visited;
        std::stack<T> stack;
        for (auto n : nodes) {
            if (has_node(n)) {
                stack.push(n);
                rec_stack.insert(n);
            }
        }
        while (!stack.empty()) {
            T v = stack.top();
            sorted_vector.push_back(v);
            stack.pop();
            if (visited.find(v) == visited.end()) {
                visited[v] = true;
                for (auto & w : this->adj.at(v)) {
                    if (rec_stack.find(w) == rec_stack.end()) {
                        stack.push(w);
                        rec_stack.insert(w);
                    }
                    else
                        throw std::runtime_error("Cyclic dependency detected");
                }
            }
        }
        return sorted_vector;
    }

    /// Breadth-first search
    std::vector<T>
    bfs(const std::vector<T> & nodes) const
    {
        std::set<T> rec_stack;
        std::vector<T> sorted_vector;
        std::map<T, bool> explored;
        std::queue<T> queue;
        for (auto n : nodes) {
            if (has_node(n)) {
                queue.push(n);
                rec_stack.insert(n);
            }
        }
        while (!queue.empty()) {
            T v = queue.front();
            sorted_vector.push_back(v);
            queue.pop();
            for (auto & w : this->adj.at(v)) {
                if (explored.find(w) == explored.end()) {
                    explored[w] = true;
                    if (rec_stack.find(w) == rec_stack.end()) {
                        queue.push(w);
                        rec_stack.insert(w);
                    }
                    else
                        throw std::runtime_error("Cyclic dependency detected");
                }
            }
        }
        return sorted_vector;
    }

private:
    /// adjacency
    std::map<T, std::set<T>> adj;
};

} // namespace godzilla
