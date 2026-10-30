#include "MainWindow.h"
#include "controllers/WorkflowController.h"
#include "dialogs/Dialogs.h"
#include "models/WorkflowTreeModel.h"
#include "models/ActivityModel.h"
#include "views/DependencyGraphView.h"
#include "theme/Theme.h"
#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QResizeEvent>
#include <QSettings>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QSplitter>
#include <QStatusBar>
#include <QStyle>
#include <QTableView>
#include <QTableWidget>
#include <QTabWidget>
#include <QTimer>
#include <QToolBar>
#include <QTreeView>
#include <QVBoxLayout>
#include <climits>
#include <algorithm>
#include <set>

namespace tividy {
namespace {
QString q(const std::string &s) { return QString::fromStdString(s); }
QPushButton *button(QAction *action, QWidget *parent) {
    QPushButton *result = new QPushButton(action->text(), parent); result->setToolTip(action->toolTip());
    result->setEnabled(action->isEnabled());
    QObject::connect(result, &QPushButton::clicked, action, &QAction::trigger);
    QObject::connect(action, &QAction::changed, result, [result, action]() { result->setEnabled(action->isEnabled()); result->setText(action->text()); result->setToolTip(action->toolTip()); });
    return result;
}
QLabel *label(QString text, const char *objectName, QWidget *parent) {
    QLabel *result = new QLabel(text, parent); result->setObjectName(objectName); result->setTextFormat(Qt::PlainText); return result;
}
const std::vector<Operation> nodeOperations = {Operation::Progress, Operation::Review, Operation::Reject, Operation::Escalate,
    Operation::Cancel, Operation::Assign, Operation::Unassign, Operation::RenameComponent, Operation::SetPriority};
}

MainWindow::MainWindow(std::shared_ptr<IWorkflowGateway> gateway, QWidget *parent)
    : QMainWindow(parent), controller(new WorkflowController(std::move(gateway), this)),
      treeModel(new WorkflowTreeModel(this)), filterModel(new WorkflowFilterModel(this)),
      activityModel(new ActivityModel(this)), activityFilter(new ActivityFilterModel(this)), refreshing(false), modelFault(false)
{
    setObjectName("mainWindow"); resize(1400, 900); setMinimumSize(920, 660);
    setWindowTitle("TIVIDY | Workflow workspace"); setDockNestingEnabled(true);
    createActions(); createWorkspace(); createActivityPanel();
    connect(controller, &WorkflowController::snapshotChanged, this, &MainWindow::refreshView);
    connect(controller, &WorkflowController::busyChanged, this, [this](bool busy) {
        progress->setVisible(busy); statusBar()->showMessage(busy ? "Working…" : "Ready", busy ? 0 : 2000); updateActions();
    });
    connect(controller, &WorkflowController::failure, this, [this](const QString &message) { afterSave = {}; Dialogs::error(this, message); });
    connect(controller, &WorkflowController::notice, this, [this](const QString &message) { statusBar()->showMessage(message, 7000); });
    connect(controller, &WorkflowController::selectRequested, this, [this](const QString &id) { selectById(id); });
    connect(controller, &WorkflowController::saved, this, [this]() { std::function<void()> next = std::move(afterSave); afterSave = {}; if (next) next(); });
    QSettings settings; restoreGeometry(settings.value("workspace/geometry").toByteArray()); restoreState(settings.value("workspace/layout").toByteArray());
    updateActions(); QTimer::singleShot(0, controller, &WorkflowController::refresh);
}

QAction *MainWindow::action(Operation op) const { return actions.value(op); }

void MainWindow::createActions()
{
    const std::vector<Operation> all = {Operation::CreateWorkflow,Operation::AddItem,Operation::AddGroup,Operation::RenameComponent,
        Operation::SetPriority,Operation::Assign,Operation::Unassign,Operation::Progress,Operation::Review,Operation::Reject,
        Operation::Escalate,Operation::Cancel,Operation::AddWorker,Operation::EditWorker,Operation::RemoveWorker,Operation::SetSupervisor,
        Operation::SetStrategy,Operation::AddDependency,Operation::RemoveDependency,Operation::ConfigureNotifier,Operation::ConfigureAudit};
    for (Operation op : all) {
        QAction *a = new QAction(q(operationName(op)), this);
        a->setObjectName("op_" + QString::number(static_cast<int>(op))); a->setToolTip(a->text()); actions.insert(op, a);
        connect(a, &QAction::triggered, this, [this, op]() { perform(op); });
    }
    action(Operation::CreateWorkflow)->setShortcut(QKeySequence::New);
    action(Operation::AddItem)->setShortcut(QKeySequence(Qt::Key_Insert));
    action(Operation::AddGroup)->setShortcut(QKeySequence("Ctrl+Shift+G"));
    action(Operation::RenameComponent)->setShortcut(QKeySequence(Qt::Key_F2));
    action(Operation::Assign)->setShortcut(QKeySequence("Ctrl+Shift+A"));
    action(Operation::Unassign)->setShortcut(QKeySequence("Ctrl+Shift+U"));
    action(Operation::Progress)->setShortcut(QKeySequence("Ctrl+Return"));
    action(Operation::Review)->setShortcut(QKeySequence("Ctrl+R"));
    action(Operation::Reject)->setShortcut(QKeySequence("Ctrl+Shift+R"));
    action(Operation::Escalate)->setShortcut(QKeySequence("Ctrl+E"));
    action(Operation::Cancel)->setShortcut(QKeySequence(Qt::Key_Delete));
    action(Operation::AddDependency)->setShortcut(QKeySequence("Ctrl+D"));
    action(Operation::RemoveDependency)->setShortcut(QKeySequence("Ctrl+Shift+D"));
    action(Operation::AddWorker)->setShortcut(QKeySequence("Ctrl+Shift+W"));
    openAction = new QAction("Open JSON…", this); openAction->setShortcut(QKeySequence::Open); openAction->setObjectName("openWorkflow");
    saveAction = new QAction("Save", this); saveAction->setShortcut(QKeySequence::Save); saveAction->setObjectName("saveWorkflow");
    saveAsAction = new QAction("Save as…", this); saveAsAction->setShortcut(QKeySequence::SaveAs);
    refreshAction = new QAction("Refresh", this); refreshAction->setShortcut(QKeySequence(Qt::Key_F5));
    connect(openAction, &QAction::triggered, this, &MainWindow::openWorkflow);
    connect(saveAction, &QAction::triggered, this, [this]() { saveWorkflow(); });
    connect(saveAsAction, &QAction::triggered, this, [this]() { saveWorkflow(true); });
    connect(refreshAction, &QAction::triggered, controller, &WorkflowController::refresh);
    QMenu *file = menuBar()->addMenu("&File"); file->addAction(action(Operation::CreateWorkflow)); file->addAction(openAction); file->addSeparator();
    file->addAction(saveAction); file->addAction(saveAsAction); file->addSeparator();
    QAction *quit = file->addAction("Exit"); quit->setShortcut(QKeySequence::Quit); connect(quit, &QAction::triggered, this, &MainWindow::close);
    QMenu *work = menuBar()->addMenu("&Work"); work->addAction(action(Operation::AddItem)); work->addAction(action(Operation::AddGroup)); work->addSeparator();
    for (Operation op : nodeOperations) work->addAction(action(op));
    work->addSeparator(); work->addAction(action(Operation::AddDependency)); work->addAction(action(Operation::RemoveDependency));
    work->addSeparator(); work->addAction(action(Operation::ConfigureNotifier)); work->addAction(action(Operation::ConfigureAudit));
    QMenu *people = menuBar()->addMenu("&Workforce");
    for (Operation op : {Operation::AddWorker,Operation::EditWorker,Operation::RemoveWorker,Operation::SetSupervisor,Operation::SetStrategy}) people->addAction(action(op));
    QMenu *view = menuBar()->addMenu("&View"); view->setObjectName("viewMenu"); view->addAction(refreshAction);
    QAction *focus = new QAction("Search work", this); focus->setShortcut(QKeySequence::Find); addAction(focus);
    connect(focus, &QAction::triggered, this, [this]() { pages->setCurrentIndex(0); search->setFocus(); search->selectAll(); });
}

void MainWindow::createWorkspace()
{
    QToolBar *toolbar = addToolBar("Workspace"); toolbar->setObjectName("workspaceToolbar"); toolbar->setMovable(false);
    toolbar->addWidget(label("TIVIDY", "brand", toolbar)); toolbar->addSeparator();
    toolbar->addAction(action(Operation::CreateWorkflow)); toolbar->addAction(openAction); toolbar->addAction(saveAction);
    QWidget *spacer = new QWidget(toolbar); spacer->setObjectName("toolbarSpacer"); spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); toolbar->addWidget(spacer);
    toolbar->addWidget(label("CONNECTED WORK. CLEAR PROGRESS.", "tagline", toolbar));
    QWidget *body = new QWidget(this); body->setObjectName("workspaceBody"); QVBoxLayout *layout = new QVBoxLayout(body); layout->setContentsMargins(24, 20, 24, 12); layout->setSpacing(12);
    layout->addWidget(label("WORKFLOW MANAGEMENT", "eyebrow", body));
    title = label("Your workflow workspace", "pageTitle", body); layout->addWidget(title);
    counts = label("Bring your work, people and dependencies together.", "counts", body); layout->addWidget(counts);
    availability = label(QString(), "availability", body); availability->setWordWrap(true); availability->hide(); layout->addWidget(availability);
    pages = new QTabWidget(body); pages->setObjectName("workspaceTabs"); layout->addWidget(pages, 1);
    QWidget *workflow = new QWidget(pages); QVBoxLayout *workLayout = new QVBoxLayout(workflow); workLayout->setContentsMargins(0, 8, 0, 0);
    QHBoxLayout *filters = new QHBoxLayout();
    search = new QLineEdit(workflow); search->setObjectName("workSearch"); search->setPlaceholderText("Search work or assignee…"); search->setClearButtonEnabled(true); search->setMinimumWidth(180);
    stateFilter = new QComboBox(workflow); stateFilter->setObjectName("stateFilter"); stateFilter->addItem("All states", QString());
    workerFilter = new QComboBox(workflow); workerFilter->setObjectName("workerFilter"); workerFilter->addItem("All assignees", QString()); workerFilter->addItem("Unassigned", QString());
    stateFilter->setToolTip("Filter by lifecycle state"); workerFilter->setToolTip("Filter by a single assignee, or unassigned work");
    filters->addWidget(search, 2); filters->addWidget(stateFilter, 1); filters->addWidget(workerFilter, 1);
    QPushButton *clear = new QPushButton("Clear", workflow); clear->setObjectName("clearFilters"); filters->addWidget(clear); workLayout->addLayout(filters);
    QHBoxLayout *priority = new QHBoxLayout(); priorityEnabled = new QCheckBox("Priority range", workflow);
    priorityMin = new QSpinBox(workflow); priorityMax = new QSpinBox(workflow); priorityMin->setRange(INT_MIN, INT_MAX); priorityMax->setRange(INT_MIN, INT_MAX);
    priorityMin->setValue(0); priorityMax->setValue(100); priorityMin->setEnabled(false); priorityMax->setEnabled(false);
    priorityMin->setToolTip("Minimum priority (inclusive)"); priorityMax->setToolTip("Maximum priority (inclusive)");
    priority->addWidget(priorityEnabled); priority->addWidget(priorityMin); priority->addWidget(label("to", "rangeLabel", workflow)); priority->addWidget(priorityMax); priority->addStretch();
    priority->addWidget(button(action(Operation::AddItem), workflow)); priority->addWidget(button(action(Operation::AddGroup), workflow)); workLayout->addLayout(priority);
    QSplitter *splitter = new QSplitter(workflow); splitter->setObjectName("workSplitter"); splitter->setChildrenCollapsible(false);
    QWidget *treePane = new QWidget(splitter); QVBoxLayout *treeLayout = new QVBoxLayout(treePane); treeLayout->setContentsMargins(0, 0, 4, 0);
    QHBoxLayout *treeTools = new QHBoxLayout(); treeTools->addWidget(label("WORK HIERARCHY", "eyebrow", treePane)); treeTools->addStretch();
    QPushButton *expand = new QPushButton("Expand", treePane), *collapse = new QPushButton("Collapse", treePane); treeTools->addWidget(expand); treeTools->addWidget(collapse); treeLayout->addLayout(treeTools);
    filterModel->setSourceModel(treeModel); tree = new QTreeView(treePane); tree->setObjectName("workflowTree"); tree->setModel(filterModel);
    tree->setAlternatingRowColors(false); tree->setUniformRowHeights(true); tree->setSortingEnabled(true); tree->sortByColumn(3, Qt::DescendingOrder);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows); tree->setSelectionMode(QAbstractItemView::SingleSelection); tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree->setContextMenuPolicy(Qt::CustomContextMenu); tree->setAllColumnsShowFocus(true); tree->setColumnWidth(0, 230); tree->setColumnWidth(1, 150); tree->setColumnWidth(2, 130); tree->setColumnWidth(3, 70);
    tree->header()->setStretchLastSection(false); tree->header()->setSectionResizeMode(QHeaderView::Interactive);
    treeLayout->addWidget(tree, 1);
    emptyTree = label("No work to display. Create or open a workflow to begin.", "emptyTree", treePane); emptyTree->setWordWrap(true); treeLayout->addWidget(emptyTree);
    detailTabs = new QTabWidget(splitter); detailTabs->setObjectName("detailTabs");
    QWidget *inspect = new QWidget(detailTabs); QVBoxLayout *inspectLayout = new QVBoxLayout(inspect); inspectLayout->setContentsMargins(16, 16, 12, 12);
    inspectorTitle = label("Select a work item", "sectionTitle", inspect); inspectorTitle->setWordWrap(true); inspectLayout->addWidget(inspectorTitle);
    inspectorText = label("Details and available actions will appear here.", "inspectorText", inspect); inspectorText->setWordWrap(true); inspectorText->setTextInteractionFlags(Qt::TextSelectableByMouse); inspectLayout->addWidget(inspectorText);
    QGridLayout *commands = new QGridLayout(); int i = 0;
    for (Operation op : {Operation::Progress,Operation::Review,Operation::Assign,Operation::Unassign,Operation::Escalate,Operation::Reject,Operation::SetPriority,Operation::Cancel}) {
        QPushButton *b = button(action(op), inspect); if (op == Operation::Cancel || op == Operation::Reject) b->setObjectName("danger"); commands->addWidget(b, i / 2, i % 2); ++i;
    }
    inspectLayout->addLayout(commands);
    inspectLayout->addWidget(button(action(Operation::ConfigureNotifier), inspect)); inspectLayout->addWidget(button(action(Operation::ConfigureAudit), inspect)); inspectLayout->addStretch();
    QScrollArea *inspectorScroll = new QScrollArea(detailTabs); inspectorScroll->setWidgetResizable(true); inspectorScroll->setFrameShape(QFrame::NoFrame); inspectorScroll->setWidget(inspect);
    detailTabs->addTab(inspectorScroll, "Inspector");
    QWidget *graphPane = new QWidget(detailTabs); QVBoxLayout *graphLayout = new QVBoxLayout(graphPane); graphLayout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *graphTools = new QHBoxLayout();
    QPushButton *fit = new QPushButton("Fit", graphPane), *plus = new QPushButton("+", graphPane), *minus = new QPushButton("−", graphPane);
    fit->setToolTip("Fit the complete dependency graph"); plus->setToolTip("Zoom in"); minus->setToolTip("Zoom out");
    graphTools->addWidget(fit); graphTools->addWidget(minus); graphTools->addWidget(plus); graphTools->addStretch(); graphTools->addWidget(button(action(Operation::AddDependency), graphPane)); graphLayout->addLayout(graphTools);
    graph = new DependencyGraphView(graphPane); graphLayout->addWidget(graph, 1); detailTabs->addTab(graphPane, "Dependencies");
    splitter->addWidget(treePane); splitter->addWidget(detailTabs); splitter->setStretchFactor(0, 6); splitter->setStretchFactor(1, 5); workLayout->addWidget(splitter, 1);
    pages->addTab(workflow, "Workflow");
    QWidget *workforce = new QWidget(pages); QVBoxLayout *peopleLayout = new QVBoxLayout(workforce); peopleLayout->setContentsMargins(0, 12, 0, 0);
    QHBoxLayout *peopleTools = new QHBoxLayout(); peopleTools->addWidget(label("Your people", "sectionTitle", workforce)); peopleTools->addStretch();
    for (Operation op : {Operation::AddWorker,Operation::EditWorker,Operation::SetSupervisor,Operation::RemoveWorker}) peopleTools->addWidget(button(action(op), workforce));
    peopleLayout->addLayout(peopleTools);
    workers = new QTableWidget(0, 5, workforce); workers->setObjectName("workerTable"); workers->setHorizontalHeaderLabels({"Name", "Role", "Email", "Superior", "Workload"});
    workers->setSelectionBehavior(QAbstractItemView::SelectRows); workers->setSelectionMode(QAbstractItemView::SingleSelection); workers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    workers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); workers->verticalHeader()->hide(); workers->setSortingEnabled(true); peopleLayout->addWidget(workers, 1);
    QHBoxLayout *strategy = new QHBoxLayout(); strategyLabel = label("Assignment strategy: not selected", "strategyLabel", workforce);
    strategy->addWidget(strategyLabel, 1); strategy->addWidget(button(action(Operation::SetStrategy), workforce)); peopleLayout->addLayout(strategy); pages->addTab(workforce, "Workforce");
    setCentralWidget(body);
    progress = new QProgressBar(this); progress->setRange(0, 0); progress->setMaximumWidth(140); progress->setTextVisible(false); progress->hide(); statusBar()->addPermanentWidget(progress);
    QTimer *debounce = new QTimer(this); debounce->setSingleShot(true); debounce->setInterval(120);
    connect(search, &QLineEdit::textChanged, this, [debounce]() { debounce->start(); }); connect(debounce, &QTimer::timeout, this, &MainWindow::updateFilters);
    connect(stateFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() { updateFilters(); });
    connect(workerFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() { updateFilters(); });
    connect(priorityEnabled, &QCheckBox::toggled, this, [this](bool checked) { priorityMin->setEnabled(checked); priorityMax->setEnabled(checked); updateFilters(); });
    connect(priorityMin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) { if (value > priorityMax->value()) { QSignalBlocker block(priorityMax); priorityMax->setValue(value); } updateFilters(); });
    connect(priorityMax, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) { if (value < priorityMin->value()) { QSignalBlocker block(priorityMin); priorityMin->setValue(value); } updateFilters(); });
    connect(clear, &QPushButton::clicked, this, &MainWindow::clearFilters);
    connect(expand, &QPushButton::clicked, tree, &QTreeView::expandAll); connect(collapse, &QPushButton::clicked, tree, &QTreeView::collapseAll);
    connect(tree->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex &index) {
        if (refreshing) return;
        selection = index.data(WorkflowTreeModel::IdRole).toString(); graph->selectComponent(selection); updateInspector(); updateActions();
    });
    connect(tree, &QTreeView::customContextMenuRequested, this, [this](QPoint point) { const QModelIndex idx = tree->indexAt(point); contextMenu(idx.data(WorkflowTreeModel::IdRole).toString(), tree->viewport()->mapToGlobal(point)); });
    connect(graph, &DependencyGraphView::componentSelected, this, [this](QString id) { selectById(id, true); });
    connect(graph, &DependencyGraphView::nodeMenuRequested, this, &MainWindow::contextMenu);
    connect(graph, &DependencyGraphView::edgeMenuRequested, this, [this](QString prerequisite, QString dependent, QPoint point) {
        QMenu menu(this); QAction *remove = menu.addAction("Remove this dependency"); remove->setEnabled(supported(Operation::RemoveDependency));
        if (menu.exec(point) == remove && Dialogs::confirm(this, "Remove dependency", "Remove the selected prerequisite relationship?")) {
            OperationRequest request; request.operation = Operation::RemoveDependency; request.targetId = dependent.toStdString(); request.relatedId = prerequisite.toStdString(); controller->execute(request);
        }
    });
    connect(graph, &DependencyGraphView::layoutFailed, this, [this](QString message) { statusBar()->showMessage(message, 15000); });
    connect(fit, &QPushButton::clicked, graph, &DependencyGraphView::fitGraph); connect(plus, &QPushButton::clicked, graph, &DependencyGraphView::zoomIn); connect(minus, &QPushButton::clicked, graph, &DependencyGraphView::zoomOut);
    connect(workers, &QTableWidget::itemSelectionChanged, this, &MainWindow::updateActions);
    connect(pages, &QTabWidget::currentChanged, this, [this]() { updateActions(); });
    connect(workers, &QTableWidget::cellDoubleClicked, this, [this]() { if (supported(Operation::EditWorker)) perform(Operation::EditWorker); });
    QWidget::setTabOrder(search, stateFilter); QWidget::setTabOrder(stateFilter, workerFilter); QWidget::setTabOrder(workerFilter, tree);
}

