# TIVIDY – Workflow Management System

## COS 214 – Practical 6

**Project:** TIVIDY – Workflow Management System
**Module:** COS 214
**Year:** 2026
**Language:** C++11
**Team Size:** 5 members


**GitHub Repository:**
`<INSERT GITHUB REPOSITORY LINK>`

---

# 1. Project Overview

TIVIDY is a **generic Workflow Management System (WfMS)** designed to model, manage and execute structured workflows.

The system separates the definition of a workflow from the individual running instances created from that definition. This allows the same workflow engine to support different business processes while keeping each running instance independent.

For this practical, the selected application scenario is **employee onboarding across HR, IT, Finance and Training departments**.

The employee-onboarding scenario is an application of TIVIDY rather than the definition of TIVIDY itself. The underlying workflow engine is intended to support other workflow scenarios through different workflow definitions, rules, participants and integrations.

---

# 2. Project Objectives

The main objectives of TIVIDY are to:

* Represent reusable and versioned workflow definitions.
* Create independent running workflow instances from definitions.
* Represent individual work items, stages and nested sub-processes.
* Support sequential, conditional and parallel workflow execution.
* Manage dependencies between activities.
* Determine when work becomes available.
* Assign work to suitable participants according to configurable policies.
* Manage work-item lifecycles and valid state transitions.
* Support approval, rejection, correction, cancellation and escalation.
* Support deadlines and exception handling.
* Allow workflow behaviour to vary through configurable strategies and policies.
* Publish events for notifications, monitoring and auditing.
* Integrate with external systems through common interfaces.
* Maintain workflow history and audit information.
* Support necessary rollback or restoration of workflow state.
* Allow workflow definitions to evolve through versioning.
* Keep the core workflow engine independent from specific business applications.

---

# 3. Workflow Management System Research

Research into Workflow Management Systems showed that a workflow consists of structured activities, rules and control information used to coordinate work.

A Workflow Management System provides the runtime environment that interprets workflow definitions, creates workflow instances and coordinates people and external applications.

The research identified several important concepts that influence the design of TIVIDY:

* Workflow definitions describe the structure and rules of a process.
* Workflow instances represent individual executions of a definition.
* Work items represent units of work within a workflow.
* Sub-processes allow workflows to contain groups of related activities.
* Dependencies determine when activities may proceed.
* Assignment determines which participant performs a work item.
* Routing determines which activity or branch executes next.
* Events allow other components to react to workflow occurrences.
* External systems can be integrated through defined interfaces.
* History and monitoring provide information about workflow execution.
* Versioning allows definitions to change without unexpectedly changing running instances.

The research therefore supports separating **workflow definition**, **workflow execution**, **work management**, **routing**, **assignment**, **events**, **integration** and **monitoring** into appropriate responsibilities.

Detailed research and references are provided in the project documentation.

---

# 4. Selected Scenario – Employee Onboarding

## 4.1 Scenario Description

The selected scenario is employee onboarding in a medium-sized organisation containing:

* HR
* IT
* Finance/Payroll
* Training

The workflow starts after an employee accepts an offer and HR creates an onboarding instance.

The workflow coordinates the preparation required for the employee to join the organisation.

A successful workflow ends when:

1. Employee information has been accepted.
2. The onboarding plan has been approved.
3. Required IT preparation has been completed.
4. Payroll registration has been confirmed.
5. Orientation has been completed.
6. Any additional required work has been completed.
7. HR has completed the final readiness review.
8. The external employee record has been successfully updated.

Rejected and cancelled instances retain their history and reasons.

---

# 5. Main Workflow

The employee-onboarding workflow follows the general process below:

```text
Employee accepts offer
        |
        v
HR creates onboarding instance
        |
        v
Employee submits information
        |
        v
HR verifies information
        |
   +----+----+
   |         |
Invalid    Valid
   |         |
   v         v
Correction  Approval
   |         |
   +---------+
             |
             v
      Approval decision
        /      |       \
   Approve   Revise   Reject
      |         |        |
      |         |        v
      |         |      End
      |         |
      |         +----> Correction
      |
      v
Parallel preparation
   /       |        \
  IT    Payroll   Orientation
   \       |        /
    \      |       /
     +-----+------+
           |
           v
    HR readiness review
           |
      +----+----+
      |         |
   Defect      Ready
      |         |
      v         v
 Correction   Update
                 |
                 v
              Complete
```

