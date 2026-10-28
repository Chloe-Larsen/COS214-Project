#include <iostream>
#include <sstream>
#include <string>

#include "../../include/adapter/LegacyEmailService.h"

LegacyEmailService::LegacyEmailService() {}

void LegacyEmailService::sendEmailHtmlBody(std::string email, std::string html)
{
	std::cout << "Sending email to - " << email << '\n'
			  << html << std::endl;
}