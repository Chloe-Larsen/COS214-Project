#include "WorkflowTreeModel.h"
#include "theme/Theme.h"
#include <QBrush>
#include <QFont>
#include <algorithm>
#include <limits>
#include <stdexcept>

namespace tividy {
static QString q(const std::string &s) { return QString::fromStdString(s); }

WorkflowTreeModel::WorkflowTreeModel(QObject *parent) : QAbstractItemModel(parent) {}

void WorkflowTreeModel::setSnapshot(const WorkflowSnapshot &snapshot)
{
    if (snapshot.components.size() > 10000) throw std::runtime_error("A maximum of 10,000 components can be displayed.");
    QHash<QString, int> ids;
    const int count = static_cast<int>(snapshot.components.size());
    QVector<int> nextParents(count, -1), nextPositions(count), nextRoots;
    QVector<QVector<int> > nextChildren(count);
    for (int i = 0; i < count; ++i) {
        const QString id = q(snapshot.components[i].id);
        if (id.isEmpty() || ids.contains(id)) throw std::runtime_error("Engine snapshot contains an empty or duplicate component ID.");
        ids.insert(id, i);
    }
    for (int i = 0; i < count; ++i) {
        const ComponentRecord &node = snapshot.components[i];
        if (node.parentId.empty()) {
            nextPositions[i] = nextRoots.size(); nextRoots.push_back(i);
        } else {
            const int parent = ids.value(q(node.parentId), -1);
            if (parent < 0 || !snapshot.components[parent].group)
                throw std::runtime_error("Engine snapshot contains an unknown or non-group parent.");
            nextParents[i] = parent;
            nextPositions[i] = nextChildren[parent].size(); nextChildren[parent].push_back(i);
        }
    }
    QVector<int> order = nextRoots;
    QVector<int> depth(count, 0);
    for (int offset = 0; offset < order.size(); ++offset) {
        const int id = order[offset];
        if (depth[id] > 256) throw std::runtime_error("The displayed hierarchy exceeds 256 levels.");
        for (int child : nextChildren[id]) { depth[child] = depth[id] + 1; order.push_back(child); }
    }
    if (order.size() != count) throw std::runtime_error("Engine snapshot contains a hierarchy cycle.");
    struct Stats { int leaves = 0, complete = 0; int priority = std::numeric_limits<int>::min(); std::set<std::string> workers; };
    std::vector<Stats> stats(count);
    QVector<int> priorities(count);
    QVector<QString> nextSummaries(count), nextAssignees(count);
    QHash<QString, QString> names;
    for (const WorkerRecord &person : snapshot.workers) names.insert(q(person.id), q(person.name));
    for (int n = order.size() - 1; n >= 0; --n) {
        const int i = order[n];
        const ComponentRecord &node = snapshot.components[i];
        Stats &s = stats[i];
        if (!node.group) { s.leaves = 1; s.complete = node.state == "COMPLETED" ? 1 : 0; s.priority = node.priority; s.workers.insert(node.workerId); }
        else for (int child : nextChildren[i]) {
            const Stats &other = stats[child];
            s.leaves += other.leaves; s.complete += other.complete;
            s.priority = std::max(s.priority, other.priority); s.workers.insert(other.workers.begin(), other.workers.end());
        }
        priorities[i] = s.priority == std::numeric_limits<int>::min() && node.group && s.leaves == 0 ? node.priority : s.priority;
        nextSummaries[i] = node.group ? QString("%1/%2 complete").arg(s.complete).arg(s.leaves) : QString();
        if (s.workers.size() > 1) nextAssignees[i] = "Multiple";
        else if (s.workers.empty() || s.workers.begin()->empty()) nextAssignees[i] = "Unassigned";
        else nextAssignees[i] = names.value(q(*s.workers.begin()), "Unknown worker");
    }
    beginResetModel();
    current = snapshot; byId = ids; parents = nextParents; positions = nextPositions;
    roots = nextRoots; children = nextChildren; displayPriority = priorities;
    summaries = nextSummaries; assignees = nextAssignees; workerNames = names;
    endResetModel();
}

const ComponentRecord *WorkflowTreeModel::component(const QString &id) const
{
    const int i = byId.value(id, -1);
    return i < 0 ? nullptr : &current.components[static_cast<std::size_t>(i)];
}

QModelIndex WorkflowTreeModel::indexForId(const QString &id) const
{
    const int i = byId.value(id, -1);
    return i < 0 ? QModelIndex() : createIndex(positions[i], 0, static_cast<quintptr>(i + 1));
}

QModelIndex WorkflowTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || column >= 5 || (parent.isValid() && parent.column() != 0)) return QModelIndex();
    const int p = parent.isValid() ? static_cast<int>(parent.internalId()) - 1 : -1;
    const QVector<int> &list = p < 0 ? roots : children[p];
    return row >= list.size() ? QModelIndex() : createIndex(row, column, static_cast<quintptr>(list[row] + 1));
}

