#ifndef EMAILSERVICE_H
#define EMAILSERVICE_H

#include <string>

class EmailService
{
public:
	EmailService();
	virtual ~EmailService();
	
	/**
	 * @brief Send an email with a subject and body.
	 * 
	 * @param email The email address to send the email to.
	 * @param subject The subject line of the email.
	 * @param body The main content of the email.
	 */
	virtual void sendEmail(std::string email, std::string subject, std::string body) = 0;
};

#endif
