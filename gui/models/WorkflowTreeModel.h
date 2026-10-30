#ifndef TIVIDY_GUI_WORKFLOWTREEMODEL_H
#define TIVIDY_GUI_WORKFLOWTREEMODEL_H

#include "integration/WorkflowTypes.h"
#include <QAbstractItemModel>
#include <QHash>
#include <QSortFilterProxyModel>
#include <QVector>

namespace tividy {

/** Read-only tree presentation over a copied, structurally checked snapshot. */
class WorkflowTreeModel : public QAbstractItemModel {
public:
    /** Roles used for identity, filtering and numeric sorting. */
    enum Roles { IdRole = Qt::UserRole + 1, PriorityRole, StateRole, WorkerRole, GroupRole };
    /** Construct an empty model. Qt's parent owns the model. */
    explicit WorkflowTreeModel(QObject *parent = nullptr);
    /** Replace the snapshot; throws before mutation on malformed hierarchy data. */
    void setSnapshot(const WorkflowSnapshot &snapshot);
    /** Return a component by stable identifier, or nullptr. Borrowed until reset. */
    const ComponentRecord *component(const QString &id) const;
    /** Locate a component for selection synchronisation. */
    QModelIndex indexForId(const QString &id) const;
    /** Return the children, parent, size and cell contents required by Qt. */
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    /** Return the direct parent of a displayed component. */
    QModelIndex parent(const QModelIndex &child) const override;
    /** Return the number of direct children, not descendant leaf count. */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /** Return the five tree columns. */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    /** Return presentation, identity, filtering or sorting data. */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /** Return readable column captions. */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    WorkflowSnapshot current;
    QHash<QString, int> byId;
    QHash<QString, QString> workerNames;
    QVector<QVector<int> > children;
    QVector<int> roots, parents, positions, displayPriority;
    QVector<QString> summaries, assignees;
};

/** AND-combined filters retaining ancestors of matching components. */
class WorkflowFilterModel : public QSortFilterProxyModel {
public:
    /** Construct a case-insensitive tree filter. */
    explicit WorkflowFilterModel(QObject *parent = nullptr);
    /** Set text, status, assignee and optional inclusive priority range. */
    void setCriteria(QString text, QString state, QString worker, bool unassigned,
                     bool priorityEnabled, int minimum, int maximum);
    /** Clear cached ancestor matches after a source snapshot change. */
    void refresh();
protected:
    /** Accept direct matches and the ancestors that provide their context. */
    bool filterAcceptsRow(int row, const QModelIndex &parent) const override;
    /** Compare numeric priority and provide deterministic name/ID tie breaks. */
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
private:
    bool matches(const QModelIndex &index) const;
    QString text, state, worker;
    bool unassignedOnly, priorityEnabled;
    int minimum, maximum;
    mutable QHash<QString, bool> accepted;
};

} // namespace tividy
#endif