QModelIndex WorkflowTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) return QModelIndex();
    const int p = parents[static_cast<int>(child.internalId()) - 1];
    return p < 0 ? QModelIndex() : createIndex(positions[p], 0, static_cast<quintptr>(p + 1));
}

int WorkflowTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0) return 0;
    return parent.isValid() ? children[static_cast<int>(parent.internalId()) - 1].size() : roots.size();
}

int WorkflowTreeModel::columnCount(const QModelIndex &) const { return 5; }

QVariant WorkflowTreeModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid()) return QVariant();
    const int i = static_cast<int>(idx.internalId()) - 1;
    const ComponentRecord &node = current.components[static_cast<std::size_t>(i)];
    if (role == IdRole) return q(node.id);
    if (role == PriorityRole) return displayPriority[i];
    if (role == StateRole) return q(node.state);
    if (role == WorkerRole) return q(node.workerId);
    if (role == GroupRole) return node.group;
    if (role == Qt::DecorationRole && idx.column() == 0)
        return Theme::icon(node.group ? "group" : q(node.state), Theme::stateColour(q(node.state)));
    if (role == Qt::ToolTipRole) {
        QString result = q(node.name) + "\nID: " + q(node.id);
        if (node.group) result += "\n" + summaries[i] + "\nPriority is the highest descendant priority.";
        if (node.rejected) result += "\nRejected: " + q(node.rejectionReason);
        if (node.escalated) result += "\nEscalated";
        return result;
    }
    if (role == Qt::FontRole && node.group) { QFont font; font.setBold(true); return font; }
    if (role == Qt::DisplayRole) switch (idx.column()) {
    case 0: return q(node.name);
    case 1: {
        QString label = node.state.empty() && node.group ? summaries[i] : Theme::stateLabel(q(node.state));
        if (node.rejected) label += " / Rejected";
        if (node.escalated) label += " / Escalated";
        return label;
    }
    case 2: return assignees[i];
    case 3: return displayPriority[i];
    case 4: return q(node.deadline);
    }
    return QVariant();
}

QVariant WorkflowTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return QVariant();
    static const char *labels[] = {"Work", "State", "Assignee", "Priority", "Deadline"};
    return section >= 0 && section < 5 ? QVariant(labels[section]) : QVariant();
}

WorkflowFilterModel::WorkflowFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent), unassignedOnly(false), priorityEnabled(false), minimum(0), maximum(0)
{
    setDynamicSortFilter(true); setSortCaseSensitivity(Qt::CaseInsensitive);
}

void WorkflowFilterModel::setCriteria(QString a, QString b, QString c, bool unassigned, bool enabled, int low, int high)
{
    text = a.trimmed(); state = b; worker = c; unassignedOnly = unassigned;
    priorityEnabled = enabled; minimum = low; maximum = high; refresh();
}

void WorkflowFilterModel::refresh() { accepted.clear(); invalidateFilter(); }

bool WorkflowFilterModel::matches(const QModelIndex &idx) const
{
    if (!state.isEmpty() && idx.data(WorkflowTreeModel::StateRole).toString() != state) return false;
    const QString assigned = idx.data(WorkflowTreeModel::WorkerRole).toString();
    if (unassignedOnly && (!assigned.isEmpty() || idx.data(WorkflowTreeModel::GroupRole).toBool())) return false;
    if (!worker.isEmpty() && assigned != worker) return false;
    const int priority = idx.data(WorkflowTreeModel::PriorityRole).toInt();
    if (priorityEnabled && (priority < minimum || priority > maximum)) return false;
    return text.isEmpty() || idx.data().toString().contains(text, Qt::CaseInsensitive)
        || idx.sibling(idx.row(), 2).data().toString().contains(text, Qt::CaseInsensitive);
}

bool WorkflowFilterModel::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    const QModelIndex idx = sourceModel()->index(row, 0, parent);
    const QString id = idx.data(WorkflowTreeModel::IdRole).toString();
    if (accepted.contains(id)) return accepted.value(id);
    bool result = matches(idx);
    for (int child = 0; !result && child < sourceModel()->rowCount(idx); ++child) result = filterAcceptsRow(child, idx);
    accepted.insert(id, result); return result;
}

bool WorkflowFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (left.column() == 3 && left.data(WorkflowTreeModel::PriorityRole) != right.data(WorkflowTreeModel::PriorityRole))
        return left.data(WorkflowTreeModel::PriorityRole).toInt() < right.data(WorkflowTreeModel::PriorityRole).toInt();
    const int order = QString::localeAwareCompare(left.data().toString().toCaseFolded(), right.data().toString().toCaseFolded());
    if (order != 0) return order < 0;
    return left.data(WorkflowTreeModel::IdRole).toString() < right.data(WorkflowTreeModel::IdRole).toString();
}

} // namespace tividy
