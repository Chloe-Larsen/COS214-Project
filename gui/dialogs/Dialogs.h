#ifndef TIVIDY_GUI_DIALOGS_H
#define TIVIDY_GUI_DIALOGS_H
#include "integration/WorkflowTypes.h"
#include <QString>
class QWidget;

namespace tividy {
/** Validated data-entry dialogs; the engine still decides whether an action is legal. */
class Dialogs {
public:
    /** Populate a request; false means the user cancelled without any mutation. */
    static bool request(QWidget *parent, Operation operation, const WorkflowSnapshot &snapshot,
                        const QString &selectedId, OperationRequest &request);
    /** Show an error as plain text, never as executable/formatting HTML. */
    static void error(QWidget *parent, const QString &message);
    /** Confirm a destructive operation with the selected component's name. */
    static bool confirm(QWidget *parent, const QString &title, const QString &message);
};
}
#endif