void MainWindow::createActivityPanel()
{
    activityDock = new QDockWidget("Activity · notifications, audit & adapter output", this); activityDock->setObjectName("activityDock");
    QWidget *body = new QWidget(activityDock); QVBoxLayout *layout = new QVBoxLayout(body); layout->setContentsMargins(12, 4, 12, 8);
    QHBoxLayout *tools = new QHBoxLayout(); activityCategory = new QComboBox(body); activityCategory->addItems({"All categories", "Notification", "Audit", "Adapter"});
    activitySearch = new QLineEdit(body); activitySearch->setPlaceholderText("Search activity…"); activitySearch->setClearButtonEnabled(true);
    QPushButton *exportButton = new QPushButton("Export visible CSV…", body); tools->addWidget(activityCategory); tools->addWidget(activitySearch, 1); tools->addWidget(exportButton); layout->addLayout(tools);
    activityFilter->setSourceModel(activityModel); activity = new QTableView(body); activity->setObjectName("activityTable"); activity->setModel(activityFilter);
    activity->setSelectionBehavior(QAbstractItemView::SelectRows); activity->setSelectionMode(QAbstractItemView::SingleSelection); activity->setWordWrap(false); activity->verticalHeader()->hide();
    activity->setColumnWidth(0, 170); activity->setColumnWidth(1, 110); activity->setColumnWidth(2, 180); activity->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(activity); activityDock->setWidget(body); activityDock->setMinimumHeight(160); addDockWidget(Qt::BottomDockWidgetArea, activityDock); resizeDocks({activityDock}, {200}, Qt::Vertical);
    findChild<QMenu *>("viewMenu")->addAction(activityDock->toggleViewAction());
    const auto filter = [this]() { activityFilter->setCriteria(activityCategory->currentIndex() == 0 ? QString() : activityCategory->currentText(), activitySearch->text()); };
    connect(activityCategory, QOverload<int>::of(&QComboBox::currentIndexChanged), this, filter); connect(activitySearch, &QLineEdit::textChanged, this, filter);
    connect(activity, &QTableView::doubleClicked, this, &MainWindow::showActivityDetails); connect(exportButton, &QPushButton::clicked, this, &MainWindow::exportActivity);
}

