#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "Observer.h"
#include "../adapter/EmailService.h"

class Notification : Observer
{
private:
	EmailService *emailService;

public:
	Notification(EmailService *alertService);
	
	void update(Event *event) override;
	Notification *clone() override;
};

#endif
