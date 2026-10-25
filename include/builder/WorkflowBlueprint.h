#ifndef WORKFLOWBLUEPRINT_H
#define WORKFLOWBLUEPRINT_H

#include <string>
#include <vector>
#include <map>

class Workers;

struct TaskConfig {
    std::string id;
    std::string name;
    std::string stageId;
    int priority;
    std::string deadline;
    std::string role;
    std::string approvalTier;
};

struct StageConfig {
    std::string id;
    std::string name;
    std::string parentStageId;
    std::string routingType;
};

class WorkflowBlueprint {
private:
    std::string id;
    std::string name;
    std::vector<StageConfig> stages;
    std::vector<TaskConfig> tasks;
    std::vector<std::pair<std::string, std::string>> dependencies;
    std::vector<Worker> workers;
    std::vector<Item> items;

public:
    WorkflowBlueprint(const std::string& id, const std::string& name) : id(id), name(name) {}

    std::string getId() const { return id; }
    std::string getName() const { return name; }

    void addStageConfig(const StageConfig& sc) { stages.push_back(sc); }
    void addTaskConfig(const TaskConfig& tc) { tasks.push_back(tc); }
    void addDependencyConfig(const std::string& prereq, const std::string& dep) {
        dependencies.push_back({prereq, dep});
    }

    const std::vector<StageConfig>& getStages() const { return stages; }
    const std::vector<TaskConfig>& getTasks() const { return tasks; }
    const std::vector<std::pair<std::string, std::string>>& getDependencies() const { return dependencies; }
    const std::vector<Worker>& getWorkers() const { return workers;}
    const std::vector<Item>& getItems() const { return items;}
};

#endif // WORKFLOWBLUEPRINT_H