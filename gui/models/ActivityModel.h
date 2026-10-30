#ifndef TIVIDY_GUI_ACTIVITYMODEL_H
#define TIVIDY_GUI_ACTIVITYMODEL_H
#include "integration/WorkflowTypes.h"
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

namespace tividy {
/** Bounded read-only presentation of actual notifications, audits and output. */
class ActivityModel : public QAbstractTableModel {
public:
    /** Construct an empty activity table. */
    explicit ActivityModel(QObject *parent = nullptr);
    /** Replace the visible history with the latest 10,000 engine records. */
    void setRecords(const std::vector<ActivityRecord> &records);
    /** Qt row/column, cell and header implementations. */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /** Return timestamp, category, work and message columns. */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    /** Return plain text from the copied record. */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /** Return readable column captions. */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
private:
    std::vector<ActivityRecord> records;
};

/** Category and text filter; it does not alter retained engine history. */
class ActivityFilterModel : public QSortFilterProxyModel {
public:
    /** Construct an empty filter. */
    explicit ActivityFilterModel(QObject *parent = nullptr);
    /** Set a category (empty means all) and a case-insensitive search. */
    void setCriteria(QString category, QString text);
protected:
    /** Match both category and search text across visible fields. */
    bool filterAcceptsRow(int row, const QModelIndex &parent) const override;
private:
    QString category, text;
};
}
#endif
