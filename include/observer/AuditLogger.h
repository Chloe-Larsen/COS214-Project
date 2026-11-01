#ifndef AUDITLOGGER_H
#define AUDITLOGGER_H

#include "Observer.h"

class AuditLogger : public Observer
{
public:
	AuditLogger();

	/**
	 * @brief Logs the event as an audit log.
	 * 
	 * @param event The event to log.
	 */
	void update(Event *event) override;

	AuditLogger *clone() override;
};

#endif
