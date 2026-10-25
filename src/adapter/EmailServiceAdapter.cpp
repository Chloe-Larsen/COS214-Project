#include "../../include/adapter/EmailServiceAdapter.h"

EmailServiceAdapter::EmailServiceAdapter() : legacyEmailService(new LegacyEmailService()) {}

EmailServiceAdapter::~EmailServiceAdapter()
{
	delete legacyEmailService;
}

void EmailServiceAdapter::sendEmail(std::string email, std::string subject, std::string body)
{
	legacyEmailService->send(email + ";" + subject + ";" + body); // legacy service accepts single string with all data
}
