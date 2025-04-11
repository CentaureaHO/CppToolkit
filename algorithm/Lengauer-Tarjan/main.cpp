#include <dom_analyzer.h>

#include <vector>
#include <set>
#include <functional>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace Cele::Algo;

vector<int> countDominated(const vector<vector<int>>& dominatorTree, const vector<int>& entries)
{
    int         node_count = dominatorTree.size();
    vector<int> count(node_count, 0);

    function<int(int)> dfs_count = [&](int node) -> int {
        count[node] = 1;
        for (int child : dominatorTree[node]) count[node] += dfs_count(child);

        return count[node];
    };

    for (int entry : entries)
        if (entry < node_count) dfs_count(entry);

    return count;
}

int main()
{
    int n, m;
    cin >> n >> m;

    vector<vector<int>> graph(n + 1);
    for (int i = 0; i < m; ++i)
    {
        int u, v;
        cin >> u >> v;
        graph[u].push_back(v);
    }

    vector<int> entries = {1};

    DomAnalyzer analyzer;
    analyzer.solve(graph, entries);

    vector<int> dominated_counts = countDominated(analyzer.dom_tree, entries);

    for (int i = 1; i <= n; ++i) cout << dominated_counts[i] << ' ';
    cout << '\n';

    return 0;
}
