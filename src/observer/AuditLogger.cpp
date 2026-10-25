#include <iostream>

#include "../../include/observer/AuditLogger.h"

AuditLogger::AuditLogger() : Observer()
{
	// TODO - implement AuditLogger::AuditLogger
	throw "Not yet implemented";
}

void AuditLogger::update(Event *event)
{
	std::cout << "(" << event->getTimestamp() << ") [AUDIT LOG > " << event->getSource()->getName() << "] " << event->getType() << std::endl;
}

AuditLogger *AuditLogger::clone()
{
	return new AuditLogger();
}