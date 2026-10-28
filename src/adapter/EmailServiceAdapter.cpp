#include "../../include/adapter/EmailServiceAdapter.h"

EmailServiceAdapter::EmailServiceAdapter() : legacyEmailService(new LegacyEmailService()) {}

EmailServiceAdapter::~EmailServiceAdapter()
{
	delete legacyEmailService;
}

void EmailServiceAdapter::sendEmail(std::string email, std::string subject, std::string body)
{
	std::string htmlBody = "<h2>" + subject + "</h2>\n<p>To: <i>" + subject + "</i></p>\n<p>" + body + "</p>";
	legacyEmailService->sendEmailHtmlBody(email, htmlBody);
}