QString MainWindow::selectedWorker() const
{
    const int row = workers->currentRow(); return row < 0 || !workers->item(row, 0) ? QString() : workers->item(row, 0)->data(Qt::UserRole).toString();
}

bool MainWindow::supported(Operation op) const
{
    if (modelFault || controller->busy() || !controller->snapshot().capabilities.count(op)) return false;
    if (op == Operation::EditWorker || op == Operation::RemoveWorker || op == Operation::SetSupervisor) return pages->currentIndex() == 1 && !selectedWorker().isEmpty();
    const ComponentRecord *node = treeModel->component(selection);
    if (op == Operation::SetPriority && node && node->group) return false;
    if (std::find(nodeOperations.begin(), nodeOperations.end(), op) != nodeOperations.end()) return pages->currentIndex() == 0 && node && node->allowedActions.count(op);
    if ((op == Operation::AddItem || op == Operation::AddGroup) && node) {
        const ComponentRecord *parent = node->group ? node : treeModel->component(q(node->parentId));
        return !parent || parent->allowedActions.count(op);
    }
    return true;
}

void MainWindow::updateActions()
{
    for (auto it = actions.begin(); it != actions.end(); ++it) {
        it.value()->setEnabled(supported(it.key()));
        it.value()->setToolTip(it.value()->isEnabled() ? q(operationName(it.key())) : "Not available for the current selection or connected engine.");
    }
    const bool ready = !controller->busy() && !modelFault;
    openAction->setEnabled(ready && controller->snapshot().canImport);
    saveAction->setEnabled(ready && controller->snapshot().canExport); saveAsAction->setEnabled(saveAction->isEnabled()); refreshAction->setEnabled(!controller->busy());
    const ComponentRecord *node = treeModel->component(selection);
    action(Operation::AddItem)->setText(node && !node->group ? "Add sibling item" : "Add item");
    action(Operation::AddGroup)->setText(node && !node->group ? "Add sibling group" : "Add group");
}

