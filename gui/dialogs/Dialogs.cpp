#include "Dialogs.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QSpinBox>
#include <QUuid>
#include <QVBoxLayout>
#include <climits>
#include <functional>
#include <memory>

namespace tividy {
namespace {
QString q(const std::string &value) { return QString::fromStdString(value); }
const ComponentRecord *component(const WorkflowSnapshot &s, const QString &id) {
    for (const ComponentRecord &c : s.components) if (q(c.id) == id) return &c;
    return nullptr;
}
const WorkerRecord *worker(const WorkflowSnapshot &s, const QString &id) {
    for (const WorkerRecord &w : s.workers) if (q(w.id) == id) return &w;
    return nullptr;
}
void choose(QComboBox *combo, const QString &id) { const int index = combo->findData(id); if (index >= 0) combo->setCurrentIndex(index); }
QComboBox *workerChoice(QDialog *dialog, const WorkflowSnapshot &s, const QString &exclude = QString()) {
    QComboBox *combo = new QComboBox(dialog); combo->addItem("Unassigned / none", QString());
    for (const WorkerRecord &w : s.workers) if (q(w.id) != exclude) combo->addItem(q(w.name) + " · " + q(w.role) + " [" + q(w.id).left(8) + "]", q(w.id));
    return combo;
}
bool run(QDialog &dialog, std::unique_ptr<QFormLayout> &form, const std::function<QString()> &validate) {
    QVBoxLayout *layout = new QVBoxLayout(&dialog); layout->setContentsMargins(24, 24, 24, 24); layout->setSpacing(16);
    layout->addLayout(form.release());
    QLabel *error = new QLabel(&dialog); error->setObjectName("availability"); error->setWordWrap(true); error->setTextFormat(Qt::PlainText); error->hide(); layout->addWidget(error);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setText("Apply"); buttons->button(QDialogButtonBox::Ok)->setObjectName("primary");
    layout->addWidget(buttons);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, [&]() {
        const QString message = validate();
        if (!message.isEmpty()) { error->setText(message); error->show(); return; }
        dialog.accept();
    });
    dialog.setMinimumWidth(470); return dialog.exec() == QDialog::Accepted;
}
QLineEdit *field(QDialog &dialog, const QString &value = QString(), int limit = 256) {
    QLineEdit *edit = new QLineEdit(value, &dialog); edit->setMaxLength(limit); return edit;
}
}

void Dialogs::error(QWidget *parent, const QString &message)
{
    QMessageBox box(QMessageBox::Warning, "Operation could not be completed", QString(), QMessageBox::Ok, parent);
    box.setTextFormat(Qt::PlainText);
    box.setText(message.left(1200));
    if (message.size() > 1200) box.setDetailedText(message);
    box.exec();
}

bool Dialogs::confirm(QWidget *parent, const QString &title, const QString &message)
{
    QMessageBox box(QMessageBox::Question, title, QString(), QMessageBox::Yes | QMessageBox::No, parent);
    box.setTextFormat(Qt::PlainText); box.setText(message); box.setDefaultButton(QMessageBox::No);
    return box.exec() == QMessageBox::Yes;
}

