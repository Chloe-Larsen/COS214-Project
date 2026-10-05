# Task 2: Define the Scenario and System

Our chosen scenario is employee onboarding across HR, IT, Finance and Training. The same engine will support other processes through different workflow definitions and configurable rules.

## Initial scope of TIVIDY

TIVIDY will store reusable, versioned workflow definitions and manage independent running instances. A definition describes a process and its rules. An instance represents one execution with its own data, state, assignees, progress and history.

### Main responsibilities of the workflow manager

The initial workflow manager will support the following:

- Represent individual work items, stages and nested sub-processes, and traverse them for progress and pending-work views.
- Make work available when its required prerequisites and routing conditions are satisfied, then assign it to eligible participants using configurable rules.
- Enforce valid work-item actions and manage completion, rejection, cancellation and escalation.
- Support sequential work, conditional routing, parallel branches, joins and rework loops.
- Apply optional validation, security, priority and auditing behaviour to selected work items.
- Execute participant actions through a consistent interface, retain history and support undo or rollback for eligible internal changes.
- Publish events for dependency updates, notifications, auditing and monitoring.
- Exchange data with external systems through a common interface.

### System boundaries

TIVIDY controls work availability, assignment, progression and tracking. Organisational approval limits and process rules are supplied through configurable policies. Participants make approval decisions, while external systems perform their own functions, such as creating accounts or registering employees for payroll.

### Workflow definitions and running instances

| Information | Workflow definition | Running instance |
|---|---|---|
| **Identity** | Definition ID, name and version. | Instance ID and reference to the selected definition version. |
| **Work** | Work-item templates, stages, sub-processes and dependencies. | Independent work items with their current states and completion progress. |
| **Rules** | Required roles, validation rules, routing conditions, approval rules and deadline settings. | Selected branches, evaluated decisions and actual deadlines. |
| **Data** | Required input fields and initial configuration. | Employee details, submitted documents, review results and service responses. |
| **People** | Roles and participant eligibility requirements. | Actual assignees, decisions and workload information. |
| **History** | Definition revision information. | Actions, timestamps, state changes, notifications and rollback checkpoints. |

Completing work in one instance leaves other instances unchanged. Published definitions remain unchanged. Any edits create a new version for future instances, while existing instances retain their original version.

### Parts of the system expected to vary

The structure of work, required roles, deadlines, approval limits, validation rules and external services will vary between different definitions. Optional behaviour can be attached to selected work items. Traversal may visit the whole hierarchy or selected work. Event subscribers can change independently of the workflow model.

Routing and assignment vary independently. Routing determines which activity or branch proceeds next using sequential, parallel or conditional rules. Assignment selects an eligible participant using role, business-rule or workload rules. Different definition versions can also select compatible families of work components, routing policies and approval handlers without changing the main coordinator.

## Employee onboarding scenario

### Organisation and environment

The scenario takes place in a medium-sized organisation with HR, IT, Finance and Training departments. After an employee accepts an offer, HR verifies their information, obtains approval for the onboarding plan and coordinates departmental preparation.

### Process being managed

The process coordinates the preparation required for a newly hired employee to join the organisation. It begins after the employee accepts an offer and HR opens an onboarding record. It ends when the required preparation and final record update are confirmed, or when the instance is rejected or cancelled.

Instance data includes the employee reference, department, job role, work location, start date and resource requirements. Each employee has a separate instance of the onboarding definition. The engine handles generic work items and policies.

### Participants and responsibilities

| Participant or role | Responsibility |
|---|---|
| **New employee** | Submit requested information and documents, correct missing or incorrect details, and attend orientation. |
| **HR officer** | Create the instance, verify information, coordinate corrections and perform the final readiness review. |
| **Line manager** | Confirm the onboarding plan, required access and resources, and approve requests within their authority. |
| **Director** | Handle requests beyond the manager's approval authority or escalations assigned to their level. |
| **IT staff** | Prepare accounts, role-appropriate access and required equipment. |
| **Finance or payroll staff** | Verify supplied payroll information and confirm payroll registration. |
| **Training coordinator** | Arrange the initial orientation briefing and record its completion. |

