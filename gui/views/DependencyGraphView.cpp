#include "DependencyGraphView.h"
#include "theme/Theme.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QContextMenuEvent>
#include <QFontMetrics>
#include <QFutureWatcher>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QLabel>
#include <QScrollBar>
#include <QShowEvent>
#include <QTimer>
#include <QWheelEvent>
#include <cmath>

namespace tividy {
namespace {
class NodeItem : public QGraphicsRectItem {
public:
    explicit NodeItem(ComponentRecord value) : QGraphicsRectItem(0, 0, 264, 98), record(std::move(value)) {
        setData(0, "node"); setData(1, QString::fromStdString(record.id));
        setFlags(ItemIsSelectable | ItemIsFocusable); setCursor(Qt::PointingHandCursor);
        setToolTip(QString::fromStdString(record.name + "\nID: " + record.id)); setZValue(2);
    }
    void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) override {
        const QString state = QString::fromStdString(record.state);
        p->setRenderHint(QPainter::Antialiasing); p->setBrush(Theme::dustGrey());
        p->setPen(QPen(isSelected() ? Theme::cerulean() : Theme::coffeeBean(), isSelected() ? 3 : 1.2));
        p->drawRoundedRect(rect(), 8, 8);
        p->setPen(QPen(Theme::stateColour(state), 4)); p->drawLine(QPointF(8, 16), QPointF(8, 80));
        QFont font = p->font(); font.setPointSize(10); font.setBold(true); p->setFont(font); p->setPen(Theme::coffeeBean());
        p->drawText(QRectF(20, 12, 225, 24), Qt::AlignLeft | Qt::AlignVCenter,
                    QFontMetrics(font).elidedText(QString::fromStdString(record.name), Qt::ElideRight, 222));
        font.setPointSize(9); font.setBold(false); p->setFont(font);
        QString label = record.group && state.isEmpty() ? "Group" : Theme::stateLabel(state);
        if (record.rejected) label += " / Rejected";
        if (record.escalated) label += " / Escalated";
        p->drawText(QRectF(20, 39, 226, 22), Qt::AlignLeft, QFontMetrics(font).elidedText(label, Qt::ElideRight, 222));
        p->drawText(QRectF(20, 66, 225, 20), Qt::AlignLeft, QString("%1 · Priority %2").arg(record.group ? "Group" : "Item").arg(record.priority));
    }
private:
    ComponentRecord record;
};
class EdgeItem : public QGraphicsPathItem {
public:
    explicit EdgeItem(const QPainterPath &path) : QGraphicsPathItem(path) { setFlags(ItemIsSelectable); setZValue(0); }
    void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) override {
        p->setPen(QPen(isSelected() ? Theme::cerulean() : Theme::coffeeBean(), isSelected() ? 3 : 1.4)); p->setBrush(Qt::NoBrush); p->drawPath(path());
    }
};
struct LayoutOutcome { GraphLayoutResult result; QString error; };
}

DependencyGraphView::DependencyGraphView(QWidget *parent)
    : QGraphicsView(parent), activeScene(new QGraphicsScene()), batchTimer(new QTimer(this)), generation(0),
      nodeOffset(0), edgeOffset(0), synchronising(false), firstLayout(true), arranging(false)
{
    setObjectName("dependencyGraph"); setScene(activeScene.get());
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag); setTransformationAnchor(AnchorUnderMouse);
    setBackgroundBrush(Theme::dustGrey()); setMinimumSize(280, 140);
    setToolTip("Click a component to select it. Right-click for dependency actions. Drag empty space to pan; Ctrl+wheel to zoom.");
    layoutThread.setMaxThreadCount(1);
    layoutStatus = new QLabel("Updating graph…", viewport()); layoutStatus->setObjectName("graphStatus");
    layoutStatus->setAttribute(Qt::WA_TransparentForMouseEvents); layoutStatus->move(12, 12); layoutStatus->hide();
    connect(batchTimer, &QTimer::timeout, this, &DependencyGraphView::appendBatch);
}

DependencyGraphView::~DependencyGraphView() { batchTimer->stop(); layoutThread.waitForDone(); setScene(nullptr); }

void DependencyGraphView::setSnapshot(WorkflowSnapshot snapshot)
{
    ++generation; const std::uint64_t token = generation;
    arranging = true; layoutStatus->adjustSize(); layoutStatus->show(); setInteractive(false);
    batchTimer->stop(); nextScene.reset(); nextNodes.clear();
    QFutureWatcher<LayoutOutcome> *watcher = new QFutureWatcher<LayoutOutcome>(this);
    connect(watcher, &QFutureWatcher<LayoutOutcome>::finished, this, [this, watcher, token, snapshot]() {
        const LayoutOutcome outcome = watcher->result(); watcher->deleteLater();
        if (token != generation) return;
        if (!outcome.error.isEmpty()) {
            activeScene->clear(); nodes.clear();
            activeScene->addText("The dependency graph could not be displayed. Refresh after the engine resolves the reported problem.")->setDefaultTextColor(Theme::coffeeBean());
            arranging = false; layoutStatus->hide(); setInteractive(true);
            emit layoutFailed(outcome.error); return;
        }
        building = snapshot; positions = outcome.result; nodeOffset = edgeOffset = 0;
        nextScene.reset(new QGraphicsScene()); nextScene->setItemIndexMethod(QGraphicsScene::NoIndex);
        nextScene->setSceneRect(0, 0, std::max(400.0, positions.width), std::max(220.0, positions.height));
        batchTimer->start(0);
    });
    watcher->setFuture(QtConcurrent::run(&layoutThread, [snapshot]() {
        LayoutOutcome out;
        try { out.result = GraphLayout::arrange(snapshot); }
        catch (const std::exception &error) { out.error = QString::fromUtf8(error.what()); }
        catch (...) { out.error = "The dependency graph could not be arranged."; }
        return out;
    }));
}