void MainWindow::refreshView()
{
    const WorkflowSnapshot &s = controller->snapshot();
    const bool wasEmpty = treeModel->rowCount() == 0;
    const QString previous = selection, person = selectedWorker();
    const int scroll = tree->verticalScrollBar()->value(); std::set<std::string> expanded;
    for (const ComponentRecord &c : s.components) if (tree->isExpanded(filterModel->mapFromSource(treeModel->indexForId(q(c.id))))) expanded.insert(c.id);
    refreshing = true;
    try { treeModel->setSnapshot(s); modelFault = false; }
    catch (const std::exception &error) {
        modelFault = true; refreshing = false; updateActions(); availability->setText(QString::fromUtf8(error.what())); availability->show(); return;
    }
    filterModel->refresh();
    const QString selectedState = stateFilter->currentData().toString(), selectedPerson = workerFilter->currentData().toString(); const bool wasUnassigned = workerFilter->currentIndex() == 1;
    {
        QSignalBlocker a(stateFilter), b(workerFilter); stateFilter->clear(); stateFilter->addItem("All states", QString());
        std::set<std::string> states = {"AVAILABLE", "IN_PROGRESS", "APPROVED", "COMPLETED", "BLOCKED", "CANCELLED"};
        for (const ComponentRecord &c : s.components) if (!c.state.empty()) states.insert(c.state);
        for (const std::string &state : states) stateFilter->addItem(Theme::stateLabel(q(state)), q(state));
        stateFilter->setCurrentIndex(std::max(0, stateFilter->findData(selectedState)));
        workerFilter->clear(); workerFilter->addItem("All assignees", QString()); workerFilter->addItem("Unassigned", QString());
        for (const WorkerRecord &w : s.workers) workerFilter->addItem(q(w.name) + " [" + q(w.id).left(8) + "]", q(w.id));
        workerFilter->setCurrentIndex(wasUnassigned ? 1 : std::max(0, workerFilter->findData(selectedPerson)));
    }
    updateFilters();
    for (const std::string &id : expanded) tree->setExpanded(filterModel->mapFromSource(treeModel->indexForId(q(id))), true);
    if (wasEmpty && !s.components.empty()) tree->expandToDepth(1);
    tree->verticalScrollBar()->setValue(scroll);
    workers->setSortingEnabled(false); workers->setRowCount(static_cast<int>(s.workers.size()));
    QMap<QString, QString> names; for (const WorkerRecord &w : s.workers) names.insert(q(w.id), q(w.name));
    for (int row = 0; row < workers->rowCount(); ++row) {
        const WorkerRecord &w = s.workers[static_cast<std::size_t>(row)]; const QStringList values = {q(w.name), q(w.role), q(w.email), names.value(q(w.supervisorId), "None"), QString::number(w.workload)};
        for (int col = 0; col < 5; ++col) {
            std::unique_ptr<QTableWidgetItem> item(new QTableWidgetItem(values[col]));
            item->setData(Qt::UserRole, q(w.id));
            if (col == 4) item->setData(Qt::DisplayRole, w.workload);
            workers->setItem(row, col, item.get()); item.release();
        }
    }
    workers->setSortingEnabled(true);
    for (int row = 0; row < workers->rowCount(); ++row) if (workers->item(row, 0)->data(Qt::UserRole).toString() == person) workers->selectRow(row);
    const bool following = activity->verticalScrollBar()->value() >= activity->verticalScrollBar()->maximum() - 2;
    activityModel->setRecords(s.activity); if (following) activity->scrollToBottom();
    strategyLabel->setText("Assignment strategy: " + (s.assignmentStrategy.empty() ? QStringLiteral("not selected") : q(s.assignmentStrategy)));
    title->setText(s.name.empty() ? "Your workflow workspace" : q(s.name));
    counts->setText(QString("%1 %2  ·  %3 %4  ·  %5 %6").arg(s.components.size()).arg(s.components.size() == 1 ? "component" : "components")
        .arg(s.workers.size()).arg(s.workers.size() == 1 ? "worker" : "workers").arg(s.dependencies.size()).arg(s.dependencies.size() == 1 ? "dependency" : "dependencies"));
    availability->setText(q(s.availabilityMessage)); availability->setVisible(!s.availabilityMessage.empty()); emptyTree->setVisible(filterModel->rowCount() == 0);
    emptyTree->setText(s.components.empty() ? "No work to display. Create or open a workflow to begin." : "No work matches these filters. Clear the filters to see all work.");
    WorkflowSnapshot graphSnapshot = s;
    for (ComponentRecord &c : graphSnapshot.components) if (c.group) {
        const QModelIndex index = treeModel->indexForId(q(c.id));
        c.priority = index.data(WorkflowTreeModel::PriorityRole).toInt();
        if (c.state.empty()) c.state = index.sibling(index.row(), 1).data().toString().toStdString();
    }
    graph->setSnapshot(std::move(graphSnapshot)); refreshing = false;
    if (treeModel->component(previous)) selectById(previous);
    else if (filterModel->rowCount() > 0) tree->setCurrentIndex(filterModel->index(0, 0));
    else { selection.clear(); updateInspector(); }
    updateActions();
    setWindowTitle((controller->dirty() ? "• " : "") + (s.name.empty() ? QStringLiteral("TIVIDY") : q(s.name) + " | TIVIDY"));
}