### Major pieces of work

1. **Prepare the instance for onboarding.** HR selects a definition version, creates the instance and stores the employee reference, department, job role, work location, start date and resource requirements. Required preparation items become available and are assigned to eligible participants.

2. **Check information and obtain approval.** The employee supplies the requested information, and HR checks that it is complete and valid. Missing or incorrect information returns to the employee for correction and another HR review. The onboarding plan proceeds to an authorised approver only after the required information is accepted. Approval starts departmental preparation, a revision request returns the plan for correction, and final rejection ends the instance.

3. **Prepare the employee in parallel.** After approval, the workflow activates three independent branches that can progress while the others remain active:

   - **IT preparation:** Prepare the required account, access and equipment. This is a nested sub-process whose internal activities have their own dependencies. Account creation must finish before account access is granted.
   - **Payroll preparation:** Verify the submitted payroll information and obtain confirmation of registration.
   - **Orientation:** Schedule and complete the initial orientation briefing. The briefing can occur independently of account preparation.

4. **Join the branches and review readiness.** The three branches join before HR's final review. IT preparation, payroll registration, orientation and any additional work activated by the selected route must be complete. HR checks the recorded results. A defect returns to the responsible department for correction and another review.

5. **Record the outcome and close the instance.** Once HR accepts the required results, the external employee record is updated. Confirmation of that update allows TIVIDY to mark the instance complete and publish an event for monitoring, auditing and completion notices.

### Important decisions and routing points

- If required information is missing or invalid, route it back to the employee for correction and another HR review. Accepted information proceeds to approval.
- Use the plan's resource requirements and configured approval authority to select the approver. An authorised approval proceeds, a revision request loops back for correction, and final rejection ends the instance.
- Use work location and job role to activate additional tasks. For example, remote work requires remote-access preparation, while on-site work requires site-access preparation. Only selected tasks contribute to the completion conditions.
- IT, payroll and orientation run as parallel workflow branches. Their join waits for every required branch and any activated additional tasks. Parallel progression does not require separate execution threads.

### Approvals and dependencies

The approval chain begins with the manager. A request outside the manager's authority passes to the director. The first handler with sufficient authority decides the request. An explicit rejection ends the approval route. If neither handler has sufficient authority, the request remains pending for HR to resolve.

HR verification must finish before approval. Approval must occur before departmental preparation. All required branches and any activated additional tasks must finish before HR's final readiness review. The employee-record update must be confirmed before successful closure. Overdue work may trigger reminders, reassignment or escalation according to the rules attached to the work.

### Failures and exception routes

| Condition | Workflow response |
|---|---|
| **Missing or invalid information** | Request correction, record the reason and repeat verification before approval. |
| **Approval requires revision** | Return the plan for correction and review it again. |
| **Approval is finally rejected** | Record the reason, stop pending downstream work and retain the rejected outcome. |
| **Assignee is unavailable or deadline has passed** | Keep the work pending, reassign it or escalate it according to the configured policy. |
| **Departmental preparation fails or is incomplete** | Keep the final join blocked and retry or correct the affected work. |
| **Required external update fails** | Record the response and retry or escalate the update. Keep successful closure blocked until confirmation is received. |
| **Notification delivery fails** | Record the delivery failure and arrange a retry without reversing completed business work. |
| **Employee withdraws or HR cancels** | Cancel remaining internal work, retain the history and arrange cleanup of external resources already created. |

Undo or rollback preserves the audit history and applies only to eligible internal changes. External actions require separate cleanup or compensation, such as deactivating an account created before cancellation.

### External systems

An email service sends assignment, correction, escalation and completion notices. An employee-record or legacy contact-record system receives the final onboarding update. IT provisioning and payroll registration supply confirmation or failure results. Adapters allow these different systems to use a common interface with TIVIDY.

### Expected outcome of the workflow

A successfully completed instance records accepted employee information, an approved onboarding plan, completed IT preparation, confirmed payroll registration, completed orientation and a confirmed employee-record update. Its history explains who performed each action and when. Rejected and cancelled instances retain their reasons and the work already performed.
