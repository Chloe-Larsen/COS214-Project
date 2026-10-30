#ifndef TIVIDY_GUI_COREWORKFLOWGATEWAY_H
#define TIVIDY_GUI_COREWORKFLOWGATEWAY_H

#include "IWorkflowGateway.h"
#include <memory>

class WorkflowManager;

namespace tividy {

/** Optional, real engine functions installed by the core integration owner. */
struct CoreBindings {
    std::function<WorkflowSnapshot()> read;
    std::function<OperationResult(const OperationRequest &)> execute;
    std::function<OperationResult(const WorkflowDocument &)> import;
    std::function<WorkflowDocument()> exportState;
    std::function<void(std::function<void()>)> subscribe;
};

/**
 * Calls the supplied core without accessing private fields or adding domain
 * logic. The ZIP's worker add/list API works now. Other operations require the
 * explicit bindings described in the integration document; they are disabled
 * rather than simulated when unavailable.
 */
class CoreWorkflowGateway : public IWorkflowGateway {
public:
    /** Retain the actual manager and optional completed engine bindings. */
    CoreWorkflowGateway(std::shared_ptr<WorkflowManager> manager, CoreBindings bindings);
    /** Release owners after the controller has disconnected on the engine queue. */
    ~CoreWorkflowGateway() override;
    /** Read actual engine data. */
    WorkflowSnapshot snapshot() override;
    /** Invoke a binding, or the available legacy add-worker API. */
    OperationResult execute(const OperationRequest &request) override;
    /** Require an atomic core import binding. */
    OperationResult importDocument(const WorkflowDocument &document) override;
    /** Require a lossless core export binding. */
    WorkflowDocument exportDocument() override;
    /** Forward invalidations without forwarding engine object pointers. */
    void setInvalidationHandler(std::function<void()> handler) override;

private:
    std::shared_ptr<WorkflowManager> manager;
    CoreBindings bindings;
    std::function<void()> invalidated;
    std::uint64_t revision;
};

/** Construct the application gateway; the sole engine wiring location. */
std::shared_ptr<IWorkflowGateway> createGateway();

} // namespace tividy
#endif