void MainWindow::updateInspector()
{
    const ComponentRecord *c = treeModel->component(selection);
    if (!c) { inspectorTitle->setText("Select a work item"); inspectorText->setText("Details and available actions will appear here."); return; }
    inspectorTitle->setText(q(c->name));
    const QModelIndex index = treeModel->indexForId(selection);
    const QString name = index.sibling(index.row(), 2).data().toString();
    QString text = (c->group ? "Group" : "Item") + QString("\nState: ") + (c->state.empty() && c->group ? QStringLiteral("See descendant progress in the tree") : Theme::stateLabel(q(c->state)))
        + "\nAssignee: " + name + (c->group ? "\nPriority (highest descendant): " : "\nPriority: ") + index.sibling(index.row(), 3).data().toString() + "\nDeadline: " + (c->deadline.empty() ? QStringLiteral("None") : q(c->deadline));
    if (c->rejected) text += "\nRejected: " + q(c->rejectionReason);
    if (c->escalated) text += "\nEscalated";
    int incoming = 0, outgoing = 0; for (const DependencyRecord &d : controller->snapshot().dependencies) { if (d.dependentId == c->id) ++incoming; if (d.prerequisiteId == c->id) ++outgoing; }
    text += QString("\n\nPrerequisites: %1\nDependents: %2").arg(incoming).arg(outgoing); inspectorText->setText(text);
}

