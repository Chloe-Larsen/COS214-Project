#include <iostream>
#include <sstream>
#include <string>

#include "../../include/adapter/LegacyEmailService.h"

LegacyEmailService::LegacyEmailService() {}

void LegacyEmailService::send(std::string data)
{
	std::stringstream ss(data);
	std::string email;
	std::string subject;
	std::string body;

	// split string using ';' as the delimiter
	std::getline(ss, email, ';');
	std::getline(ss, subject, ';');
	std::getline(ss, body);

	std::cout << "Sending email to - " << email << " [" << subject << "] " << body << std::endl;
}