The workflow also supports additional tasks based on conditions such as work location and job role.

---

# 6. TIVIDY System Scope

TIVIDY is responsible for:

* Workflow definition management.
* Workflow instance creation and tracking.
* Work-item availability.
* Work-item assignment.
* Workflow routing.
* Dependency management.
* Work-item lifecycle management.
* Approval and escalation coordination.
* Event publication.
* Workflow history.
* Integration with external systems.

TIVIDY does **not** directly perform the specialised work of external systems.

For example:

* IT performs account and equipment provisioning.
* Payroll systems perform payroll registration.
* Email services deliver notifications.
* External employee-record systems store employee information.

TIVIDY coordinates these activities and records their results.

---

# 7. Workflow Definitions and Instances

TIVIDY distinguishes between a **Workflow Definition** and a **Workflow Instance**.

### Workflow Definition

A workflow definition is the reusable blueprint for a process.

It contains information such as:

* Workflow ID
* Name
* Version
* Work items
* Stages
* Sub-processes
* Dependencies
* Required roles
* Validation rules
* Routing rules
* Approval rules
* Deadline configuration

### Workflow Instance

A workflow instance represents one execution of a workflow definition.

It contains its own:

* Instance ID
* Current state
* Employee data
* Work-item states
* Actual assignees
* Decisions
* Progress
* History
* Notifications
* Rollback checkpoints

Multiple instances can therefore execute the same definition independently.

A change to one instance does not affect another instance.

Published workflow definitions are treated as immutable. Changes create a new version for future instances, while existing instances continue using their selected definition version.

---

# 8. Major Workflow Responsibilities

## 8.1 Work Items

A work item represents an individual piece of work that must be completed.

Examples include:

* Verify employee information
* Approve onboarding plan
* Create employee account
* Register employee for payroll
* Complete orientation
* Perform final readiness review

Work items have their own lifecycle and can only perform valid actions for their current state.

---

## 8.2 Stages and Sub-processes

Related work items can be grouped into stages or sub-processes.

For example, IT preparation is a nested sub-process containing:

1. Create account.
2. Prepare access.
3. Prepare equipment.

Account creation must be completed before account access can be granted.

This hierarchy allows TIVIDY to represent both individual work and larger groups of work.

---

## 8.3 Routing

Routing determines what happens next in a workflow.

TIVIDY supports:

* Sequential routing.
* Conditional routing.
* Parallel branches.
* Joins.
* Loops.
* Rework.
* Rejection.
* Escalation.

For example, invalid employee information routes back to the employee for correction, while valid information continues to approval.

---

## 8.4 Assignment

Assignment determines which eligible participant should perform a work item.

Assignment can be based on:

* Role.
* Business rules.
* Workload.
* Availability.
* Other configured policies.

Assignment is kept separate from routing because routing determines **what happens next**, while assignment determines **who performs the work**.

---

## 8.5 Events

Events allow components to react to workflow occurrences.

Examples include:

* Work item completed.
* Work item overdue.
* Approval rejected.
* Instance completed.
* Notification required.
* External update failed.

Events can be used by monitoring, notification and auditing components without placing all of this behaviour inside the workflow engine.

---

# 9. Selected Design Patterns

TIVIDY uses ten GoF design patterns to address different responsibilities within the workflow system.

| Pattern                     | TIVIDY Application                                                                                                                |
| --------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| **Composite**               | Represents individual work items and groups/nested sub-processes as a common hierarchy.                                           |
| **Iterator**                | Provides a consistent way to traverse workflow items without exposing the underlying collection structure.                        |
| **State**                   | Manages the lifecycle and valid transitions of work items and workflow instances.                                                 |
| **Strategy**                | Allows assignment and routing policies to vary independently from the workflow engine.                                            |
| **Decorator**               | Adds optional behaviour such as validation, security, priority or auditing to selected work items.                                |
| **Command**                 | Encapsulates workflow actions such as approve, reject, complete, cancel and escalate.                                             |
| **Observer**                | Allows monitoring, notification and auditing components to react to workflow events.                                              |
| **Memento**                 | Stores checkpoints of workflow state to support restoration or rollback where appropriate.                                        |
| **Adapter**                 | Allows TIVIDY to communicate with external systems that have incompatible interfaces.                                             |
| **Chain of Responsibility** | Supports approval escalation by passing a request through authorised handlers until an appropriate handler can make the decision. |
| **Abstract Factory**        | Provides compatible families of workflow-related objects for different workflow definitions or configurations.                    |

