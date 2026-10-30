#include "WorkflowController.h"
#include "services/JsonCodec.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QFile>
#include <QFutureWatcher>
#include <QSaveFile>
#include <QTimer>
#include <stdexcept>
#include <utility>

namespace tividy {
namespace {
QString describe(const OperationResult &result)
{
    QString message = QString::fromStdString(result.message);
    for (const Diagnostic &d : result.diagnostics) {
        message += "\n";
        if (d.line > 0) message += QString("Line %1, column %2: ").arg(d.line).arg(d.column);
        else if (!d.location.empty()) message += QString::fromStdString(d.location) + ": ";
        message += QString::fromStdString(d.message);
    }
    return message.isEmpty() ? (result.ok ? QStringLiteral("Completed.") : QStringLiteral("The operation was rejected by the workflow engine.")) : message;
}
}

WorkflowController::WorkflowController(std::shared_ptr<IWorkflowGateway> connection, QObject *parent)
    : QObject(parent), gateway(std::move(connection)), invalidated(new std::atomic<bool>(false)),
      working(false), subscribed(false), haveSavedRevision(false), savedRevision(0)
{
    if (!gateway) throw std::invalid_argument("A workflow gateway is required.");
    engineThread.setMaxThreadCount(1);
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (!working && invalidated->exchange(false)) refresh();
    });
    timer->start(150);
}

WorkflowController::~WorkflowController()
{
    const std::shared_ptr<IWorkflowGateway> engine = gateway;
    QtConcurrent::run(&engineThread, [engine]() {
        try { engine->setInvalidationHandler(std::function<void()>()); } catch (...) {}
    }).waitForFinished();
    engineThread.waitForDone();
}

const WorkflowSnapshot &WorkflowController::snapshot() const { return current; }
bool WorkflowController::busy() const { return working; }
bool WorkflowController::dirty() const { return haveSavedRevision ? current.revision != savedRevision : current.dirty; }
QString WorkflowController::filePath() const { return path; }

void WorkflowController::start(std::function<Outcome()> job)
{
    if (working) { emit notice("Please wait for the current operation to finish."); return; }
    working = true; emit busyChanged(true);
    QFutureWatcher<Outcome> *watcher = new QFutureWatcher<Outcome>(this);
    connect(watcher, &QFutureWatcher<Outcome>::finished, this, [this, watcher]() {
        const Outcome result = watcher->result();
        watcher->deleteLater();
        working = false;
        if (result.hasSnapshot) current = result.snapshot;
        if (result.success && result.fresh) { path.clear(); haveSavedRevision = false; }
        if (result.success && (result.saved || result.imported)) {
            path = result.path; savedRevision = current.revision; haveSavedRevision = true;
        }
        if (result.success && result.hasSnapshot && result.snapshot.name.empty()) { path.clear(); haveSavedRevision = false; }
        if (result.hasSnapshot) emit snapshotChanged();
        emit busyChanged(false);
        if (!result.message.isEmpty()) {
            if (result.success) emit notice(result.message); else emit failure(result.message);
        }
        if (result.success && !result.selection.isEmpty()) emit selectRequested(result.selection);
        if (result.success && result.saved) emit saved();
    });
    watcher->setFuture(QtConcurrent::run(&engineThread, [job]() {
        try { return job(); }
        catch (const std::exception &error) { Outcome out; out.message = QString::fromUtf8(error.what()); return out; }
        catch (const std::string &error) { Outcome out; out.message = QString::fromStdString(error); return out; }
        catch (const char *error) { Outcome out; out.message = error ? QString::fromUtf8(error) : QStringLiteral("Unknown engine error."); return out; }
        catch (...) { Outcome out; out.message = "The operation could not be completed. The engine reported an unknown error."; return out; }
    }));
}

void WorkflowController::refresh()
{
    if (working) { invalidated->store(true); return; }
    const std::shared_ptr<IWorkflowGateway> engine = gateway;
    const std::shared_ptr<std::atomic<bool> > flag = invalidated;
    const bool connectEvents = !subscribed;
    subscribed = true;
    start([engine, flag, connectEvents]() {
        if (connectEvents) engine->setInvalidationHandler([flag]() { flag->store(true); });
        Outcome result; result.snapshot = engine->snapshot(); result.hasSnapshot = true; result.success = true; return result;
    });
}

void WorkflowController::execute(OperationRequest request)
{
    const std::shared_ptr<IWorkflowGateway> engine = gateway;
    start([engine, request]() {
        const OperationResult answer = engine->execute(request);
        Outcome result; result.success = answer.ok; result.message = describe(answer);
        result.fresh = answer.ok && request.operation == Operation::CreateWorkflow;
        result.selection = QString::fromStdString(answer.selectedId);
        result.snapshot = engine->snapshot(); result.hasSnapshot = true; return result;
    });
}

void WorkflowController::openFile(QString file)
{
    if (!current.canImport) { emit failure("Opening a workflow is not available in the connected engine."); return; }
    const std::shared_ptr<IWorkflowGateway> engine = gateway;
    start([engine, file]() {
        QFile input(file);
        if (!input.open(QIODevice::ReadOnly)) throw std::runtime_error(input.errorString().toStdString());
        if (input.size() > 64 * 1024 * 1024) throw std::runtime_error("The JSON file exceeds the 64 MiB import limit.");
        const QByteArray bytes = input.read(64 * 1024 * 1024 + 1);
        if (input.error() != QFileDevice::NoError) throw std::runtime_error(input.errorString().toStdString());
        const WorkflowDocument document = JsonCodec::decode(std::string(bytes.constData(), static_cast<std::size_t>(bytes.size())));
        const OperationResult answer = engine->importDocument(document);
        Outcome result; result.success = answer.ok; result.message = describe(answer); result.imported = answer.ok;
        result.path = file; result.snapshot = engine->snapshot(); result.hasSnapshot = true; return result;
    });
}

void WorkflowController::exportActivity(QString file, QByteArray csv)
{
    start([file, csv]() {
        QSaveFile output(file);
        if (!output.open(QIODevice::WriteOnly) || output.write(csv) != csv.size() || !output.commit())
            throw std::runtime_error(output.errorString().toStdString());
        Outcome result; result.success = true; result.message = "Visible activity exported."; return result;
    });
}

void WorkflowController::saveFile(QString file)
{
    if (!current.canExport) { emit failure("Saving is not available in the connected engine."); return; }
    const std::shared_ptr<IWorkflowGateway> engine = gateway;
    start([engine, file]() {
        const std::string json = JsonCodec::encode(engine->exportDocument());
        QSaveFile output(file);
        if (!output.open(QIODevice::WriteOnly)) throw std::runtime_error(output.errorString().toStdString());
        if (output.write(json.data(), static_cast<qint64>(json.size())) != static_cast<qint64>(json.size()) || !output.commit())
            throw std::runtime_error(output.errorString().toStdString());
        Outcome result; result.success = true; result.saved = true; result.path = file;
        result.message = "Workflow saved."; result.snapshot = engine->snapshot(); result.hasSnapshot = true; return result;
    });
}
}
