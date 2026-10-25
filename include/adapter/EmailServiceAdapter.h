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

	void sendEmail(std::string email, std::string subject, std::string body);
};

#endif
