**Task 2: Define the Scenario and System**

Our chosen scenario is employee onboarding across HR, IT, Finance and Training. TIVIDY coordinates the activities needed to prepare a new employee for work. The same engine can support other processes through different work structures, participants and assignment strategies.

**Initial scope of TIVIDY**

TIVIDY organises a workflow as a tree of work components. An Item represents an individual activity, while an ItemGroup contains items or other groups. Both use the WorkComponent interface so that operations can be applied to individual activities or forwarded through a group.

The WorkflowManager coordinates the root work component, registered workers, assignment strategy, traversal and command execution. Builders support the construction of items and groups, while cloning allows existing work structures to be reused and configured for similar processes.

**Main responsibilities of the workflow manager**

The initial system will support the following:

- Represent individual work items, stages and nested sub-processes using items and groups.
- Construct work with appropriate names, workers, deadlines, priorities and dependencies.
- Traverse the work hierarchy and filter items by status or assigned worker.
- Keep dependent work blocked until its required approvals have been received.
- Assign work explicitly or select workers using role-based or workload-based assignment strategies.
- Support sequential activities and independent branches through dependencies between work items.
- Enforce state-dependent progression, blocking and cancellation.
- Support review, rejection and escalation through a worker and supervisor hierarchy.
- Execute assignment, progression, rejection, escalation and cancellation through commands, retaining an in-memory command history.
- Allow events to be published to dependency, notification and audit observers.
- Support email notification through a common interface and an adapter to a legacy email service.

**System boundaries**

TIVIDY controls work organisation, availability, assignment, progression and tracking. The organisation determines the required activities, responsibilities and review levels. Workers perform the activities, make review decisions and handle exceptions.

Departmental systems remain responsible for their specialised operations. For example, IT creates accounts, Finance registers employees for payroll, and HR maintains employee records. The responsible workers confirm the results of those activities in TIVIDY.

**Workflow information**

| **Element** | **Information and responsibility** |
|---|---|
| **WorkflowManager** | Coordinates the root work component, creation time, registered workers, assignment strategy and command history. |
| **Item** | Represents an individual activity with a name, deadline, priority, assigned worker, current state, outstanding dependency count and rejection information. |
| **ItemGroup** | Organises child work components, which may be individual items or further groups. |
| **Worker** | Represents a participant with an identity, name, role, supervisor, subordinates and an escalation queue. |
| **Event** | Identifies a workflow occurrence through its type, source item and timestamp. |

The item states are Blocked, Available, InProgress, Approved, Completed and Cancelled. Assignment is recorded separately from the lifecycle state. Rejection records a flag and reason, while escalation is tracked separately and can transfer responsibility to a supervisor.

**Parts of the system expected to vary**

Different processes can use different work hierarchies, dependencies, participants, deadlines and priorities. Assignment can vary between role-based and workload-based strategies. Traversal can use dependency or priority iterators, with filtering by status or assignee.

Notification, dependency and audit observers can be attached where required. The email service implementation can also change behind its common interface.

**Employee Onboarding Scenario**

**Organisation and environment**

The scenario takes place in a medium-sized organisation with HR, IT, Finance and Training departments. After an employee accepts an offer, HR verifies their information, obtains approval for the onboarding plan and coordinates departmental preparation.

**Process being managed**

The process coordinates the preparation required for a newly hired employee to join the organisation. It begins when HR creates the employee’s onboarding work tree. It ends successfully when the required preparation, reviews and employee-record update have been confirmed. If onboarding must stop, HR cancels the remaining active work.

The employee’s department, job role, location, start date and resource requirements guide the selection and configuration of onboarding activities.

**Participants and responsibilities**

| **Participant or role** | **Responsibility** |
|---|---|
| **New employee** | Submit requested information and documents, correct missing or incorrect details, and attend orientation. |
| **HR officer** | Create and organise onboarding work, verify information, coordinate corrections and perform the final readiness review. |
| **Line manager** | Review the onboarding plan, confirm required access and resources, and handle reviews or escalations assigned to their level. |
| **Director** | Handle reviews and escalations passed to their level through the supervisor hierarchy. |
| **IT staff** | Prepare accounts, role-appropriate access and equipment, and confirm the results. |
| **Finance or payroll staff** | Verify payroll information, register the employee in the departmental system and confirm completion. |
| **Training coordinator** | Arrange the initial orientation briefing and record its completion. |

**Major pieces of work**

**1. Create onboarding work**

HR creates the onboarding group and its required activities. Builders configure item names, workers, deadlines, priorities and observers. Similar existing items or groups may be cloned and adjusted for the employee.

**2. Verify employee information**

The employee supplies the requested information and documents. HR checks that the information is complete and correct.