The patterns are not included simply to increase the number of patterns. Each pattern addresses a specific design problem within TIVIDY.

---

# 10. Pattern Responsibilities

## Composite

The Composite pattern represents individual work items and groups of work items through a common interface.

This is useful because TIVIDY must support:

* Individual work items.
* Stages.
* Nested sub-processes.
* Groups of related activities.

A workflow can therefore be treated as a hierarchy without the client needing separate logic for individual and grouped work.

---

## Iterator

The Iterator pattern allows TIVIDY to traverse workflow structures without exposing how the underlying collection is stored.

It can be used to:

* Visit workflow items.
* Find pending work.
* Calculate progress.
* Traverse nested workflow structures.

---

## State

The State pattern manages the lifecycle of workflow items.

Example states include:

```text
Pending
   |
   v
Available
   |
   v
Assigned
   |
   v
In Progress
   |
   +---------> Rejected
   |
   +---------> Cancelled
   |
   v
Completed
```

Different states allow or prevent different actions.

---

## Strategy

The Strategy pattern allows assignment and routing algorithms to be changed without modifying the workflow engine.

Examples include:

* Assign by role.
* Assign by workload.
* Assign by business rule.
* Route sequentially.
* Route conditionally.

---

## Decorator

The Decorator pattern allows optional behaviour to be attached to work items without changing the original work-item implementation.

Possible decorators include:

* Validation.
* Security checking.
* Priority handling.
* Auditing.

This supports the requirement that optional behaviour can be applied only where needed.

---

## Command

The Command pattern represents workflow actions as objects.

Examples include:

* Complete work item.
* Approve.
* Reject.
* Cancel.
* Escalate.
* Roll back.

This separates the request for an action from the object that performs it and also supports recording or reversing appropriate operations.

---

## Observer

The Observer pattern allows multiple components to react when workflow events occur.

For example, when an onboarding instance is completed:

* Monitoring can update statistics.
* Auditing can record the event.
* Notification services can send completion notices.

The workflow engine does not need to contain all of this behaviour itself.

---

## Memento

The Memento pattern stores workflow state at appropriate checkpoints.

This can support:

* Rollback.
* Recovery.
* Restoration of workflow state.

Memento is intended for workflow state restoration rather than pretending that every external side effect can automatically be undone.

---

## Adapter

The Adapter pattern allows TIVIDY to communicate with external systems through a common interface.

Examples include adapters for:

* Payroll systems.
* IT provisioning systems.
* Employee-record systems.
* Email services.

This keeps external system-specific APIs outside the core workflow model.

---

## Chain of Responsibility

The Chain of Responsibility pattern is used for approval escalation.

For example:

```text
Approval Request
       |
       v
Line Manager
       |
       | insufficient authority
       v
Director
       |
       | insufficient authority
       v
Pending / HR Resolution
```

Each handler checks whether it has sufficient authority to handle the request.

---

## Abstract Factory

The Abstract Factory pattern can provide compatible families of workflow-related objects based on a selected workflow definition or configuration.

This allows different workflow configurations to create compatible:

* Work-item components.
* Routing components.
* Assignment components.
* Approval components.

The main workflow coordinator does not need to know the concrete classes being created.

---

# 11. UML Design

The UML diagrams document both the structure and runtime behaviour of TIVIDY.

## Activity Diagrams

Three Activity Diagrams are included to demonstrate the workflow at different levels.

### Activity Diagram 1

Provides the high-level employee-onboarding workflow.

### Activity Diagram 2

Provides a more detailed view of workflow coordination, routing, assignment and parallel processing.

### Activity Diagram 3

Provides a detailed view of a selected employee-onboarding process or sub-process.

The activity diagrams demonstrate concepts including:

* Actions.
* Decisions.
* Guards.
* Loops.
* Parallel branches.
* Forks and joins.
* Swimlanes.
* Sub-processes.

