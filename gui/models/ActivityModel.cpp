#include "ActivityModel.h"
#include <algorithm>

namespace tividy {
ActivityModel::ActivityModel(QObject *parent) : QAbstractTableModel(parent) {}
void ActivityModel::setRecords(const std::vector<ActivityRecord> &values)
{
    beginResetModel();
    records.assign(values.begin() + static_cast<std::ptrdiff_t>(values.size() > 10000 ? values.size() - 10000 : 0), values.end());
    endResetModel();
}
int ActivityModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : static_cast<int>(records.size()); }
int ActivityModel::columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : 4; }
QVariant ActivityModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(records.size())) return QVariant();
    const ActivityRecord &r = records[static_cast<std::size_t>(index.row())];
    if (role == Qt::ToolTipRole) return QString::fromStdString(r.message);
    if (role != Qt::DisplayRole) return QVariant();
    switch (index.column()) {
    case 0: return QString::fromStdString(r.timestamp);
    case 1: return QString::fromStdString(r.category);
    case 2: return QString::fromStdString(r.componentName);
    case 3: return QString::fromStdString(r.message);
    }
    return QVariant();
}
QVariant ActivityModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return QVariant();
    static const char *labels[] = {"Timestamp", "Category", "Work", "Message"};
    return section >= 0 && section < 4 ? QVariant(labels[section]) : QVariant();
}
ActivityFilterModel::ActivityFilterModel(QObject *parent) : QSortFilterProxyModel(parent) {}
void ActivityFilterModel::setCriteria(QString value, QString query) { category = value; text = query.trimmed(); invalidateFilter(); }
bool ActivityFilterModel::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    if (!category.isEmpty() && sourceModel()->index(row, 1, parent).data().toString().compare(category, Qt::CaseInsensitive) != 0) return false;
    if (text.isEmpty()) return true;
    for (int i = 0; i < 4; ++i) if (sourceModel()->index(row, i, parent).data().toString().contains(text, Qt::CaseInsensitive)) return true;
    return false;
}
}
