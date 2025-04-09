#ifndef __DOM_ANALYZER_H__
#define __DOM_ANALYZER_H__

#include <vector>
#include <set>

namespace Cele
{
    namespace Algo
    {
        class DomAnalyzer
        {
          public:
            std::vector<std::vector<int>> dom_tree;
            std::vector<std::set<int>>    dom_frontier;
            std::vector<int>              imm_dom;

          public:
            DomAnalyzer();

          public:
            void solve(const std::vector<std::vector<int>>& graph, const std::vector<int>& entry_points);
            void clear();

          private:
            void removeVirtualSource(int virtual_source);
        };

    }  // namespace Algo
}  // namespace Cele

#endif  // __DOM_ANALYZER_H__
