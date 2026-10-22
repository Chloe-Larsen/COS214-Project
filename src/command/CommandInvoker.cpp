#include "../../include/command/CommandInvoker.h"

CommandInvoker::CommandInvoker()
{
	// TODO - implement CommandInvoker::CommandInvoker
	throw "Not yet implemented";
}

CommandInvoker::~CommandInvoker()
{
}

void CommandInvoker::executeCommand(WorkflowCommand *command)
{
	// TODO - implement CommandInvoker::executeCommand
	throw "Not yet implemented";
}

std::vector<WorkflowCommand *> CommandInvoker::getHistory()
{
	return this->history;
}