void DependencyGraphView::appendBatch()
{
    if (!nextScene) { batchTimer->stop(); return; }
    for (int n = 0; n < 100 && nodeOffset < building.components.size(); ++n, ++nodeOffset) {
        const ComponentRecord &c = building.components[nodeOffset]; const auto point = positions.positions.find(c.id);
        if (point == positions.positions.end()) continue;
        std::unique_ptr<NodeItem> item(new NodeItem(c)); item->setPos(point->second.first, point->second.second);
        nextNodes.insert(QString::fromStdString(c.id), item.get()); nextScene->addItem(item.get()); item.release();
    }
    if (nodeOffset < building.components.size()) return;
    for (int n = 0; n < 200 && edgeOffset < building.dependencies.size(); ++n, ++edgeOffset) {
        const DependencyRecord &d = building.dependencies[edgeOffset];
        const auto a = positions.positions.find(d.prerequisiteId), b = positions.positions.find(d.dependentId);
        if (a == positions.positions.end() || b == positions.positions.end()) continue;
        const QPointF start(a->second.first + 264, a->second.second + 49), end(b->second.first, b->second.second + 49);
        const qreal bend = std::max(36.0, (end.x() - start.x()) * .5);
        QPainterPath path(start); path.cubicTo(start + QPointF(bend, 0), end - QPointF(bend, 0), end);
        std::unique_ptr<EdgeItem> edge(new EdgeItem(path)); edge->setData(0, "edge");
        edge->setData(1, QString::fromStdString(d.prerequisiteId)); edge->setData(2, QString::fromStdString(d.dependentId));
        edge->setToolTip("Prerequisite → dependent. Right-click to remove.");
        QPolygonF arrow; arrow << end << end + QPointF(-9, -5) << end + QPointF(-9, 5);
        QGraphicsPolygonItem *head = new QGraphicsPolygonItem(arrow, edge.get()); head->setBrush(Theme::coffeeBean()); head->setPen(Qt::NoPen);
        nextScene->addItem(edge.get()); edge.release();
    }
    if (edgeOffset < building.dependencies.size()) return;
    batchTimer->stop();
    if (building.components.empty()) {
        QGraphicsTextItem *text = nextScene->addText("Your dependency graph will appear here.");
        text->setDefaultTextColor(Theme::coffeeBean()); text->setPos(32, 64);
    }
    setScene(nextScene.get()); activeScene = std::move(nextScene); nodes = nextNodes; nextNodes.clear();
    arranging = false; layoutStatus->hide(); setInteractive(true);
    connect(activeScene.get(), &QGraphicsScene::selectionChanged, this, &DependencyGraphView::selectionChanged);
    selectComponent(selectedId);
    if (firstLayout && isVisible() && !building.components.empty()) { fitGraph(); firstLayout = false; }
}

void DependencyGraphView::selectionChanged()
{
    if (synchronising || arranging) return;
    const QList<QGraphicsItem *> selected = activeScene->selectedItems();
    if (selected.isEmpty()) return;
    QGraphicsItem *item = selected.front();
    const QString id = item->data(0).toString() == "edge" ? item->data(2).toString() : item->data(1).toString();
    selectedId = id; emit componentSelected(id);
}

void DependencyGraphView::selectComponent(const QString &id)
{
    const bool changed = selectedId != id;
    selectedId = id; synchronising = true;
    activeScene->clearSelection();
    if (nodes.contains(id)) { nodes.value(id)->setSelected(true); if (changed) ensureVisible(nodes.value(id), 20, 20); }
    synchronising = false;
}

void DependencyGraphView::fitGraph() { if (scene()) fitInView(scene()->itemsBoundingRect().adjusted(-24, -24, 24, 24), Qt::KeepAspectRatio); }
void DependencyGraphView::showEvent(QShowEvent *event)
{
    QGraphicsView::showEvent(event);
    QTimer::singleShot(0, this, [this]() { if (firstLayout && !nodes.isEmpty()) { fitGraph(); firstLayout = false; } });
}
void DependencyGraphView::scaleBy(qreal factor) {
    const qreal current = transform().m11();
    if (current <= 0) return;
    const qreal target = qBound(qreal(.00001), current * factor, qreal(3));
    scale(target / current, target / current);
}
void DependencyGraphView::zoomIn() { scaleBy(1.2); }
void DependencyGraphView::zoomOut() { scaleBy(1 / 1.2); }
void DependencyGraphView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) { scaleBy(event->angleDelta().y() > 0 ? 1.15 : 1 / 1.15); event->accept(); }
    else QGraphicsView::wheelEvent(event);
}
void DependencyGraphView::contextMenuEvent(QContextMenuEvent *event)
{
    if (arranging) return;
    QGraphicsItem *item = itemAt(event->pos());
    while (item && item->data(0).toString().isEmpty()) item = item->parentItem();
    if (!item) { emit nodeMenuRequested(QString(), event->globalPos()); return; }
    if (item->data(0).toString() == "edge") emit edgeMenuRequested(item->data(1).toString(), item->data(2).toString(), event->globalPos());
    else emit nodeMenuRequested(item->data(1).toString(), event->globalPos());
}
}