Missing or incorrect information leads to a recorded rejection reason and a request for correction. HR coordinates another review after the employee provides the corrected information. Dependent work remains blocked until the required approval has been obtained.

**3. Review the onboarding plan**

The onboarding plan identifies the required access, equipment and other preparation. Review requests pass through the worker and supervisor hierarchy for the required number of approval levels.

Each required reviewer makes a decision. A rejection stops the review chain and records a reason. HR then coordinates corrections or decides whether onboarding should be cancelled. Requests that cannot be resolved through the available hierarchy require escalation and follow-up.

**4. Carry out departmental preparation**

Once the relevant approval dependencies have been satisfied, departmental preparation can proceed through independent branches:

- **IT preparation:** Create the required account, configure access and prepare equipment. Account approval is a prerequisite for the dependent access activity.
- **Payroll preparation:** Verify payroll information, register the employee in the departmental system and confirm the result.
- **Orientation:** Schedule and complete an initial briefing that can take place independently of account preparation.

HR includes any additional activities required for the employee. For example, remote employees may require remote-access preparation, while on-site employees may require site-access preparation.

**5. Check final readiness**

HR’s final readiness item depends on approval of the required departmental preparation items. Approval of the IT branch alone is insufficient; payroll, orientation and any additional required work must also satisfy their approval dependencies.

HR checks the departmental results and confirms that the required activities have reached their completion stage. Problems are returned to the responsible department for correction and another review.

**6. Confirm the employee-record update**

After the readiness review is accepted, HR updates the employee’s organisational record outside TIVIDY and confirms the corresponding final task.

Onboarding is considered successful when all required preparation, reviews and final activities have been confirmed.

**Important decisions and dependencies**

HR selects the required tasks when constructing the onboarding tree. The employee’s circumstances determine which additional activities are included. Only the selected activities become prerequisites for later work.

Information verification precedes approval of the onboarding plan. Plan approval precedes departmental preparation. Within IT preparation, account approval precedes the dependent access activity. The final readiness item depends on the required departmental approvals. Independent branches can remain active at the same time. When ItemGroup forwards a progress operation, it calls its children in order.Dependencies determine whether work can proceed, while the assignment strategy selects the worker responsible for it.

**Reviews and escalation**

Workers are linked to supervisors. A review request can require approval through several levels of this hierarchy. Each required reviewer must approve the request for the chain to succeed. An explicit rejection stops the chain and records its reason.

If the required review cannot be completed through the available hierarchy, the request requires escalation and follow-up. A supervisor handling an escalation can take responsibility for the item and place it in their escalation queue. Escalation and Rejection are recorded separately from the item’s lifecycle state. HR decides whether the work should be corrected or the remaining onboarding activities cancelled.

Deadlines record target dates. HR or another responsible participant identifies overdue work and arranges reassignment or escalation where appropriate.

**Failures and exception routes**

| **Condition** | **Workflow response** |
|---|---|
| **Missing or invalid information** | Record the reason, request correction and arrange another review before dependent work proceeds. |
| **Approval requires revision** | Return the work for correction and submit it for review again. |
| **Approval is finally rejected** | Retain the rejection reason. HR decides whether to stop onboarding and cancel the remaining active work. |
| **Required review level is unavailable** | Escalate the item for follow-up through the worker hierarchy. |
| **Assignee is unavailable or a deadline has passed** | Arrange reassignment or escalation as appropriate. |
| **Departmental preparation fails or is incomplete** | Correct the departmental problem and obtain the required approval before downstream work proceeds. |
| **Required external update fails** | Keep the corresponding activity unfinished while the responsible department resolves the problem and confirms the update. |
| **Notification problem occurs** | Investigate the email service separately and keep the work-item outcome based on the actual work performed. |
| **Employee withdraws or HR cancels** | Cancel remaining active work where permitted by its state and arrange departmental cleanup of resources already created. |

**External systems**

The Notification observer uses the EmailService interface to produce assignment, completion, approval, rejection and escalation notices. EmailServiceAdapter translates the subject and body into the HTML format accepted by LegacyEmailService. The current legacy service demonstrates delivery by writing the message to console output.

**Expected outcome**

Successful onboarding leaves the employee’s information accepted, the onboarding plan reviewed and approved, IT preparation confirmed, payroll registration confirmed, orientation completed, and the final readiness review and employee-record update finished. 

The work tree shows the activities and their outcomes. Assigned workers and rejection reasons provide context for follow-up. Commands executed through WorkflowManager remain in its in-memory history, while published events can produce timestamped audit output.

If onboarding stops, the remaining active work is cancelled where permitted by its state. Departments remain responsible for cleaning up any accounts, equipment or registrations already created.