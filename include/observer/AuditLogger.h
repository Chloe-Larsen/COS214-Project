#ifndef AUDITLOGGER_H
#define AUDITLOGGER_H

#include "Observer.h"

class AuditLogger : Observer
{
public:
	AuditLogger();
	void update(Event *event) override;
};

#endif
