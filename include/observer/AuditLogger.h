#ifndef AUDITLOGGER_H
#define AUDITLOGGER_H

#include "Observer.h"

class AuditLogger : public Observer
{
public:
	AuditLogger();
	void update(Event *event) override;
	AuditLogger *clone() override;
};

#endif
