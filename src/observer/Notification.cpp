#include "../../include/observer/Notification.h"
#include "../../include/adapter/EmailServiceAdapter.h"
#include "../../include/chain/Worker.h"

Notification::Notification(EmailService *emailService) : emailService(emailService) {}

void Notification::update(Event *event)
{
	Item *source = event->getSource();
	Worker *worker = source->getWorker();
	Worker *supervisor = worker->getSupervisor();
	std::string type = event->getType();

	if (type == "COMPLETED")
	{
		if (supervisor != nullptr)
			emailService->sendEmail(supervisor->getEmail(), "Item Completed", worker->getName() + " completed an item: " + source->getName() + ".");
	}
	else if (type == "APPROVED")
	{
		emailService->sendEmail(worker->getEmail(), "Item Approved", "Your item, " + source->getName() + ", was approved.");
	}
	else if (type == "REJECTED")
	{
		emailService->sendEmail(worker->getEmail(), "Item Approval Rejected", "Your approval request for your item, " + source->getName() + ", was rejected.");
	}
	else if (type == "ESCALATED")
	{
		if (supervisor != nullptr)
			emailService->sendEmail(supervisor->getEmail(), "Item Escalated", "The item, " + source->getName() + ", was escalated.");
	}
	else if (type == "ASSIGNED")
	{
		emailService->sendEmail(worker->getEmail(), "Item Assignment", "You were assigned a new item: " + source->getName() + ".");
	}
}

Notification *Notification::clone()
{
	return new Notification(new EmailServiceAdapter());
}