---

## Class Diagram

The Class Diagram represents the static structure of TIVIDY.

It identifies:

* Core workflow classes.
* Workflow definitions.
* Workflow instances.
* Work items.
* Composite structures.
* State classes.
* Strategies.
* Commands.
* Observers.
* Adapters.
* Approval handlers.
* Factories.

The class diagram is designed to remain consistent with the selected design patterns and activity/sequence/state diagrams.

---

## Sequence Diagrams

Two Sequence Diagrams demonstrate important runtime interactions.

Examples include:

1. Employee onboarding approval and escalation.
2. Parallel departmental preparation and workflow completion.

The diagrams demonstrate communication between TIVIDY components and external systems.

---

## State Diagram

The State Diagram describes the lifecycle of a workflow item or workflow instance.

It demonstrates valid transitions such as:

```text
Pending
   ↓
Available
   ↓
Assigned
   ↓
In Progress
   ↓
Completed
```

with alternative transitions for:

* Rejection.
* Cancellation.
* Escalation.
* Rework.
* Failure.

---

# 12. Design Decisions

The main design decisions made for TIVIDY include:

### DD-01 – Separate Workflow Definitions from Instances

Workflow definitions are reusable and versioned, while instances contain their own state, data, assignments and history.

**Reason:** This prevents one running workflow from affecting another and allows definitions to evolve safely.

---

### DD-02 – Use Composite for Workflow Hierarchy

Individual work items and groups of work are represented using a common hierarchy.

**Reason:** TIVIDY must support stages and nested sub-processes as well as individual activities.

---

### DD-03 – Use Strategy for Assignment and Routing

Assignment and routing behaviour can vary without modifying the main workflow engine.

**Reason:** Different organisations and workflow definitions may use different policies.

---

### DD-04 – Use State for Work-Item Lifecycle

Work items use explicit states and valid transitions.

**Reason:** This prevents invalid operations and makes workflow behaviour easier to understand and maintain.

---

### DD-05 – Use Parallel Workflow Branches

After onboarding approval, IT, Payroll and Orientation may progress independently before joining at the final readiness review.

**Reason:** These activities do not always depend on one another and therefore do not need to execute sequentially.

---

### DD-06 – Keep External Systems Behind Interfaces

External services are accessed through common interfaces and adapters.

**Reason:** The workflow engine should not depend directly on a particular external system implementation.

---

# 13. Revision History

| Version | Date        | Revision                                 | Reason                                       |
| ------- | ----------- | ---------------------------------------- | -------------------------------------------- |
| 0.1     | 29 Sep 2026 | Initial workflow-management research     | Establish system concepts and terminology    |
| 0.2     | 30 Sep 2026 | Employee onboarding selected as scenario | Provide a concrete application of TIVIDY     |
| 0.3     | Oct 2026    | Initial pattern selection                | Map GoF patterns to TIVIDY responsibilities  |
| 0.4     | Oct 2026    | UML design developed                     | Align system structure and runtime behaviour |
| 1.0     | 6 Oct 2026  | Practical 6 submission                   | Finalise research and initial design         |

*Dates and revisions should be updated to reflect the team's actual Git history.*

---

# 14. GitHub Workflow

The project is maintained using Git and GitHub.

Team members should:

* Work on separate branches where appropriate.
* Create meaningful commits.
* Push work regularly.
* Use descriptive commit messages.
* Open pull requests when team review is required.
* Review and integrate team members' work.
* Keep the main branch in a usable state.

Example commit messages include:

```text
Add workflow management research
Define employee onboarding scenario
Add activity diagram design
Document Composite and Iterator patterns
Add initial class diagram
Add sequence diagrams
Update TIVIDY design decisions
Complete Practical 6 documentation
```

The Git history should demonstrate genuine development over time rather than a single final upload.

---

# 15. Repository Structure

The repository is organised to separate documentation, diagrams and future implementation.

```text
COS214-Project/
│
├── README.md
│
├── docs/
│   ├── research/
│   │   ├── workflow-management-research.md
│   │   └── references.md
│   │
│   ├── diagrams/
│   │   ├── activity-diagrams/
│   │   ├── class-diagram/
│   │   ├── sequence-diagrams/
│   │   └── state-diagrams/
│   │
│   ├── design-decisions/
│   │   └── design-decisions.md
│   │
│   └── pattern-documentation/
│       └── design-patterns.md
│
└── src/
    └── implementation/
```

