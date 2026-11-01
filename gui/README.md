# TIVIDY GUI
## Build and run

On Ubuntu/WSL:

```bash
sudo apt-get install build-essential pkg-config qtbase5-dev qtbase5-dev-tools
make -C gui -j4
./gui/build/tividy-gui
```
## Using the workspace

1. Choose **New workflow**, or **Open JSON** to load a workflow. Both supplied industry examples are under `resources/examples/`.
2. Add groups/items using the buttons or the tree/graph context menu. Selecting an item offers sibling creation; selecting a group adds children. Right-click empty tree/graph space to add another root.
3. In **Workforce**, add workers and their superiors. Edit and remove workers there. Assigned workers and referenced superiors must be disconnected before removal.
4. Assign a worker directly, or choose **Assignment strategy**. Role-based assignment requires a role and selects its least busy matching worker. Workload-based assignment selects the worker with the least active work, optionally within a role. Applying a strategy assigns eligible unassigned items. Only newly created unassigned items automatically use the selected strategy; existing manual unassignments stay unchanged until you explicitly apply the strategy again. Choose Manual assignment to turn automatic assignment off.
5. **Progress** starts available work. **Request review** records the superior's explicit Approve/Reject decision. **Progress** completes approved work. Reject, Escalate, Cancel and Unassign are available when valid.
6. Add/remove dependencies through the tree or graph. An arrow points from prerequisite to dependent. Dependencies can be changed before the dependent work starts. Group prerequisites wait for every descendant item; group dependents gate their descendant items. Approved or completed prerequisites satisfy a dependency.
7. Configure a notifier or audit logger for a component, group, or entire workflow. Group/workflow settings include future children. Component settings override inherited settings, including when disabled.
8. Notifications, audit entries and adapter output appear in the activity dock. Filter by category/text, inspect an entry, or export the visible rows as CSV.
9. Save and reopen the workflow, including workers, current states, dependencies, strategy, observer settings and activity. Invalid imports leave the current workflow intact.

## Shortcuts

| Action | Shortcut |
|---|---|
| New / Open / Save / Save as | Ctrl+N / Ctrl+O / Ctrl+S / Ctrl+Shift+S |
| Search / Refresh | Ctrl+F / F5 |
| Add item / group | Insert / Ctrl+Shift+G |
| Rename | F2 |
| Assign / Unassign | Ctrl+Shift+A / Ctrl+Shift+U |
| Progress / Review | Ctrl+Enter / Ctrl+R |
| Reject / Escalate / Cancel | Ctrl+Shift+R / Ctrl+E / Delete |
| Add / remove dependency | Ctrl+D / Ctrl+Shift+D |
| Add worker | Ctrl+Shift+W |