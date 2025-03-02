#pragma once

#include "Graph/common.hpp"
#include <iomanip>
#include <iostream>
#include <numeric>
#include <queue>
#include <ranges>
#include <stack>
#include <utility>
#include <vector>

namespace graph {
    class graph_t final {
        size_t count_verts_;
        size_t count_edges_;

        std::vector<int> a_;
        std::vector<int> t_;
        std::vector<int> n_;
        std::vector<int> p_;

        using edge_t = std::tuple<size_t, size_t, int>; // v1 -- v2, w

    private:
        static std::istream& read_edge(std::istream& is, edge_t& edge) {
            char dummy;
            int v1_, v2_, w_;
            is >> v1_ >> std::ws >> dummy >> dummy >> std::ws >> v2_ >> dummy >> std::ws >> w_;

            if (!is.good() && !is.eof())
                throw error_t{str_red("Invalid edge")};
            if (v1_ <= 0 && !is.eof())
                throw error_t{str_red("Vertex(1, src) index <= 0")};
            if (v2_ <= 0 && !is.eof())
                throw error_t{str_red("Vertex(2, dst) index <= 0")};

            auto& [v1, v2, w] = edge;
            v1 = static_cast<size_t>(v1_ - 1);
            v2 = static_cast<size_t>(v2_ - 1);
            w = w_;
            return is;
        }

        void create(const std::vector<std::vector<std::pair<int, int>>>& edges) {
            size_t summary_count = count_verts_ + 2 * count_edges_;

            a_.resize(summary_count);
            t_.resize(summary_count);
            n_.resize(summary_count);
            p_.resize(summary_count);

            iota(a_.begin(), a_.end(), 0);
            iota(n_.begin(), n_.end(), 0);
            iota(p_.begin(), p_.end(), 0);
            fill(t_.begin(), t_.begin() + count_verts_, 0);

            int idx = count_verts_;
            std::vector<int> curr_idx(count_verts_);
            iota(curr_idx.begin(), curr_idx.end(), 0);

            for (size_t i = 0; i < count_verts_; ++i) {
                for (auto edge : edges[i]) {
                    int v1 = i;
                    int v2 = edge.first;

                    t_[idx]     = v1 + 1;
                    t_[idx + 1] = v2 + 1;

                    n_[curr_idx[v1]] = idx;
                    n_[curr_idx[v2]] = idx + 1;

                    p_[curr_idx[v1]] = std::exchange(curr_idx[v1], idx);
                    p_[curr_idx[v2]] = std::exchange(curr_idx[v2], idx + 1);

                    idx += 2;
                }
            }

            for (size_t i = 0; i < count_verts_; ++i) {
                n_[curr_idx[i]] = i;
                p_[i] = curr_idx[i];
            }
        }

    public:
        template <typename Func, typename... Args>
        void dfs(int start, Func func, Args&&... args) const {
            start--;
            std::stack<int> s;
            std::vector<bool> used(count_verts_, false);
            std::vector<int> order;
            order.reserve(count_verts_);

            used[start] = true;
            s.push(start);
            while (!s.empty()) {
                int v = s.top();
                s.pop();
                order.push_back(v);

                for (int i = n_[v]; i != v; i = n_[i]) {
                    int next = t_[i ^ 1] - 1;
                    if (!used[next]) {
                        used[next] = true;
                        s.push(next);
                    }
                }
            }

            for (auto v : std::views::reverse(order))
                func(v, std::forward<Args>(args)...);
        }

        template <typename Func, typename... Args>
        void bfs(int start, Func func, Args&&... args) const {
            start--;
            
            std::vector<bool> used(count_verts_, false);
            std::vector<int> order;
            std::queue<int> q;

            used[start] = true;
            q.push(start);

            while (!q.empty()) {
                int v = q.front();
                q.pop();
                order.push_back(v);

                for (int i = n_[v]; i != v; i = n_[i]) {
                    int next = t_[i ^ 1] - 1;
                    if (!used[next]) {
                        used[next] = true;
                        q.push(next);
                    }
                }
            }

            for (auto v : order)
                func(v, std::forward<Args>(args)...);
        }

        std::istream& read(std::istream& is) {
            count_edges_ = 0;
            count_verts_ = 0;
            edge_t edge;
            std::vector<std::vector<std::pair<int, int>>> edges;
            while (read_edge(is, edge)) {
                auto& [v1, v2, w] = edge;
                count_verts_ = std::max(count_verts_, 1 + std::max(v1, v2));
                if (count_verts_ > edges.size())
                    edges.resize(count_verts_);

                edges[v1].push_back({v2, w});

                count_edges_++;
            }
            
            create(edges);
            return is;
        }

        std::ostream& print(std::ostream& os) const {
            int width = 4;
            os << print_blue("graph\n");
            os << print_blue("a: "); for (auto a : a_) os << std::setw(width) << print_lcyan(a) << "\t"; os << "\n";
            os << print_blue("t: "); for (auto t : t_) os << std::setw(width) << print_lcyan(t) << "\t"; os << "\n";
            os << print_blue("n: "); for (auto n : n_) os << std::setw(width) << print_lcyan(n) << "\t"; os << "\n";
            os << print_blue("p: "); for (auto p : p_) os << std::setw(width) << print_lcyan(p) << "\t";
            return os;
        }
    };

    std::istream& operator>>(std::istream& is, graph_t& graph) {
        return graph.read(is);
    }

    std::ostream& operator<<(std::ostream& os, const graph_t& graph) {
        return graph.print(os);
    }
}