#ifndef TIVIDY_GUI_MAINWINDOW_H
#define TIVIDY_GUI_MAINWINDOW_H
#include "integration/IWorkflowGateway.h"
#include <QMainWindow>
#include <QMap>
#include <functional>
#include <memory>

class QAction; class QCheckBox; class QCloseEvent; class QComboBox; class QDockWidget;
class QLabel; class QLineEdit; class QProgressBar; class QSpinBox; class QTableView;
class QTableWidget; class QTabWidget; class QTreeView;
class QResizeEvent;

namespace tividy {
class WorkflowController; class WorkflowTreeModel; class WorkflowFilterModel;
class ActivityModel; class ActivityFilterModel; class DependencyGraphView;

/** Complete desktop presentation; all mutations go through WorkflowController. */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    /** Construct the workspace for a real engine gateway. */
    explicit MainWindow(std::shared_ptr<IWorkflowGateway> gateway, QWidget *parent = nullptr);
protected:
    /** Protect unsaved work and in-flight transactions on close. */
    void closeEvent(QCloseEvent *event) override;
    /** Compact the header on shorter displays without compressing controls. */
    void resizeEvent(QResizeEvent *event) override;
private:
    void createActions();
    void createWorkspace();
    void createActivityPanel();
    void refreshView();
    void updateActions();
    void updateInspector();
    void updateFilters();
    void clearFilters();
    void perform(Operation operation);
    void selectById(const QString &id, bool fromGraph = false);
    void contextMenu(const QString &id, const QPoint &position);
    void openWorkflow();
    void saveWorkflow(bool choosePath = false);
    void protectUnsaved(std::function<void()> action);
    void showActivityDetails(const QModelIndex &index);
    void exportActivity();
    bool supported(Operation operation) const;
    QString selectedWorker() const;
    QAction *action(Operation operation) const;

    WorkflowController *controller;
    WorkflowTreeModel *treeModel;
    WorkflowFilterModel *filterModel;
    ActivityModel *activityModel;
    ActivityFilterModel *activityFilter;
    QMap<Operation, QAction *> actions;
    QAction *openAction, *saveAction, *saveAsAction, *refreshAction;
    QTreeView *tree;
    DependencyGraphView *graph;
    QTableWidget *workers;
    QTableView *activity;
    QTabWidget *pages, *detailTabs;
    QDockWidget *activityDock = nullptr;
    QLineEdit *search, *activitySearch;
    QComboBox *stateFilter, *workerFilter, *activityCategory;
    QCheckBox *priorityEnabled;
    QSpinBox *priorityMin, *priorityMax;
    QLabel *title, *counts, *availability, *emptyTree, *inspectorTitle, *inspectorText, *strategyLabel;
    QProgressBar *progress;
    QString selection;
    bool refreshing, modelFault;
    std::function<void()> afterSave;
};
}
#endif
