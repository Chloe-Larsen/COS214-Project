# TIVIDY – Workflow Management System

**One Engine. Two Industries. Connected Workflows.**

TIVIDY is a generic Workflow Management System developed in C++11. It provides a reusable engine for creating, managing, assigning, tracking, and processing workflows.

The system demonstrates how a single workflow engine can support different industries without changing its underlying functionality. It includes a command-line application and a graphical user interface developed using Qt 5.

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
- Provide a graphical interface using Qt 5.
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

### Core Application

- C++11
- GNU Compiler Collection (G++)
- GNU Make

### Graphical User Interface

- Qt 5
- Qt Widgets
- Qt Concurrent
- pkg-config

### Containerisation and Development Tools

- Docker
- GDB – debugging
- Valgrind – memory checking
- Visual Paradigm – UML diagrams

### Installing Dependencies

On Ubuntu or WSL, install the core dependencies:

```bash
sudo apt update
sudo apt install build-essential g++ make
```

To build the GUI, install the additional Qt dependencies:

```bash
sudo apt install pkg-config qtbase5-dev qtbase5-dev-tools
```

Optional development tools:

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

### 2. Build the Core Application

Build the application using the Makefile:

```bash
make program
```

Alternatively, use:

```bash
make
```

The `make` command builds and runs the application.

### 3. Run the Core Application

Run the compiled executable:

```bash
./program
```

Follow the application's prompts to interact with the workflow system.

### 4. Clean the Build

Remove generated build files using:

```bash
make clean
```

## Graphical User Interface (GUI)

TIVIDY includes a graphical user interface developed using Qt 5. The GUI provides a visual way to interact with the workflow management system alongside the command-line application.

### Building the GUI

After installing the Qt dependencies, build the GUI using:

```bash
make -C gui
```

### Running the GUI

After a successful build, launch the GUI:

```bash
./gui/build/tividy-gui
```

### GUI Docker Image

The Dockerfile includes a separate build target for the graphical interface.

Build the GUI image:

```bash
docker build --target gui -t tividy-gui .
```

Run the GUI container:

```bash
docker run --rm tividy-gui
```

**Note:** Displaying the GUI from Docker requires a graphical display environment to be configured. Starting the container alone does not automatically make the GUI window appear on the host machine.

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
- GUI interaction and workflow operations.

### Memory Checking

Valgrind can be used to check for memory leaks:

```bash
make valgrind
```

### Debugging

GDB can be started using:

```bash
make gdb
```

## Docker

The project includes a multi-stage Dockerfile for building and running the core application and graphical interface separately.

### Core Application

Build the core Docker image:

```bash
make docker
```

Alternatively:

```bash
docker build -t tividy .
```

Run the container:

```bash
make docker-run
```

Alternatively:

```bash
docker run --rm tividy
```

### GUI Application

Build the GUI Docker image:

```bash
make docker-gui
```

Alternatively:

```bash
docker build --target gui -t tividy-gui .
```

The GUI container requires a graphical display environment to show its window.

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
├── gui/
│   ├── Makefile
│   ├── src/
│   └── ...
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