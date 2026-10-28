#ifndef LEGACYEMAILSERVICE_H
#define LEGACYEMAILSERVICE_H
#include <string>

class LegacyEmailService
{
public:
	LegacyEmailService();
	void sendEmailHtmlBody(std::string email, std::string html);
};

#endif
