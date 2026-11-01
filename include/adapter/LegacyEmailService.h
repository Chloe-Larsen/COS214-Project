#ifndef LEGACYEMAILSERVICE_H
#define LEGACYEMAILSERVICE_H
#include <string>

class LegacyEmailService
{
public:
	LegacyEmailService();

	/**
	 * @brief Send an HTML block as an email.
	 * 
	 * @param email The email address to send the email to.
	 * @param html The HTML block to send in the email.
	 */
	void sendEmailHtmlBody(std::string email, std::string html);
};

#endif
