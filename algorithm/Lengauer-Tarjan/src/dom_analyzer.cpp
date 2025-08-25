#include <dom_analyzer.h>

#include <functional>
#include <algorithm>

using namespace std;
using namespace Cele::Algo;

DomAnalyzer::DomAnalyzer() : frontier_generated(false) {}

void DomAnalyzer::solve(
    const vector<vector<int>>& graph, const vector<int>& entry_points, bool reverse, bool gen_frontier)
{
    frontier_generated = gen_frontier;

    int node_count = graph.size();

    int                 virtual_source = node_count;
    vector<vector<int>> working_graph;

    if (!reverse)
    {
        working_graph = graph;
        working_graph.push_back(vector<int>());
        for (int entry : entry_points) working_graph[virtual_source].push_back(entry);
    }
    else
    {
        working_graph.resize(node_count + 1);
        for (int u = 0; u < node_count; ++u)
            for (int v : graph[u]) working_graph[v].push_back(u);

        working_graph.push_back(vector<int>());
        for (int exit : entry_points) working_graph[virtual_source].push_back(exit);
    }

    build(working_graph, node_count + 1, virtual_source, entry_points);
}

void DomAnalyzer::build(
    const vector<vector<int>>& working_graph, int node_count, int virtual_source, const std::vector<int>& entry_points)
{
    vector<vector<int>> backward_edges(node_count);
    for (int u = 0; u < node_count; ++u)
        for (int v : working_graph[u]) backward_edges[v].push_back(u);

    dom_tree.clear();
    dom_tree.resize(node_count);
    dom_frontier.clear();
    dom_frontier.resize(node_count);
    imm_dom.clear();
    imm_dom.resize(node_count);

    int                 dfs_count = -1;
    vector<int>         block_to_dfs(node_count, 0), dfs_to_block(node_count), parent(node_count, 0);
    vector<int>         semi_dom(node_count);
    vector<int>         dsu_parent(node_count), min_ancestor(node_count);
    vector<vector<int>> semi_children(node_count);

    for (int i = 0; i < node_count; ++i)
    {
        dsu_parent[i]   = i;
        min_ancestor[i] = i;
        semi_dom[i]     = i;
    }

    function<void(int)> dfs = [&](int block) {
        block_to_dfs[block]     = ++dfs_count;
        dfs_to_block[dfs_count] = block;
        semi_dom[block]         = block_to_dfs[block];
        for (int next : working_graph[block])
            if (!block_to_dfs[next])
            {
                dfs(next);
                parent[next] = block;
            }
    };
    dfs(virtual_source);

    auto dsu_find = [&](int u, const auto& self) -> int {
        if (dsu_parent[u] == u) return u;
        int root = self(dsu_parent[u], self);
        if (semi_dom[min_ancestor[dsu_parent[u]]] < semi_dom[min_ancestor[u]])
            min_ancestor[u] = min_ancestor[dsu_parent[u]];
        return dsu_parent[u] = root;
    };

    auto dsu_query = [&](int u) -> int {
        dsu_find(u, dsu_find);
        return min_ancestor[u];
    };

    for (int dfs_id = dfs_count; dfs_id >= 1; --dfs_id)
    {
        int curr = dfs_to_block[dfs_id];
        for (int pred : backward_edges[curr])
            if (block_to_dfs[pred]) semi_dom[curr] = min(semi_dom[curr], semi_dom[dsu_query(pred)]);

        semi_children[dfs_to_block[semi_dom[curr]]].push_back(curr);
        dsu_parent[curr] = parent[curr];

        for (int child : semi_children[parent[curr]])
        {
            int best_vertex = dsu_query(child);
            imm_dom[child]  = (semi_dom[best_vertex] == semi_dom[child]) ? parent[curr] : best_vertex;
        }
        semi_children[parent[curr]].clear();
    }

    for (int dfs_id = 1; dfs_id <= dfs_count; ++dfs_id)
    {
        int curr = dfs_to_block[dfs_id];
        if (imm_dom[curr] != dfs_to_block[semi_dom[curr]]) imm_dom[curr] = imm_dom[imm_dom[curr]];
    }

    for (int i = 0; i < node_count; ++i)
        if (block_to_dfs[i]) dom_tree[imm_dom[i]].push_back(i);

    removeVirtualSource(virtual_source);
    --node_count;
    for (int entry : entry_points) imm_dom[entry] = -1;

    if (frontier_generated)
    {
        for (int block = 0; block < node_count; ++block)
        {
            for (int succ : working_graph[block])
            {
                int runner = block;
                while (runner != imm_dom[succ] && runner != virtual_source)
                {
                    dom_frontier[runner].insert(succ);
                    // if (imm_dom[runner] == runner) assert(false);

                    runner = imm_dom[runner];
                    if (runner == -1) break;
                }
            }
        }
    }
}

void DomAnalyzer::removeVirtualSource(int virtual_source)
{
    dom_tree.resize(virtual_source);
    dom_frontier.resize(virtual_source);
    imm_dom.resize(virtual_source);
}

void DomAnalyzer::clear()
{
    dom_tree.clear();
    dom_frontier.clear();
    imm_dom.clear();
}
