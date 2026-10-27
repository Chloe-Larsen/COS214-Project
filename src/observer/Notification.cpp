#include "../../include/observer/Notification.h"
#include "../../include/adapter/EmailServiceAdapter.h"
#include "../../include/chain/Worker.h"

Notification::Notification(EmailService *emailService) : emailService(emailService) {}

void Notification::update(Event *event)
{
	Item *source = event->getSource();
	Worker *worker = source->getWorker();
	Worker *supervisor = worker->getSupervisor();

	switch (event->getType())
	{
	case EventType::COMPLETED:
		// notify supervisor
		if (supervisor != nullptr)
			emailService->sendEmail(supervisor->getEmail(), "Item Completed", worker->getName() + " completed an item: " + source->getName() + ".");
		break;
	case EventType::APPROVED:
		// notify asignee
		emailService->sendEmail(worker->getEmail(), "Item Approved", "Your item, " + source->getName() + ", was approved.");
		break;
	case EventType::REJECTED:
		// notify asignee
		emailService->sendEmail(worker->getEmail(), "Item Approval Rejected", "Your approval request for your item, " + source->getName() + ", was rejected.");
		break;
	case EventType::ESCALATED:
		// notify supervisor
		if (supervisor != nullptr)
			emailService->sendEmail(supervisor->getEmail(), "Item Escalated", "The item, " + source->getName() + ", was escalated.");
		break;
	case EventType::ASSIGNED:
		// notify asignee
		emailService->sendEmail(worker->getEmail(), "Item Assignment", "You were assigned a new item: " + source->getName() + ".");
		break;
	}
}

Notification *Notification::clone()
{
	return new Notification(new EmailServiceAdapter());
}