The `src` directory is reserved for future implementation work where applicable.

---

# 16. Documentation

The project documentation covers the following practical requirements:

| Task   | Documentation                                         |
| ------ | ----------------------------------------------------- |
| Task 1 | Workflow Management System research and references    |
| Task 2 | Employee onboarding scenario and TIVIDY system scope  |
| Task 3 | Three UML Activity Diagrams                           |
| Task 4 | Ten GoF design patterns and their TIVIDY applications |
| Task 5 | UML Class Diagram                                     |
| Task 6 | Two Sequence Diagrams and one State Diagram           |
| Task 7 | Design decisions and revision history                 |
| Task 8 | GitHub workflow and team contribution statement       |

---

# 17. Team Members

| Name     | Student Number     | Role / Contribution |
| -------- | ------------------ | ------------------- |
| `Anchen Kruger` | `u25073703` | `<Contribution>`    |
| `Caitlin Moodley` | `u25128443` | `<Contribution>`    |
| `Caleb Jennings` | `u25173805` | `<Contribution>`    |
| `Chloe Larsen` | `u25004141` | `<Contribution>`    |
| `Shanna Reinecke` | `u25008260` | `<Contribution>`    |


Remove unused rows if the team contains five members.

---

# 18. Team Contribution Statement

Each team member contributed to the research, design and documentation of TIVIDY.

Contributions include:

* Workflow management research.
* Scenario definition.
* GoF design-pattern research and mapping.
* UML activity diagrams.
* UML class diagram.
* UML sequence diagrams.
* UML state diagram.
* Design decisions and revision history.
* GitHub repository management.
* Final documentation and review.

The detailed contribution of each member should be recorded in the final submission.

---

# 19. Project Status

* [x] Workflow Management System research
* [x] Workflow scenario selected
* [x] TIVIDY scope defined
* [x] Workflow definition and instance distinction defined
* [x] Main workflow responsibilities identified
* [x] Initial design patterns selected
* [ ] Activity Diagram 1 completed
* [ ] Activity Diagram 2 completed
* [ ] Activity Diagram 3 completed
* [ ] Class Diagram completed
* [ ] Sequence Diagram 1 completed
* [ ] Sequence Diagram 2 completed
* [ ] State Diagram completed
* [ ] Design decisions documented
* [ ] Revision history finalised
* [ ] Team contribution statement completed
* [ ] Final documentation reviewed
* [ ] GitHub repository finalised
* [ ] Final PDF prepared

---

# 20. Final Demonstration

**Final Demonstration Date:** 2 November 2026

The final demonstration will present the design of TIVIDY, including:

* Workflow Management System research.
* Employee onboarding scenario.
* System scope and responsibilities.
* Selected GoF design patterns.
* UML design.
* Runtime behaviour.
* Design decisions.
* GitHub development history.
* Team contributions.

---

# 21. References

The research supporting TIVIDY is documented in the project's research section.

Key references include:

1. Hollingsworth, D. (1995). *Workflow Reference Model*. Workflow Management Coalition.

2. Workflow Management Coalition. *Terminology & Glossary*. Available from the WfMC documentation.

3. van der Aalst, W.M.P., ter Hofstede, A.H.M., Kiepuszewski, B. and Barros, A.P. (2003). *Workflow Patterns*. Distributed and Parallel Databases, 14(1), pp.5–51.

4. Russell, N., ter Hofstede, A.H.M., Edmond, D. and van der Aalst, W.M.P. (2005). *Workflow Resource Patterns*. Queensland University of Technology and Eindhoven University of Technology.

5. Camunda. *Versioning Process Definitions*. Camunda Documentation.

6. Camunda. *Process Instance Migration*. Camunda Documentation.

7. Object Management Group. (2011). *Business Process Model and Notation (BPMN) Version 2.0*.

Full reference details and links are included in the project research documentation.

---

# 22. Project Repository

**GitHub:** `<INSERT GITHUB REPOSITORY LINK>`

---

## COS 214 – TIVIDY

**Workflow Management System – Research and Initial Design**

**2026**
