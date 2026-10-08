# COS214 Project – TIVIDY

**Workflow Management System**
**COS 214 – Practical 6**
**Year:** 2026

**GitHub Repository:** Chloe-Larsen/COS214-Project

---

# Team Members

| Name            | Student Number |
| --------------- | -------------- |
| Caleb Jennings  | 25173805       |
| Anchen Kruger   | 25073703       |
| Chloe Larsen    | 25004141       |
| Caitlin Moodley | 25128443       |
| Shanna Reinecke | 25008260       |

---

# Task 1: Research Workflow Management Systems

## Workflow and Workflow Management Systems

The Workflow Management Coalition (WfMC) defines a workflow as the computerised automation of a business process, in whole or in part. A Workflow Management System (WfMS) provides the run-time environment that interprets process definitions, creates workflow instances, and interacts with the people and applications that perform activities [1].

The WfMC reference model separates the workflow engine from definition tools, worklist clients, invoked applications, other workflow engines and monitoring tools. These components communicate through defined interfaces [1].

## Workflow Definitions and Instances

A workflow definition describes the structure and rules of a process. It contains activities, routing rules, roles and control information. Multiple workflow instances can be created from the same definition, with each instance maintaining its own state and data [1][2].

TIVIDY therefore separates:

* **Workflow Definition** – the reusable blueprint of a workflow.
* **Workflow Instance** – one running execution of that definition.

The definition contains the structure, rules, roles and data requirements. The instance contains its current state, actual data, assignments, decisions and history.

Workflow definitions are versioned. Existing instances remain associated with the definition version from which they were created. Changes to a workflow definition therefore create a new version rather than unexpectedly changing existing running instances [5][6].

## Work Items, Hierarchy and Dependencies

A workflow consists of activities or work items that may be organised into stages and sub-processes [1][2].

BPMN also supports sub-processes and reusable call activities [7]. Workflow patterns identify common control-flow structures such as:

* Sequential execution.
* Choice and conditional routing.
* Parallel execution.
* Synchronisation and joins.
* Iteration and repetition [3].

TIVIDY therefore requires a hierarchy capable of representing individual work items, stages and nested sub-processes.

Dependencies determine when work is allowed to proceed. A work item may remain blocked until its required dependencies have been completed.

## Participants, Assignment and Routing

Workflow research separates control-flow, data and resource perspectives [3][4].

The resource perspective concerns who performs work and how work is distributed. Assignment may be based on:

* Roles.
* Explicit participants.
* Workload.
* Availability.
* Business rules [4].

Routing and assignment are separate responsibilities:

* **Routing** determines what activity or branch happens next.
* **Assignment** determines who performs the selected work.

TIVIDY therefore allows these policies to vary independently.

## Approval, Rejection, Escalation and Failure

Approval and rejection are workflow outcomes rather than special cases inside the workflow engine.

The workflow may support:

* Approval.
* Rejection.
* Revision and rework.
* Escalation.
* Deadlines.
* Exception handling.
* Cancellation.

In the employee-onboarding scenario, approval begins with the Line Manager. If the request exceeds the manager's authority, it is passed to the Director. If neither has sufficient authority, the request remains pending for HR to resolve.

## Events and External Systems

Workflow events allow other components to react to workflow occurrences.

Examples include:

* Work-item completion.
* Dependency changes.
* Notifications.
* Audit events.
* Monitoring events.
* External-service failures.

External applications are accessed through defined interfaces rather than being embedded directly inside the workflow engine [1][7].

## History, Monitoring and Reporting

Workflow monitoring and administration are separate concerns within the WfMC reference model [1].

TIVIDY therefore records workflow history such as:

* Actions.
* State changes.
* Decisions.
* Timestamps.
* Assignees.
* Notifications.
* Rollback checkpoints.

This information can be used by monitoring, auditing and reporting components.

## Reusability and Configurability

The research identified several responsibilities that should remain configurable:

* Workflow definitions.
* Workflow execution.
* Assignment.
* Routing.
* Approval.
* Escalation.
* Events.
* External integrations.
* History and monitoring.

The workflow engine should remain generic while workflow-specific rules and policies can vary between definitions.

## Design Implications for TIVIDY

The research resulted in the following design implications:

1. Separate workflow definitions from workflow instances.
2. Give each instance its own data, state and history.
3. Represent work using a hierarchy of items, stages and sub-processes.
4. Make assignment, routing, approval and escalation configurable.
5. Give work items state-dependent lifecycles.
6. Represent dependencies so that unavailable work can become blocked.
7. Keep external systems behind interfaces and adapters.
8. Allow events to notify independent components.
9. Support workflow history and appropriate rollback.
10. Keep workflow definitions versioned and immutable once published.

