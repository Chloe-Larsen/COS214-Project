#ifndef TIVIDY_GUI_COREWORKFLOWGATEWAY_H
#define TIVIDY_GUI_COREWORKFLOWGATEWAY_H

#include "IWorkflowGateway.h"
#include <memory>

namespace tividy {
/**
 * Owns a GUI session of real core components and workers. All access occurs on
 * WorkflowController's serial queue. Widgets receive copied records only.
 */
class CoreWorkflowGateway : public IWorkflowGateway {
public:
    /** Start an empty workspace without touching the console WorkflowManager. */
    CoreWorkflowGateway();
    /** Release the tree before its workers. */
    ~CoreWorkflowGateway() override;
    /** Copy the current core state, settings, permissions and activity. */
    WorkflowSnapshot snapshot() override;
    /** Validate and stage a request before replacing the live session. */
    OperationResult execute(const OperationRequest &request) override;
    /** Build the complete imported tree off to the side; failure preserves work. */
    OperationResult importDocument(const WorkflowDocument &document) override;
    /** Return the current workflow with settings and retained activity. */
    WorkflowDocument exportDocument() override;
    /** Replace the refresh callback; an empty callback disconnects. */
    void setInvalidationHandler(std::function<void()> handler) override;

private:
    struct Storage;
    std::unique_ptr<Storage> state;
    std::function<void()> invalidated;
    std::uint64_t revision;
};

/** Construct the application gateway; the sole engine wiring location. */
std::shared_ptr<IWorkflowGateway> createGateway();

} // namespace tividy
#endif
