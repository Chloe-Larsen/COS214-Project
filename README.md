# TIVIDY – Workflow Management System

**One Engine. Two Industries. Connected Workflows.**

TIVIDY is a generic Workflow Management System developed in C++11. It provides a reusable engine for creating, managing, assigning, tracking, and processing workflows.

The system demonstrates how a single workflow engine can support different industries without changing its underlying functionality.

## Team Members

| Name | Student Number |
|---|---|
| Caleb Jennings | 25173805 |
| Anchen Kruger | 25073703 |
| Chloe Larsen | 25004141 |
| Caitlin Moodley | 25128443 |
| Shanna Reinecke | 25008260 |

## Project Scenarios

TIVIDY demonstrates its functionality through two industry scenarios:

### 1. Aviation – Aircraft Maintenance Dispatch

Manages aircraft maintenance workflows, including maintenance tasks, worker assignments, approvals, dependencies, and task progress.

### 2. Manufacturing – Production Order Management

Manages production workflows, including production tasks, resource assignments, approvals, dependencies, and workflow progress.

Both scenarios use the same underlying workflow engine.

## Features

- Create and manage workflow items.
- Organise tasks into hierarchical groups.
- Assign tasks to workers.
- Track task status and progress.
- Manage task dependencies.
- Support approvals, rejections, cancellations, and escalations.
- Apply different assignment and workflow-routing strategies.
- Notify observers when workflow events occur.
- Integrate with external systems through adapters.
- Support both Aviation and Manufacturing workflows.

## Design Patterns

TIVIDY uses ten GoF design patterns:

| Design Pattern | Purpose |
|---|---|
| Composite | Organises individual items and groups into a hierarchy. |
| Iterator | Traverses workflow items using different iteration approaches. |
| State | Manages workflow item states and state transitions. |
| Strategy | Supports interchangeable assignment and routing strategies. |
| Factory Method | Creates workflow components through builder classes. |
| Prototype | Supports cloning workflow components. |
| Command | Encapsulates workflow actions as command objects. |
| Observer | Notifies interested objects about workflow events. |
| Adapter | Connects the system to external services through compatible interfaces. |
| Chain of Responsibility | Passes approval requests through a chain of workers. |

## Technologies and Dependencies

- C++11
- GNU Compiler Collection (G++)
- GNU Make
- Docker (for containerised execution)

Optional development tools:

- GDB – debugging
- Valgrind – memory checking
- Visual Paradigm – UML diagrams

### Installing Dependencies

On Ubuntu or WSL, install the required compiler and build tools:

```bash
sudo apt update
sudo apt install build-essential g++ make
```

Optional tools:

```bash
sudo apt install gdb valgrind
```

## Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/Chloe-Larsen/COS214-Project.git
```

Navigate into the project directory:

```bash
cd COS214-Project
```

### 2. Build the Project

Build the application using the Makefile:

```bash
make
```

If you want to build the program using the specific program target:

```bash
make program
```

### 3. Run the Application

Run the compiled executable:

```bash
./program
```

Follow the application's prompts to interact with the workflow system.

Select or configure the relevant scenario to demonstrate the Aviation or Manufacturing workflow.

### 4. Clean the Build

Remove generated build files using:

```bash
make clean
```

## Testing

Testing should be performed on both industry scenarios to verify that the shared workflow engine behaves correctly.

### Aviation Testing

Verify that the system can:

- Create aircraft maintenance tasks.
- Group related maintenance tasks.
- Assign tasks to workers.
- Process approvals and rejections.
- Handle task dependencies.
- Track task progress and completion.
- Escalate approval requests when necessary.

### Manufacturing Testing

Verify that the system can:

- Create production order tasks.
- Group related production tasks.
- Assign tasks to workers.
- Process approvals and rejections.
- Handle task dependencies.
- Track task progress and completion.
- Escalate approval requests when necessary.

### General Testing

Also verify:

- Workflow state transitions.
- Assignment and routing strategies.
- Command execution.
- Observer notifications.
- External service adapters.
- Iteration through workflow items.
- Cloning of workflow components.

Optional memory checking can be performed with Valgrind if a suitable Makefile target is available:

```bash
valgrind ./program
```

## Docker

The project includes a Dockerfile for containerised execution.

Build the Docker image:

```bash
docker build -t tividy .
```

Run the container:

```bash
docker run --rm tividy
```

## Project Structure

```text
COS214-Project/
│
├── docs/
│   ├── COS214 - Project.pdf
│   ├── Scenario+System.md
│   ├── TIVIDY-Demo.pptx
│   ├── umlImages/
│   └── visualParadigm/
│
├── include/
├── src/
│
├── Dockerfile
├── Makefile
└── README.md
```

## Documentation

Additional project documentation is available in the `docs/` directory:

- **COS214 - Project.pdf** – Project documentation.
- **Scenario+System.md** – Industry scenarios and system description.
- **TIVIDY-Demo.pptx** – Project demonstration presentation.
- **umlImages/** – UML diagrams.
- **visualParadigm/** – Editable Visual Paradigm diagram files.

## Project Goal

TIVIDY demonstrates how design patterns can be combined to create a flexible and reusable workflow management engine.

By separating the workflow engine from industry-specific scenarios, the system can support different business processes while maintaining the same core functionality.