void MainWindow::updateFilters()
{
    filterModel->setCriteria(search->text(), stateFilter->currentData().toString(), workerFilter->currentIndex() > 1 ? workerFilter->currentData().toString() : QString(),
        workerFilter->currentIndex() == 1, priorityEnabled->isChecked(), priorityMin->value(), priorityMax->value());
    if (!search->text().trimmed().isEmpty() || stateFilter->currentIndex() > 0 || workerFilter->currentIndex() > 0 || priorityEnabled->isChecked()) tree->expandAll();
    if (!refreshing) { emptyTree->setVisible(filterModel->rowCount() == 0); updateActions(); }
}

void MainWindow::clearFilters()
{
    QSignalBlocker a(search), b(stateFilter), c(workerFilter), d(priorityEnabled);
    search->clear(); stateFilter->setCurrentIndex(0); workerFilter->setCurrentIndex(0); priorityEnabled->setChecked(false);
    priorityMin->setEnabled(false); priorityMax->setEnabled(false); updateFilters();
}

void MainWindow::selectById(const QString &id, bool fromGraph)
{
    const QModelIndex source = treeModel->indexForId(id); if (!source.isValid()) return;
    QModelIndex target = filterModel->mapFromSource(source);
    if (!target.isValid() && fromGraph) { clearFilters(); target = filterModel->mapFromSource(source); statusBar()->showMessage("Filters cleared to reveal the selected graph component.", 6000); }
    if (!target.isValid()) return;
    selection = id; tree->setCurrentIndex(target); tree->scrollTo(target); if (!fromGraph) graph->selectComponent(id); updateInspector(); updateActions();
}

