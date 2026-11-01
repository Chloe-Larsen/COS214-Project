#ifndef TIVIDY_GUI_GRAPHLAYOUT_H
#define TIVIDY_GUI_GRAPHLAYOUT_H
#include "integration/WorkflowTypes.h"
#include <map>
#include <utility>

namespace tividy {
/** Value-only result suitable for transfer between layout and GUI threads. */
struct GraphLayoutResult {
    std::map<std::string, std::pair<double, double> > positions;
    double width = 0, height = 0;
};
/** Automatic left-to-right DAG layout. It never modifies dependency semantics. */
class GraphLayout {
public:
    /** Arrange all components, rejecting malformed/unrenderable snapshots. */
    static GraphLayoutResult arrange(const WorkflowSnapshot &snapshot);
};
}
#endif