bool Dialogs::request(QWidget *parent, Operation op, const WorkflowSnapshot &s, const QString &selectedId, OperationRequest &out)
{
    out = OperationRequest(); out.operation = op; out.targetId = selectedId.toStdString();
    const ComponentRecord *selected = component(s, selectedId);
    QDialog dialog(parent); dialog.setWindowTitle(q(operationName(op))); dialog.setModal(true);
    std::unique_ptr<QFormLayout> ownedForm(new QFormLayout());
    QFormLayout *form = ownedForm.get(); form->setSpacing(12); form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    if (op == Operation::CreateWorkflow) {
        QLineEdit *name = field(dialog); name->setPlaceholderText("Workflow name"); name->setObjectName("workflowName");
        form->addRow("Name *", name);
        if (!run(dialog, ownedForm, [=]() { return name->text().trimmed().isEmpty() ? QString("Enter a workflow name.") : QString(); })) return false;
        out.text = name->text().trimmed().toStdString(); return true;
    }
    if (op == Operation::AddItem || op == Operation::AddGroup || op == Operation::RenameComponent) {
        QLineEdit *name = field(dialog, op == Operation::RenameComponent && selected ? q(selected->name) : QString());
        name->setObjectName("componentName");
        form->addRow("Name *", name);
        QSpinBox *priority = new QSpinBox(&dialog); priority->setRange(INT_MIN, INT_MAX); priority->setValue(0);
        QLineEdit *deadline = field(dialog); deadline->setPlaceholderText("YYYY-MM-DD, or leave empty");
        QComboBox *assigned = workerChoice(&dialog, s);
        if (op == Operation::AddItem) { form->addRow("Priority", priority); form->addRow("Deadline", deadline); form->addRow("Assignee", assigned); }
        if (!run(dialog, ownedForm, [=]() {
            if (name->text().trimmed().isEmpty()) return QString("Enter a name.");
            if (op == Operation::AddItem && !deadline->text().isEmpty()) {
                const QDate date = QDate::fromString(deadline->text(), "yyyy-MM-dd");
                if (!date.isValid() || date.toString("yyyy-MM-dd") != deadline->text()) return QString("Use a valid date in YYYY-MM-DD format.");
            }
            return QString();
        })) return false;
        out.component.id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
        out.component.name = name->text().trimmed().toStdString(); out.text = out.component.name;
        out.component.group = op == Operation::AddGroup; out.component.priority = priority->value();
        out.component.deadline = deadline->text().toStdString(); out.component.workerId = assigned->currentData().toString().toStdString();
        out.component.parentId = selected ? (selected->group ? selected->id : selected->parentId) : std::string();
        return true;
    }
    if (op == Operation::SetPriority) {
        QSpinBox *priority = new QSpinBox(&dialog); priority->setRange(INT_MIN, INT_MAX); priority->setValue(selected ? selected->priority : 0);
        form->addRow("Priority", priority);
        if (!run(dialog, ownedForm, []() { return QString(); })) return false;
        out.component.priority = priority->value(); return true;
    }
    if (op == Operation::Assign || op == Operation::SetSupervisor) {
        QComboBox *people = workerChoice(&dialog, s, op == Operation::SetSupervisor ? selectedId : QString());
        if (op == Operation::Assign && selected) choose(people, q(selected->workerId));
        const WorkerRecord *person = worker(s, selectedId);
        if (op == Operation::SetSupervisor && person) choose(people, q(person->supervisorId));
        form->addRow(op == Operation::Assign ? "Assignee" : "Superior", people);
        if (!run(dialog, ownedForm, [=]() { return op == Operation::Assign && people->currentData().toString().isEmpty() ? QString("Choose a worker. Use Unassign to remove an assignment.") : QString(); })) return false;
        out.relatedId = people->currentData().toString().toStdString(); return true;
    }
    if (op == Operation::AddWorker || op == Operation::EditWorker) {
        const WorkerRecord *person = worker(s, selectedId);
        if (op == Operation::EditWorker && !person) { return false; }
        QLineEdit *name = field(dialog, person ? q(person->name) : QString()); name->setObjectName("workerName");
        QLineEdit *role = field(dialog, person ? q(person->role) : QString()); role->setObjectName("workerRole");
        QLineEdit *email = field(dialog, person ? q(person->email) : QString());
        const std::string id = person ? person->id : QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
        QComboBox *superior = workerChoice(&dialog, s, q(id));
        email->setEnabled(s.workerInputFields.count("email") > 0);
        superior->setEnabled(s.workerInputFields.count("supervisor") > 0);
        if (!email->isEnabled()) email->setToolTip("Email entry is not available for this connection.");
        if (!superior->isEnabled()) superior->setToolTip("Superior selection is not available for this connection.");
        if (person) choose(superior, q(person->supervisorId));
        form->addRow("Name *", name); form->addRow("Role *", role); form->addRow("Email", email); form->addRow("Superior", superior);
        if (!run(dialog, ownedForm, [=]() {
            if (name->text().trimmed().isEmpty() || role->text().trimmed().isEmpty()) return QString("Name and role are required.");
            if (!email->text().trimmed().isEmpty() && !QRegularExpression("^[^\\s@]+@[^\\s@]+$").match(email->text().trimmed()).hasMatch()) return QString("Enter an email address with one @ and no spaces.");
            return QString();
        })) return false;
        out.worker.id = id; out.worker.name = name->text().trimmed().toStdString(); out.worker.role = role->text().trimmed().toStdString();
        out.worker.email = email->text().trimmed().toStdString(); out.worker.supervisorId = superior->currentData().toString().toStdString(); return true;
    }
    if (op == Operation::Reject) {
        QPlainTextEdit *reason = new QPlainTextEdit(&dialog); reason->setPlaceholderText("Explain why the work is rejected.");
        form->addRow("Reason *", reason);
        if (!run(dialog, ownedForm, [=]() {
            if (reason->toPlainText().trimmed().isEmpty()) return QString("Enter a rejection reason.");
            return reason->toPlainText().size() > 8000 ? QString("Keep the reason below 8,000 characters.") : QString();
        })) return false;
        out.text = reason->toPlainText().trimmed().toStdString(); return true;
    }
    if (op == Operation::AddDependency || op == Operation::RemoveDependency) {
        if (op == Operation::RemoveDependency) {
            QComboBox *edges = new QComboBox(&dialog);
            for (std::size_t i = 0; i < s.dependencies.size(); ++i) {
                const DependencyRecord &d = s.dependencies[i];
                if (!selectedId.isEmpty() && q(d.prerequisiteId) != selectedId && q(d.dependentId) != selectedId) continue;
                const ComponentRecord *a = component(s, q(d.prerequisiteId)), *b = component(s, q(d.dependentId));
                edges->addItem((a ? q(a->name) : q(d.prerequisiteId)) + " → " + (b ? q(b->name) : q(d.dependentId)), static_cast<int>(i));
            }
            form->addRow("Dependency", edges);
            if (!run(dialog, ownedForm, [=]() { return edges->count() ? QString() : QString("There are no dependencies to remove for this selection."); })) return false;
            const DependencyRecord &d = s.dependencies[static_cast<std::size_t>(edges->currentData().toInt())];
            out.targetId = d.dependentId; out.relatedId = d.prerequisiteId; return true;
        }
        QComboBox *a = new QComboBox(&dialog), *b = new QComboBox(&dialog);
        for (const ComponentRecord &c : s.components) {
            const QString label = (c.group ? "Group · " : "Item · ") + q(c.name) + " [" + q(c.id).left(8) + "]";
            a->addItem(label, q(c.id)); b->addItem(label, q(c.id));
        }
        choose(b, selectedId); form->addRow("Prerequisite", a); form->addRow("Dependent", b);
        QLabel *hint = new QLabel("The dependent waits for the prerequisite. The engine validates group rules and cycles.", &dialog); hint->setWordWrap(true); form->addRow(hint);
        if (!run(dialog, ownedForm, [=]() { return a->count() == 0 || a->currentData() == b->currentData() ? QString("Choose two different components.") : QString(); })) return false;
        out.targetId = b->currentData().toString().toStdString(); out.relatedId = a->currentData().toString().toStdString(); return true;
    }
    if (op == Operation::SetStrategy) {
        QComboBox *strategy = new QComboBox(&dialog); strategy->addItem("Role based", "RoleBased"); strategy->addItem("Workload based", "WorkloadBased"); choose(strategy, q(s.assignmentStrategy));
        QComboBox *role = new QComboBox(&dialog); role->setEditable(true); role->addItem("", "");
        std::set<std::string> roles; for (const WorkerRecord &w : s.workers) roles.insert(w.role);
        for (const std::string &r : roles) role->addItem(q(r));
        const auto found = s.strategyOptions.find("role"); if (found != s.strategyOptions.end()) role->setCurrentText(q(found->second));
        QComboBox *tie = new QComboBox(&dialog); tie->addItem("Stable worker ID", "workerId"); tie->addItem("Worker name", "workerName");
        const auto tieValue = s.strategyOptions.find("tieBreak"); if (tieValue != s.strategyOptions.end()) choose(tie, q(tieValue->second));
        form->addRow("Strategy", strategy); form->addRow("Eligible role (optional)", role); form->addRow("Tie break", tie);
        if (!run(dialog, ownedForm, []() { return QString(); })) return false;
        out.text = strategy->currentData().toString().toStdString(); out.options["role"] = role->currentText().trimmed().toStdString(); out.options["tieBreak"] = tie->currentData().toString().toStdString(); return true;
    }
    if (op == Operation::ConfigureNotifier || op == Operation::ConfigureAudit) {
        QCheckBox *enabled = new QCheckBox("Enabled", &dialog); enabled->setChecked(true);
        QComboBox *scope = new QComboBox(&dialog); if (selected) scope->addItem("Selected component", "component"); scope->addItem("Entire workflow", "workflow");
        QComboBox *recipients = new QComboBox(&dialog); recipients->addItem("Assignee", "assignee"); recipients->addItem("Superior", "superior"); recipients->addItem("Assignee and superior", "both");
        QCheckBox *adapter = new QCheckBox("Include adapter output", &dialog); adapter->setChecked(true);
        form->addRow(enabled); form->addRow("Attach to", scope);
        if (op == Operation::ConfigureNotifier) form->addRow("Notify", recipients);
        form->addRow(adapter);
        const QStringList types = {"ASSIGNED", "COMPLETED", "APPROVED", "REJECTED", "ESCALATED"};
        QList<QCheckBox *> events;
        for (const QString &type : types) { QCheckBox *box = new QCheckBox(type.toLower(), &dialog); box->setChecked(true); events.push_back(box); form->addRow(box); }
        const std::map<std::string, std::string> initial = selected ? (op == Operation::ConfigureNotifier ? selected->notifier : selected->audit) : std::map<std::string, std::string>();
        const auto active = initial.find("enabled"); if (active != initial.end()) enabled->setChecked(active->second == "true");
        const auto recipient = initial.find("recipients"); if (recipient != initial.end()) choose(recipients, q(recipient->second));
        const auto output = initial.find("adapter"); if (output != initial.end()) adapter->setChecked(output->second == "true");
        const auto eventSetting = initial.find("events");
        if (eventSetting != initial.end()) { const QStringList chosen = q(eventSetting->second).split(','); for (int i = 0; i < events.size(); ++i) events[i]->setChecked(chosen.contains(types[i])); }
        if (!run(dialog, ownedForm, [=]() { for (QCheckBox *event : events) if (event->isChecked()) return QString(); return enabled->isChecked() ? QString("Choose at least one event.") : QString(); })) return false;
        out.targetId = scope->currentData().toString() == "workflow" ? std::string() : selectedId.toStdString();
        out.options["enabled"] = enabled->isChecked() ? "true" : "false"; out.options["scope"] = scope->currentData().toString().toStdString();
        out.options["recipients"] = recipients->currentData().toString().toStdString(); out.options["adapter"] = adapter->isChecked() ? "true" : "false";
        QStringList chosen; for (int i = 0; i < events.size(); ++i) if (events[i]->isChecked()) chosen.push_back(types[i]);
        out.options["events"] = chosen.join(',').toStdString(); return true;
    }
    return true;
}
}
