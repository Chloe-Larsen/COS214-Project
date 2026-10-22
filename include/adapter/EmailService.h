#ifndef EMAILSERVICE_H
#define EMAILSERVICE_H

#include <string>

class EmailService
{
public:
	EmailService();
	virtual ~EmailService();
	virtual void sendEmail(std::string email, std::string subject, std::string body) = 0;
};

#endif