void MainWindow::contextMenu(const QString &id, const QPoint &position)
{
    if (!id.isEmpty()) selectById(id, true);
    QMenu menu(this);
    for (Operation op : {Operation::AddItem,Operation::AddGroup,Operation::RenameComponent}) menu.addAction(action(op));
    menu.addSeparator(); for (Operation op : {Operation::Assign,Operation::Unassign,Operation::Progress,Operation::Review,Operation::Reject,Operation::Escalate,Operation::Cancel}) menu.addAction(action(op));
    menu.addSeparator(); menu.addAction(action(Operation::AddDependency)); menu.addAction(action(Operation::RemoveDependency));
    menu.addSeparator(); menu.addAction(action(Operation::ConfigureNotifier)); menu.addAction(action(Operation::ConfigureAudit)); menu.exec(position);
}

void MainWindow::perform(Operation op)
{
    if (!supported(op)) return;
    if (op == Operation::AddWorker) pages->setCurrentIndex(1);
    const bool people = op == Operation::EditWorker || op == Operation::RemoveWorker || op == Operation::SetSupervisor;
    const QString target = people ? selectedWorker() : selection;
    OperationRequest request;
    if (!Dialogs::request(this, op, controller->snapshot(), target, request)) return;
    if (op == Operation::Cancel || op == Operation::RemoveWorker) {
        const ComponentRecord *c = treeModel->component(target);
        if (!Dialogs::confirm(this, q(operationName(op)), op == Operation::Cancel ? "Cancel “" + (c ? q(c->name) : target) + "”? The engine will validate the effect on related work." : "Remove the selected worker? The engine will validate assignments and hierarchy references.")) return;
    }
    if (op == Operation::CreateWorkflow) protectUnsaved([this, request]() { controller->execute(request); });
    else controller->execute(request);
}

