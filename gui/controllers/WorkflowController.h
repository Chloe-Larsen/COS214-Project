#ifndef TIVIDY_GUI_WORKFLOWCONTROLLER_H
#define TIVIDY_GUI_WORKFLOWCONTROLLER_H
#include "integration/IWorkflowGateway.h"
#include <QObject>
#include <QThreadPool>
#include <QString>
#include <QByteArray>
#include <atomic>
#include <memory>

namespace tividy {
/** Serialises every engine access and marshals copied results to the UI thread. */
class WorkflowController : public QObject {
    Q_OBJECT
public:
    /** Retain a gateway. Engine callbacks may only signal an atomic invalidation. */
    explicit WorkflowController(std::shared_ptr<IWorkflowGateway> gateway, QObject *parent = nullptr);
    /** Disconnect the event bridge and drain engine work before releasing state. */
    ~WorkflowController() override;
    /** Latest engine snapshot, borrowed on the UI thread only. */
    const WorkflowSnapshot &snapshot() const;
    /** Whether an engine/file operation is running. */
    bool busy() const;
    /** Whether current state differs from the last imported/saved revision. */
    bool dirty() const;
    /** Path of the successfully imported/saved document, or empty. */
    QString filePath() const;
    /** Queue a refresh when idle. */
    void refresh();
    /** Submit one operation; busy callers receive a notice without execution. */
    void execute(OperationRequest request);
    /** Decode, validate and ask the core to atomically import a file. */
    void openFile(QString path);
    /** Ask the core for a lossless export and atomically replace the target file. */
    void saveFile(QString path);
    /** Export visible activity data without blocking the GUI thread. */
    void exportActivity(QString path, QByteArray csv);
signals:
    /** A complete new snapshot is available on the UI thread. */
    void snapshotChanged();
    /** Activity state changed; views should gate mutating actions. */
    void busyChanged(bool busy);
    /** User-facing failure with plain text detail. */
    void failure(QString message);
    /** Non-error status message. */
    void notice(QString message);
    /** Select the component returned by a successful engine transaction. */
    void selectRequested(QString id);
    /** A requested save completed, allowing a close-after-save flow. */
    void saved();
private:
    struct Outcome {
        WorkflowSnapshot snapshot;
        bool hasSnapshot = false, success = false, saved = false, imported = false, fresh = false;
        QString message, selection, path;
    };
    void start(std::function<Outcome()> job);
    std::shared_ptr<IWorkflowGateway> gateway;
    std::shared_ptr<std::atomic<bool> > invalidated;
    QThreadPool engineThread;
    WorkflowSnapshot current;
    bool working, subscribed, haveSavedRevision;
    std::uint64_t savedRevision;
    QString path;
};
}
#endif
