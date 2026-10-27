#include <iostream>

#include "../../include/observer/AuditLogger.h"
#include "../../include/composite/Item.h"

AuditLogger::AuditLogger() : Observer() {}

void AuditLogger::update(Event *event)
{
	std::cout << "(" << event->getTimestamp() << ") [AUDIT LOG > " << event->getSource()->getName() << "] " << eventTypeToString(event->getType()) << std::endl;
}

AuditLogger *AuditLogger::clone()
{
	return new AuditLogger();
}