void MainWindow::protectUnsaved(std::function<void()> next)
{
    if (!controller->dirty()) { next(); return; }
    QMessageBox box(QMessageBox::Question, "Unsaved changes", "Save changes before continuing?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, this);
    box.setDefaultButton(QMessageBox::Cancel); box.button(QMessageBox::Save)->setEnabled(controller->snapshot().canExport);
    const int answer = box.exec();
    if (answer == QMessageBox::Discard) next();
    else if (answer == QMessageBox::Save) { afterSave = std::move(next); saveWorkflow(); }
}

void MainWindow::openWorkflow()
{
    const QString file = QFileDialog::getOpenFileName(this, "Open workflow", controller->filePath(), "Workflow JSON (*.json)");
    if (!file.isEmpty()) protectUnsaved([this, file]() { controller->openFile(file); });
}
void MainWindow::saveWorkflow(bool choosePath)
{
    QString file = controller->filePath();
    if (choosePath || file.isEmpty()) file = QFileDialog::getSaveFileName(this, "Save workflow", file.isEmpty() ? "workflow.json" : file, "Workflow JSON (*.json)");
    if (file.isEmpty()) { afterSave = {}; return; }
    if (QFileInfo(file).suffix().isEmpty()) file += ".json";
    controller->saveFile(file);
}

void MainWindow::showActivityDetails(const QModelIndex &index)
{
    QDialog dialog(this); dialog.setWindowTitle("Activity details"); dialog.resize(650, 350); QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QPlainTextEdit *details = new QPlainTextEdit(&dialog); details->setReadOnly(true);
    QStringList lines; for (int col = 0; col < 4; ++col) lines << activityFilter->index(index.row(), col).data().toString(); details->setPlainText(lines.join("\n\n")); layout->addWidget(details);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog); connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject); layout->addWidget(buttons); dialog.exec();
}
void MainWindow::exportActivity()
{
    if (controller->busy()) return;
    const QString file = QFileDialog::getSaveFileName(this, "Export visible activity", "activity.csv", "CSV (*.csv)"); if (file.isEmpty()) return;
    QString csv = "Timestamp,Category,Work,Message\r\n";
    for (int row = 0; row < activityFilter->rowCount(); ++row) {
        QStringList fields;
        for (int col = 0; col < 4; ++col) {
            QString value = activityFilter->index(row, col).data().toString();
            if (!value.isEmpty() && QString("=+-@\t\r").contains(value.front())) value.prepend('\'');
            value.replace('"', "\"\""); fields << '"' + value + '"';
        }
        csv += fields.join(',') + "\r\n";
    }
    controller->exportActivity(file, csv.toUtf8());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (!centralWidget() || !activityDock) return;
    const bool compact = event->size().height() < 800;
    counts->setVisible(!compact);
    if (QLabel *eyebrow = centralWidget()->findChild<QLabel *>("eyebrow")) eyebrow->setVisible(!compact);
    title->setProperty("compact", compact);
    title->style()->unpolish(title); title->style()->polish(title);
    centralWidget()->layout()->setContentsMargins(compact ? 16 : 24, compact ? 8 : 20, compact ? 16 : 24, 8);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (controller->busy()) { statusBar()->showMessage("Please wait for the current operation to finish before closing.", 5000); event->ignore(); return; }
    if (controller->dirty()) {
        event->ignore(); protectUnsaved([this]() { QSettings settings; settings.setValue("workspace/geometry", saveGeometry()); settings.setValue("workspace/layout", saveState()); hide(); QTimer::singleShot(0, qApp, &QApplication::quit); }); return;
    }
    QSettings settings; settings.setValue("workspace/geometry", saveGeometry()); settings.setValue("workspace/layout", saveState()); event->accept();
}
}
