#include "../../include/builder/WorkflowDefinition.h"
#include "../../include/WorkflowInstance.h"
#include "../../include/composite/WorkComponent.h"

WorkflowDefinition::WorkflowDefinition(std::string id, std::string name) : id(id), name(name), rootTemplate(nullptr)
{
}

WorkflowInstance *WorkflowDefinition::createInstance()
{
    static int instanceCounter = 1;
    std::string instanceId = id + "-INST-" + std::to_string(instanceCounter++);
    WorkflowInstance *instance;
    if (rootTemplate)
    {
        WorkComponent *clonedRoot = rootTemplate->clone();
        instance->setRoot(clonedRoot);
    }

    return instance;
}

WorkComponent *WorkflowDefinition::getRootTemplate()
{
    return this->rootTemplate;
}

const std::string &WorkflowDefinition::getId() const
{
    return id;
}

const std::string &WorkflowDefinition::getName() const
{
    return name;
}