#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "Observer.h"
#include "../adapter/EmailService.h"

class Notification : public Observer
{
private:
	EmailService *emailService;

public:
	Notification(EmailService *alertService);

	/**
	 * @brief Send an email notification on an event.
	 *
	 * @param event The event forwarded via email.
	 */
	void update(Event *event) override;

	Notification *clone() override;
};

#endif
