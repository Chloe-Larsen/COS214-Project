#include "GraphLayout.h"
#include <algorithm>
#include <functional>
#include <queue>
#include <set>
#include <stdexcept>

namespace tividy {
GraphLayoutResult GraphLayout::arrange(const WorkflowSnapshot &s)
{
    if (s.components.size() > 10000 || s.dependencies.size() > 50000) throw std::runtime_error("This graph exceeds the supported display limit (10,000 nodes / 50,000 edges).");
    std::map<std::string, int> indegree, level;
    std::map<std::string, const ComponentRecord *> records;
    std::map<std::string, std::vector<std::string> > outgoing;
    for (const ComponentRecord &c : s.components) {
        if (c.id.empty() || records.count(c.id)) throw std::runtime_error("Graph nodes must have unique, non-empty IDs.");
        indegree[c.id] = 0; level[c.id] = 0; records[c.id] = &c;
    }
    std::set<std::pair<std::string, std::string> > unique;
    for (const DependencyRecord &d : s.dependencies) {
        if (!records.count(d.prerequisiteId) || !records.count(d.dependentId)) throw std::runtime_error("A graph edge references an unknown component.");
        if (unique.insert(std::make_pair(d.prerequisiteId, d.dependentId)).second) {
            outgoing[d.prerequisiteId].push_back(d.dependentId); ++indegree[d.dependentId];
        }
    }
    std::priority_queue<std::string, std::vector<std::string>, std::greater<std::string> > ready;
    for (const auto &node : indegree) if (node.second == 0) ready.push(node.first);
    std::size_t visited = 0;
    while (!ready.empty()) {
        const std::string current = ready.top(); ready.pop(); ++visited;
        for (const std::string &next : outgoing[current]) {
            level[next] = std::max(level[next], level[current] + 1);
            if (--indegree[next] == 0) ready.push(next);
        }
    }
    if (visited != records.size()) throw std::runtime_error("The engine returned cyclic dependencies. A DAG cannot be arranged until the engine resolves them.");
    std::map<int, std::vector<std::string> > layers;
    for (const auto &node : level) layers[node.second].push_back(node.first);
    GraphLayoutResult result;
    for (auto &layer : layers) {
        std::sort(layer.second.begin(), layer.second.end(), [&](const std::string &a, const std::string &b) {
            const ComponentRecord &first = *records.at(a), &second = *records.at(b);
            if (first.parentId != second.parentId) return first.parentId < second.parentId;
            return first.name != second.name ? first.name < second.name : a < b;
        });
        for (std::size_t row = 0; row < layer.second.size(); ++row) {
            const double x = 32 + layer.first * 360.0, y = 32 + row * 132.0;
            result.positions[layer.second[row]] = std::make_pair(x, y);
            result.width = std::max(result.width, x + 300); result.height = std::max(result.height, y + 132);
        }
    }
    return result;
}
}
