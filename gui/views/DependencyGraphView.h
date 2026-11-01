#ifndef TIVIDY_GUI_DEPENDENCYGRAPHVIEW_H
#define TIVIDY_GUI_DEPENDENCYGRAPHVIEW_H
#include "integration/WorkflowTypes.h"
#include "services/GraphLayout.h"
#include <QGraphicsView>
#include <QHash>
#include <QThreadPool>
#include <memory>

class QGraphicsScene;
class QTimer;
class QLabel;

namespace tividy {
/** Zoomable/pannable graph, with background layout and incremental scene creation. */
class DependencyGraphView : public QGraphicsView {
    Q_OBJECT
public:
    /** Construct an empty interactive graph. */
    explicit DependencyGraphView(QWidget *parent = nullptr);
    /** Drain layout workers before releasing scenes. */
    ~DependencyGraphView() override;
    /** Schedule a copied snapshot; stale layout results are discarded. */
    void setSnapshot(WorkflowSnapshot snapshot);
    /** Synchronise tree selection without emitting another selection request. */
    void selectComponent(const QString &id);
    /** Fit the graph within the viewport. */
    void fitGraph();
    /** Increase/decrease magnification within a bounded range. */
    void zoomIn();
    void zoomOut();
signals:
    /** A node or the dependent endpoint of an edge was selected. */
    void componentSelected(QString id);
    /** Node context menu requested at a global screen position. */
    void nodeMenuRequested(QString id, QPoint position);
    /** Edge context menu requested, with stable endpoint IDs. */
    void edgeMenuRequested(QString prerequisite, QString dependent, QPoint position);
    /** Layout could not display the engine's graph. */
    void layoutFailed(QString message);
protected:
    /** Ctrl-wheel zooms; an ordinary wheel scrolls the graph. */
    void wheelEvent(QWheelEvent *event) override;
    /** Expose contextual actions without doing domain mutations. */
    void contextMenuEvent(QContextMenuEvent *event) override;
    /** Fit the initial graph after its tab actually becomes visible. */
    void showEvent(QShowEvent *event) override;
private:
    void appendBatch();
    void selectionChanged();
    void scaleBy(qreal factor);
    QThreadPool layoutThread;
    std::unique_ptr<QGraphicsScene> activeScene, nextScene;
    WorkflowSnapshot building;
    GraphLayoutResult positions;
    QHash<QString, QGraphicsItem *> nodes, nextNodes;
    QTimer *batchTimer;
    QLabel *layoutStatus;
    std::uint64_t generation;
    std::size_t nodeOffset, edgeOffset;
    QString selectedId;
    bool synchronising, firstLayout, arranging;
};
}
#endif
