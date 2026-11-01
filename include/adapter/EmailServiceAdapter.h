#ifndef EMAILSERVICEADAPTER_H
#define EMAILSERVICEADAPTER_H

#include "EmailService.h"
#include "LegacyEmailService.h"

class EmailServiceAdapter : public EmailService
{
private:
	LegacyEmailService *legacyEmailService;

public:
	EmailServiceAdapter();
	~EmailServiceAdapter();

	/**
	 * @brief Send an email with a subject and body, after converting it to a formatted HTML block to pass to the legacy service.
	 * 
	 * @param email The email address to send the email to.
	 * @param subject The subject line of the email.
	 * @param body The main content of the email.
	 */
	void sendEmail(std::string email, std::string subject, std::string body);
};

#endif
