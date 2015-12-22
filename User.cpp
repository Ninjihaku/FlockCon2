/*
This class is basically a structure with data related to a user.
You can set or get it's properties through the following functions.
*/

#include "user.h"

User::User(std::string mName)
{
	this->Name = mName;
	this->isIgnored = false;
}

void User::SetStatus(std::string mstat)
{
	this->Status = mstat;
	return;
}

void User::SetMuteStatus(bool vstat)
{
	this->isMuted = vstat;
	return;
}

void User::SetIgnoreStatus(bool vstat)
{
	this->isIgnored = vstat;
	return;
}

std::string User::GetName()
{
	return this->Name;
}

std::string User::GetStatus()
{
	return this->Status;
}

bool User::GetMuteStatus()
{
	return this->isMuted;
}

bool User::GetIgnoreStatus()
{
	return this->isIgnored;
}