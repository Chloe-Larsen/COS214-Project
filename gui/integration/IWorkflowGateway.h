#ifndef TIVIDY_GUI_IWORKFLOWGATEWAY_H
#define TIVIDY_GUI_IWORKFLOWGATEWAY_H

#include "WorkflowTypes.h"
#include <functional>

namespace tividy {

/**
 * Framework-neutral engine boundary. All methods run on the controller's one
 * engine thread. Event callbacks only request a refresh, and must not access Qt.
 * The core owns all domain rules and must implement atomic mutations/imports.
 */
class IWorkflowGateway {
public:
    /** Release the connection and any engine state owned by this gateway. */
    virtual ~IWorkflowGateway() {}
    /** Return a consistent value snapshot and authoritative capabilities. */
    virtual WorkflowSnapshot snapshot() = 0;
    /** Execute one validated engine operation, or return structured failure. */
    virtual OperationResult execute(const OperationRequest &request) = 0;
    /** Validate and install a document atomically, without trusting file states. */
    virtual OperationResult importDocument(const WorkflowDocument &document) = 0;
    /** Export all state required for a lossless subsequent import. */
    virtual WorkflowDocument exportDocument() = 0;
    /** Connect an invalidation callback; passing an empty callback disconnects. */
    virtual void setInvalidationHandler(std::function<void()> handler) = 0;
};

} // namespace tividy
#endif