## References

[1] Hollingsworth, D. (1995). *Workflow Reference Model*. Winchester, Hampshire, UK: Workflow Management Coalition.

[2] Workflow Management Coalition. *Terminology & Glossary*. Available at: https://wfmc.org/wp-content/uploads/2022/09/TC-1011_term_glossary_v3.pdf.

[3] van der Aalst, W.M.P., ter Hofstede, A.H.M., Kiepuszewski, B. and Barros, A.P. (2003). *Workflow Patterns*. Distributed and Parallel Databases, 14(1), pp. 5–51. doi:10.1023/a:1022883727209.

[4] Russell, N., ter Hofstede, A.H.M., Edmond, D. and van der Aalst, W.M.P. (2005). *Workflow Resource Patterns*. Queensland University of Technology and Eindhoven University of Technology. Available at: [www.workflowpatterns.com](http://www.workflowpatterns.com).

[5] Camunda.io. (2026). *Versioning Process Definitions*. Camunda 8 Documentation. Available at: https://docs.camunda.io/docs/components/best-practices/operations/versioning-process-definitions/.

[6] Camunda 7 Community. (2026). *Process Instance Migration*. Available at: https://docs.camunda.org/manual/latest/webapps/cockpit/bpmn/process-instance-migration/.

[7] Object Management Group. (2011). *Business Process Model and Notation (BPMN) Version 2.0*.

---

# Task 2: Define the Scenario and System

## Selected Scenario

The selected scenario is **employee onboarding across HR, IT, Finance/Payroll and Training**.

TIVIDY itself is a generic Workflow Management System. Employee onboarding is the selected application scenario used to demonstrate how the workflow engine can coordinate a real process.

The same workflow engine could support other processes by using different workflow definitions, rules, participants and external integrations.

---

## Initial Scope of TIVIDY

TIVIDY stores reusable, versioned workflow definitions and manages independent running workflow instances.

A **workflow definition** describes a process.

A **workflow instance** represents one execution of that process and maintains its own:

* Data.
* Work-item states.
* Assignees.
* Progress.
* Decisions.
* History.

Completing work in one instance does not affect another instance.

Published workflow definitions are treated as immutable. Changes create a new definition version for future instances, while existing instances continue using their selected version.

---

## Main Responsibilities of the Workflow Manager

The workflow manager is responsible for:

* Representing individual work items, stages and nested sub-processes.
* Traversing workflow structures.
* Determining when work becomes available.
* Assigning work to suitable participants.
* Enforcing valid work-item actions.
* Managing completion, rejection, cancellation and escalation.
* Supporting sequential, conditional and parallel workflow execution.
* Managing joins between parallel branches.
* Managing dependencies between work items.
* Applying optional validation, security, priority and auditing behaviour.
* Executing participant actions through consistent commands.
* Maintaining workflow history.
* Supporting appropriate rollback and restoration.
* Publishing workflow events.
* Communicating with external systems through common interfaces.

---

## System Boundaries

TIVIDY controls:

* Work availability.
* Assignment.
* Workflow progression.
* Routing.
* Tracking.
* Approval coordination.
* Escalation.
* Events.
* Workflow history.

TIVIDY does **not** directly perform specialised external work.

For example:

* IT performs account and equipment provisioning.
* Payroll performs employee registration.
* Email services deliver notifications.
* External employee-record systems store employee information.

Organisational approval limits and process rules are supplied through configurable policies.

Participants make approval decisions, while external systems perform their own specialised functions.

---

# Workflow Definitions and Running Instances

| Information | Workflow Definition                                | Running Instance                                                           |
| ----------- | -------------------------------------------------- | -------------------------------------------------------------------------- |
| Identity    | Definition ID, name and version                    | Instance ID and selected definition version                                |
| Work        | Work-item stages, sub-processes and dependencies   | Independent work items, states and progress                                |
| Rules       | Roles, validation, routing, approval and deadlines | Selected branches, evaluated decisions and actual deadlines                |
| Data        | Required input fields and initial setup            | Employee details, documents, review results and service responses          |
| People      | Participant and role requirements                  | Actual assignees and decisions                                             |
| History     | Definition/version information                     | Actions, timestamps, state changes, notifications and rollback checkpoints |

---

## Parts of the System That Can Vary

Different workflow definitions can vary in:

* Work structure.
* Required roles.
* Deadlines.
* Approval limits.
* Validation rules.
* Routing rules.
* Assignment rules.
* External services.
* Optional work-item behaviour.

Routing and assignment vary independently.

**Routing** determines which activity or branch proceeds next.

**Assignment** determines which eligible participant performs the work.

---

# Employee Onboarding Scenario

## Organisation and Environment

The scenario takes place in a medium-sized organisation containing:

* HR.
* IT.
* Finance/Payroll.
* Training.

After an employee accepts an offer, HR creates the onboarding instance.

---

## Process Being Managed

The process coordinates the preparation required for a newly hired employee to join the organisation.

It begins when:

1. The employee accepts the offer.
2. HR creates the onboarding instance.
3. Employee information is submitted and verified.

It ends when:

* Required preparation has been completed.
* HR has completed the final readiness review.
* The external employee record has been successfully updated.

An instance can also end through rejection or cancellation.

Each employee has a separate onboarding instance.

The instance stores information such as:

* Department.
* Job role.
* Work location.
* Start date.
* Resource requirements.

---

## Participants and Responsibilities

| Participant / Role      | Responsibility                                                                                                                                 |
| ----------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| New Employee            | Submit information and documents; correct missing or incorrect details; attend orientation                                                     |
| HR Officer              | Create the instance; verify information; coordinate corrections; review revised plans; perform final readiness review; resolve approval issues |
| Line Manager            | Check approval authority; review required access/resources; approve, revise or reject                                                          |
| Director                | Handle requests outside lower approval authority; approve, revise or reject escalated requests                                                 |
| IT Staff                | Prepare accounts, role-appropriate access and required equipment                                                                               |
| Finance / Payroll Staff | Verify payroll information and confirm payroll registration                                                                                    |
| Training Coordinator    | Arrange the initial orientation briefing and record completion                                                                                 |

---

# Major Pieces of Work

## 1. Prepare the Onboarding Instance

Based on the selected workflow definition version, HR creates the onboarding instance.

The instance stores:

* Employee reference.
* Department.
* Job role.
* Start date.
* Work location.
* Resource requirements.

Required preparation work is then made available and assigned to eligible participants.

---

## 2. Check Information and Obtain Approval

The employee submits the required information.

HR verifies whether the information is complete and valid.

If information is missing or incorrect:

```text
Employee
   |
   v
Correct Information
   |
   v
HR Reviews Information
   |
   v
Information Valid?
```

The correction and review cycle continues until the required information is accepted.

Once the information is accepted, TIVIDY creates and routes the approval request.

---

## 3. Approval and Escalation

Approval begins with the **Line Manager**.

The manager first checks whether they have sufficient approval authority.

```text
Route Request
      |
      v
Line Manager Checks Authority
      |
   +--+--+
   |     |
  Yes    No
   |     |
   v     v
Manager Director
Decision Checks Authority
         |
      +--+--+
      |     |
     Yes    No
      |     |
      v     v
 Director  HR Resolves
 Decision  Approval Issue
```

The first authorised handler makes the decision.

The decision can be:

* **Approve** – departmental preparation begins.
* **Revise** – the plan is returned for correction and review.
* **Reject** – the rejection is recorded and the approval route ends.

If the Line Manager does not have sufficient authority, the request is passed to the Director.

If the Director also lacks sufficient authority, the request remains pending for HR to resolve.

---

## 4. Prepare the Employee in Parallel

After approval, TIVIDY activates the required preparation branches.

The main branches are:

```text
              Approved
                  |
            +-----+-----+
            |     |     |
            v     v     v
           IT   Payroll Orientation
            |     |     |
            +-----+-----+
                  |
                 Join
```

### IT Preparation

IT prepares:

1. Employee account.
2. Required access.
3. Equipment.

The IT preparation is a nested sub-process.

Account creation must be completed before account access can be granted.

### Payroll Preparation

Finance/Payroll:

1. Verifies payroll information.
2. Registers the employee.
3. Provides confirmation of registration.

### Orientation

Training:

1. Schedules the orientation.
2. Conducts the initial briefing.
3. Records completion.

These branches can progress independently.

The workflow does not require separate execution threads merely because the branches are logically parallel.

---

## 5. Conditional Additional Work

Additional tasks may be activated according to workflow conditions.

For example:

* Remote employees may require remote-access preparation.
* On-site employees may require site-access preparation.
* Certain job roles may require additional equipment or access.

Only tasks activated by the selected workflow route contribute to the completion conditions.

---

## 6. Join the Branches and Review Readiness

The required branches join before the final HR review.

All required branches and activated additional tasks must be completed before the join can proceed.

HR then performs the final readiness review.

If a defect is found:

```text
Final Readiness Review
          |
       +--+--+
       |     |
    Defect  Ready
       |     |
       v     v
 Correction Update
```

The affected work is corrected and the workflow returns to the readiness review.

---

## 7. Record the Outcome and Close the Instance

Once HR accepts the required results, the external employee record is updated.

The workflow is only successfully completed once the external system confirms the update.

```text
HR Readiness Review
        |
        v
External Employee Record Update
        |
        v
Confirmation Received
        |
        v
Workflow Completed
```

TIVIDY then publishes a completion event for monitoring, auditing and notification components.

---

# Important Decisions and Routing Points

The employee-onboarding workflow follows these rules:

1. Missing or invalid information is returned to the employee for correction.
2. HR must verify the corrected information before approval.
3. Approval is selected according to resource requirements and configured approval authority.
4. The Line Manager is checked first.
5. Requests outside the manager's authority are passed to the Director.
6. The first authorised approval handler makes the decision.
7. Approval activates departmental preparation.
8. Revision sends the plan back for correction and review.
9. Rejection ends the approval route.
10. If neither manager nor director has sufficient authority, HR resolves the approval issue.
11. IT, Payroll and Orientation run as parallel branches.
12. The join waits for all required branches and activated tasks.
13. HR performs the final readiness review.
14. The external employee-record update must be confirmed before successful closure.
15. Overdue work may trigger reminders, reassignment or escalation depending on configured rules.

---

# Dependencies

Dependencies control whether work can progress.

For example:

```text
Create IT Account
       |
       v
Prepare Account Access
       |
       v
Prepare Equipment
```

A work item cannot progress if its required dependencies are not satisfied.

The `Item` design therefore maintains dependencies between work items.

When dependencies are not satisfied, an item may enter the **Blocked** state.

---

# Failures and Exception Routes

| Condition                               | Workflow Response                                                           |
| --------------------------------------- | --------------------------------------------------------------------------- |
| Missing or invalid information          | Request correction, record the reason and repeat verification               |
| Approval requires revision              | Return the plan for correction and review                                   |
| Approval is rejected                    | Record the reason and stop the approval route                               |
| Assignee unavailable or deadline passed | Keep pending, reassign or escalate                                          |
| Departmental preparation fails          | Keep the final join blocked and retry/correct the affected work             |
| Required external update fails          | Record the response and retry/escalate; successful closure remains blocked  |
| Notification delivery fails             | Record delivery failure and arrange a retry                                 |
| Employee withdraws or HR cancels        | Cancel remaining internal work, retain history and perform required cleanup |

---

# External Systems

TIVIDY communicates with external systems through interfaces and adapters.

External systems include:

* **Email service** – assignment, correction, escalation and completion notifications.
* **Employee-record / legacy contact system** – final employee-record update.
* **IT provisioning system** – account, access and equipment preparation.
* **Payroll system** – employee payroll registration.

TIVIDY coordinates these services and records their results rather than performing their specialised functions itself.

---

# Expected Outcome

A successfully completed onboarding instance records:

* Accepted employee information.
* An approved onboarding plan.
* Completed IT preparation.
* Confirmed payroll registration.
* Completed orientation.
* Completed required additional preparation.
* Successful final readiness review.
* Confirmed external employee-record update.
* Complete workflow history.

Rejected and cancelled instances retain their reasons and the work already performed.

---

# Task 3: UML Activity Diagrams

Three Activity Diagrams are included in the project documentation.

## Activity Diagram 1

The high-level employee-onboarding workflow.

It shows the overall progression from onboarding initiation through approval, preparation, readiness review and completion.

## Activity Diagram 2 – Handling Component Events

This diagram demonstrates how workflow component events are handled.

It focuses on event-driven interactions between workflow components and interested observers such as:

* Dependency handling.
* Notifications.
* Audit logging.
* Monitoring.

## Activity Diagram 3 – Employee Onboarding Approval and Escalation

This diagram provides the detailed approval flow for the employee-onboarding scenario.

It includes:

* Routing the approval request.
* Line Manager authority checking.
* Director escalation.
* Approval.
* Revision.
* Rejection.
* HR resolution when neither authority is sufficient.
* Correction and re-review loops.

The activity diagrams demonstrate:

* Actions.
* Decisions.
* Guards.
* Loops.
* Parallel branches.
* Forks and joins.
* Swimlanes.
* Workflow responsibilities.

---

# Task 4: Design Patterns

TIVIDY uses **10 GoF design patterns**.

| #  | Pattern                 | TIVIDY Application                                                                 |
| -- | ----------------------- | ---------------------------------------------------------------------------------- |
| 1  | Composite               | Represents individual work items, stages and nested sub-processes as a hierarchy   |
| 2  | Iterator                | Traverses workflow structures without exposing their internal representation       |
| 3  | State                   | Manages the lifecycle and valid transitions of work items                          |
| 4  | Strategy                | Provides configurable assignment and routing algorithms                            |
| 5  | Decorator               | Adds optional validation, priority, security and auditing behaviour                |
| 6  | Command                 | Encapsulates workflow actions such as assign, start, complete, reject and escalate |
| 7  | Observer                | Allows dependencies, notifications, auditing and monitoring to react to events     |
| 8  | Memento                 | Stores workflow checkpoints for restoration and rollback                           |
| 9  | Adapter                 | Provides a common interface to external systems                                    |
| 10 | Chain of Responsibility | Handles multi-level approval and escalation                                        |

---

## Composite

### Structure

* **Component:** `WorkComponent`
* **Leaf:** `Item`
* **Composite:** `Stage`, `SubProcess`
* **Client:** `WorkflowInstance`, `WorkflowManager`

### Design Problem

TIVIDY's work is hierarchical. A workflow instance can contain stages, work items and nested sub-processes.

The system should treat individual items and groups of items uniformly.

### Collaboration

`WorkflowInstance` holds the root of a `WorkComponent` tree.

`execute()` can be called on the root and propagated recursively.

`Stage` and `SubProcess` delegate execution to their children, while `Item` performs the actual work.

### Why Appropriate

The Composite pattern directly supports TIVIDY's requirement for hierarchical workflow structures.

---

# Iterator

### Structure

* **Iterator:** `WorkflowIterator`
* **Concrete Iterators:** `DepthFirst`, `BreadthFirst`, `Filtered`
* **Aggregate:** `WorkComponent`
* **Concrete Aggregates:** `Stage`, `SubProcess`, `Item`

### Design Problem

TIVIDY needs to traverse workflow structures without exposing how the internal hierarchy is stored.

### Collaboration

`WorkComponent` provides `createIterator()`.

Different iterators provide different traversal approaches:

* Depth-first.
* Breadth-first.
* Filtered traversal.

### Why Appropriate

The Iterator pattern separates traversal logic from the Composite hierarchy.

---

# State

### Structure

* **Context:** `Item`
* **State:** `State`
* **Concrete States:**

  * `Created`
  * `Available`
  * `Assigned`
  * `InProgress`
  * `Completed`
  * `Rejected`
  * `Cancelled`
  * `Escalated`
  * `Blocked`

### Design Problem

A work item has a complex lifecycle with different valid operations depending on its current state.

### Collaboration

`Item` delegates lifecycle operations to its current `State`.

Examples include:

* `start()`
* `complete()`
* `reject()`
* `escalate()`
* `cancel()`
* `assign()`
* `unassign()`

Each concrete state determines which operations are valid.

The `Blocked` state is used when required dependencies prevent an item from progressing.

### Why Appropriate

The State pattern prevents large conditional statements and localises lifecycle behaviour inside the appropriate state classes.

---

# Strategy

### Structure

**Assignment:**

* **Context:** `Item`
* **Strategy:** `AssignmentStrategy`
* **Concrete Strategies:** `RoleBased`, `WorkloadBased`, `RuleBased`

**Routing:**

* **Context:** `WorkflowInstance`
* **Strategy:** `Routing`
* **Concrete Strategies:** `Sequential`, `Parallel`, `Conditional`

### Design Problem

Different workflow definitions may use different assignment and routing rules.

### Collaboration

`Item` delegates assignment to its selected `AssignmentStrategy`.

`WorkflowInstance` delegates routing decisions to its selected `Routing` strategy.

### Why Appropriate

The Strategy pattern allows assignment and routing algorithms to vary without modifying the main workflow engine.

---

# Decorator

### Structure

* **Component:** `WorkComponent`
* **Concrete Component:** `Item`
* **Decorator:** `ItemDecorator`
* **Concrete Decorators:**

  * `Validation`
  * `Priority`
  * `Audit`
  * `Security`

### Design Problem

Some work items require additional behaviour without requiring a new work-item class.

### Collaboration

`ItemDecorator` wraps a `WorkComponent` and provides the same interface.

Multiple decorators can be combined.

### Why Appropriate

The Decorator pattern allows optional behaviour to be added dynamically while keeping the underlying work-item structure unchanged.

---

# Command

### Structure

* **Command:** `WorkflowCommand`
* **Concrete Commands:**

  * `Assign`
  * `Start`
  * `Complete`
  * `Reject`
  * `Escalate`
* **Receiver:** `Item`, `WorkflowInstance`
* **Invoker:** `CommandInvoker`
* **Client:** `WorkflowManager`

### Design Problem

Workflow actions need to be represented consistently and recorded for history and appropriate rollback.

### Collaboration

Each workflow action is represented by a `WorkflowCommand`.

`CommandInvoker` executes commands and maintains command history.

The command delegates the actual operation to its receiver.

The Command pattern works with Memento to support appropriate undo/rollback behaviour.

### Why Appropriate

Command decouples the object requesting an action from the object performing it and allows actions to be recorded and managed consistently.

---

# Observer

### Structure

* **Subject:** `EventSource`
* **Concrete Subject:** `WorkComponent`
* **Observer:** `Observer`
* **Concrete Observers:**

  * `Dependency`
  * `Notification`
  * `AuditLogger`
  * `Monitoring`

### Design Problem

Several components may need to react when a workflow event occurs.

For example, completing one item may affect the availability of another item.

### Collaboration

When a `WorkComponent` changes state, observers receive an event.

Observers react independently:

* `Dependency` updates dependent work.
* `Notification` sends notifications.
* `AuditLogger` records the event.
* `Monitoring` updates monitoring information.

### Why Appropriate

New observers can be added without modifying the workflow component producing the event.

---

# Memento

### Structure

* **Originator:** `Item`, `WorkflowInstance`
* **Memento:** `WorkflowMemento`
* **Caretaker:** `HistoryCaretaker`

### Design Problem

TIVIDY requires appropriate rollback and restoration of workflow state.

### Collaboration

Before an operation that may need restoration, the originator creates a `WorkflowMemento`.

`HistoryCaretaker` stores the memento.

If restoration is required, the originator restores its previous state from the memento.

`CommandInvoker` works with Memento to support command rollback.

### Why Appropriate

Memento preserves the internal state of workflow objects without exposing their internal implementation to the caretaker.

It is used for workflow state restoration and does not imply that external side effects can always be automatically undone.

---

# Adapter

### Structure

* **Target:** `ExternalSystemInterface`
* **Adapters:**

  * `LegacyCRMAdapter`
  * `PaymentGatewayAdapter`
  * `EmailServiceAdapter`
* **Adaptees:**

  * `LegacyCRM`
  * `PaymentGateway`
  * `EmailService`
* **Clients:** `Item`, `WorkflowManager`

### Design Problem

TIVIDY must communicate with external systems that may expose different interfaces.

### Collaboration

The client communicates through `ExternalSystemInterface`.

The adapter translates TIVIDY's request into the interface required by the external system.

### Why Appropriate

The Adapter pattern prevents external-system-specific interfaces from becoming part of the core workflow model.

---

# Chain of Responsibility

### Structure

* **Handler:** `ApprovalHandler`
* **Concrete Handlers:**

  * `ManagerApproval`
  * `DirectorApproval`
  * `VPApproval`
  * `EscalationHandler`
* **Clients:** `Item`, `WorkflowManager`

### Design Problem

Some approval requests require multiple levels of authority.

### Collaboration

An approval request is passed through the approval chain.

The Line Manager is checked first.

If the manager has sufficient authority, the manager handles the request.

If not, the request is passed to the Director.

If the Director also lacks authority, the request is passed to the appropriate escalation/resolution mechanism.

In the selected employee-onboarding scenario, the demonstrated chain is:

```text
Line Manager
     |
     | insufficient authority
     v
Director
     |
     | insufficient authority
     v
HR Resolution
```

### Why Appropriate

The Chain of Responsibility pattern supports configurable multi-level approval and escalation without requiring the workflow manager to contain all approval-level logic.

---

# Task 5: UML Class Diagram

The UML Class Diagram documents the static structure of TIVIDY.

The class diagram includes the main workflow concepts and their relationships, including:

* Workflow definitions.
* Workflow instances.
* Work components.
* Items.
* Stages.
* Sub-processes.
* States.
* Assignment strategies.
* Routing strategies.
* Commands.
* Observers.
* Mementos.
* External-system adapters.
* Approval handlers.
* Dependencies.

The current class diagram is stored in the repository as both an image and a Visual Paradigm project file.

---

# Task 6: Runtime Behaviour Diagrams

## Sequence Diagram 1 – Linking to External Services

This sequence diagram demonstrates how TIVIDY communicates with external systems through an interface and adapter.

It demonstrates the separation between the workflow manager and external-system-specific implementations.

## Sequence Diagram 2 – Execute Parallel Work

This sequence diagram demonstrates how TIVIDY activates multiple independent workflow branches and waits for the required branches to complete before continuing.

The scenario uses:

* IT preparation.
* Payroll preparation.
* Orientation.

The branches eventually join before the final readiness review.

## State Diagram

The State Diagram represents the lifecycle of a workflow item.

The states include:

* Created.
* Available.
* Assigned.
* In Progress.
* Completed.
* Rejected.
* Cancelled.
* Escalated.
* Blocked.

The `Blocked` state represents an item that cannot currently progress because required dependencies have not been satisfied.

---

# Task 7: Design Decisions and Revisions

## Design Decisions

### DD-01 – Separate Workflow Definitions from Instances

Workflow definitions are reusable and versioned, while running instances contain their own data, state and history.

**Reason:** This prevents one running instance from affecting another and allows workflow definitions to evolve safely.

### DD-02 – Use Composite for Workflow Hierarchy

Work items, stages and sub-processes are represented through a common hierarchy.

**Reason:** The workflow is not flat and must support nested work.

### DD-03 – Use Strategy for Assignment and Routing

Assignment and routing behaviour can vary independently.

**Reason:** Different workflow definitions may require different policies.

### DD-04 – Use State for Work-Item Lifecycle

Work items use explicit states and state-specific behaviour.

**Reason:** This prevents invalid actions and avoids large conditional statements.

### DD-05 – Use Parallel Workflow Branches

IT, Payroll and Orientation can progress independently after approval.

**Reason:** These activities do not necessarily depend on one another and can therefore be represented as parallel branches.

### DD-06 – Keep External Systems Behind Interfaces

External services are accessed through common interfaces and adapters.

**Reason:** The workflow engine should not depend directly on a specific external implementation.

### DD-07 – Represent Work-Item Dependencies Explicitly

Items maintain references to their dependencies.

**Reason:** A work item cannot always progress until its required dependencies have been completed. This also supports the `Blocked` state.

---

# Revisions

## Revision 1 – Adding the Blocked State

### Problem

The original design did not adequately represent work items that cannot progress because required dependencies have not been completed.

### Change

The `Blocked` state was added to the State pattern.

The following functions were also introduced:

* `evaluateReadiness()`
* `dependencySatisfied()`

### Reason

The change provides the necessary interface for determining whether an item is ready to progress.

### UML Updates

* `Blocked` was added as a concrete state.
* `dependencySatisfied()` was added to `State`.
* `evaluateReadiness()` was added to `State`.
* `evaluateReadiness()` was added to `Item`.

---

## Revision 2 – Adding Missing Functions to Item

### Problem

The original class design did not contain all functions required for the selected design patterns to interact correctly with `Item`.

### Missing Functionality

The original design did not contain:

* An accessor for `currentState`.
* A mutator for `currentState`.
* Assignment functionality.
* Unassignment functionality.
* A function to initiate the approval chain.
* Memento save functionality.
* Memento restore functionality.

### Change

The following functions were added:

```text
getState()
setState()
assign(person : Participant)
unassign()
save()
restore(memento : WorkflowMemento)
```

### Reason

The selected patterns require `Item` to interact with State, Strategy, Command, Chain of Responsibility and Memento.

---

## Revision 3 – Adding Missing State Functions

### Problem

The State class did not contain the functions required to support assignment and unassignment.

### Change

The following functions were added:

```text
assign(item : Item*, person : Participant*)
unassign()
```

### Reason

The functions allow the Assigned state to correctly manage assignment behaviour.

---

## Revision 4 – Adding Dependencies to Item

### Problem

The original design represented dependency behaviour through the Observer pattern but did not explicitly store the dependencies on the `Item` itself.

### Change

The `Item` class was updated to maintain a collection of dependencies:

```text
vector<Item*> dependencies
```

The following functions were added:

```text
addDependency(item : Item*)
removeDependency(item : Item*)
```

### Reason

A work item cannot progress if its required dependencies have not been satisfied. Explicit dependency storage allows TIVIDY to determine whether work is ready to proceed.

---

# Task 8: GitHub Workflow

The project is maintained using Git and GitHub.

The team uses GitHub to:

* Store the project documentation.
* Store UML diagrams.
* Store Visual Paradigm project files.
* Track changes.
* Record contributions.
* Maintain the project history.

Team members should use meaningful commits and keep the repository organised.

---

## Current Team Contribution Statement

| Team Member     | Current Contribution                                  |
| --------------- | ----------------------------------------------------- |
| Shanna Reinecke | Workflow Management System research and documentation |
| Chloe Larsen    | UML Class Diagram                                     |
| Caleb Jennings  | To be completed                                       |
| Anchen Kruger   | To be completed                                       |
| Caitlin Moodley | To be completed                                       |

The contribution statement should be updated as additional work is committed to the repository.

---

# Repository Structure

The current repository structure is:

```text
COS214-Project/
│
├── .gitignore
├── README.md
│
└── docs/
    ├── Scenario+System.md
    │
    ├── umlImages/
    │   ├── Activity Diagram 2.jpg
    │   ├── Activity Diagram 3.jpg
    │   ├── Class Diagram.jpg
    │   ├── Sequence Diagram 1.jpg
    │   ├── Sequence_Diagram_2.jpg
    │   └── State diagram .jpg
    │
    └── visualParadigm/
        ├── Activity Diagram 2.vpp
        ├── Activity_Diagram_3.vpp
        ├── Class Diagram.vpp
        ├── Sequence Diagram 1.vpp
        ├── Sequence_Diagram_2.vpp
        └── StateDiagram.vpp
```

The repository currently focuses on the **research, scenario definition and UML design** for TIVIDY.

There is currently no `src/` implementation directory in the repository, so the README does not claim that an implementation structure exists.

---

# Documentation Files

| File / Location           | Purpose                                               |
| ------------------------- | ----------------------------------------------------- |
| `README.md`               | Overall project documentation and submission overview |
| `docs/Scenario+System.md` | Detailed scenario and system definition               |
| `docs/umlImages/`         | Exported UML diagrams                                 |
| `docs/visualParadigm/`    | Editable Visual Paradigm UML project files            |

---

# Project Deliverables

| Task   | Deliverable                                               |
| ------ | --------------------------------------------------------- |
| Task 1 | Workflow Management System research and references        |
| Task 2 | Employee onboarding scenario and TIVIDY system definition |
| Task 3 | Three UML Activity Diagrams                               |
| Task 4 | Ten GoF design patterns and their TIVIDY applications     |
| Task 5 | UML Class Diagram                                         |
| Task 6 | Two Sequence Diagrams and one State Diagram               |
| Task 7 | Design decisions and revision history                     |
| Task 8 | GitHub workflow and team contribution statement           |

---

# Current UML Files

The repository currently contains the following UML artefacts:

### Activity Diagrams

* `docs/umlImages/Activity Diagram 2.jpg`
* `docs/umlImages/Activity Diagram 3.jpg`
* `docs/visualParadigm/Activity Diagram 2.vpp`
* `docs/visualParadigm/Activity_Diagram_3.vpp`

### Class Diagram

* `docs/umlImages/Class Diagram.jpg`
* `docs/visualParadigm/Class Diagram.vpp`

### Sequence Diagrams

* `docs/umlImages/Sequence Diagram 1.jpg`
* `docs/umlImages/Sequence_Diagram_2.jpg`
* `docs/visualParadigm/Sequence Diagram 1.vpp`
* `docs/visualParadigm/Sequence_Diagram_2.vpp`

### State Diagram

* `docs/umlImages/State diagram .jpg`
* `docs/visualParadigm/StateDiagram.vpp`

---

# Project Status

## Research and Scenario

* [x] Workflow Management System research
* [x] References documented
* [x] Employee onboarding scenario selected
* [x] TIVIDY system scope defined
* [x] Workflow definition and instance distinction defined
* [x] Main workflow responsibilities identified
* [x] Approval and escalation process defined
* [x] Parallel preparation process defined
* [x] Failure and exception routes defined

## Design Patterns

* [x] Composite
* [x] Iterator
* [x] State
* [x] Strategy
* [x] Decorator
* [x] Command
* [x] Observer
* [x] Memento
* [x] Adapter
* [x] Chain of Responsibility

## UML

* [x] Activity Diagram 2
* [x] Activity Diagram 3
* [x] Class Diagram
* [x] Sequence Diagram 1
* [x] Sequence Diagram 2
* [x] State Diagram
* [ ] Activity Diagram 1 added to repository, if required

## Documentation

* [x] Design decisions documented
* [x] Revision history documented
* [x] Repository structure documented
* [x] Team members documented
* [ ] Remaining team contributions completed
* [ ] Final documentation review
* [ ] Final PDF prepared

---

# Final Demonstration

**Date:** 2 November 2026

The final demonstration will present:

* Workflow Management System research.
* TIVIDY's generic workflow-management purpose.
* Employee onboarding scenario.
* System boundaries and responsibilities.
* Workflow definitions and instances.
* Ten selected GoF design patterns.
* UML Activity Diagrams.
* UML Class Diagram.
* UML Sequence Diagrams.
* UML State Diagram.
* Design decisions and revisions.
* GitHub repository and development history.
* Team contributions.

---

# Project Repository

**GitHub Repository:** Chloe-Larsen/COS214-Project

---

# COS 214 – TIVIDY

**Workflow Management System**

**Research, Scenario and UML Design**

**2026**
