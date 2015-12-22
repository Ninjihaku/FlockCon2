/*
This class is basically a structure with data related to a room.
You can set or get it's properties through the following functions.
*/

#include "room.h"
Room::Room()
{
	
}

Room::Room(std::string RName)
{
	this->Name = RName;
}

void Room::SetName(std::string RName)
{
	this->Name = RName;
	return;
}

void Room::SetDescription(std::string RDesc)
{
	this->Description = RDesc;
	return;
}

void Room::SetDate(std::string RDate)
{
	this->Date = RDate;
	return;
}

void Room::SetCreator(std::string RCr)
{
	this->Creator = RCr;
	return;
}

void Room::SetUserCount(std::string RUC)
{
	this->UserCount = std::strtol(RUC.c_str(),0,10);
	return;
}

std::string Room::GetName()
{
	return this->Name;
}

std::string Room::GetDescription()
{
	return this->Description;
}

std::string Room::GetDate()
{
	return this->Date;
}

std::string Room::GetCreator()
{
	return this->Creator;
}

unsigned int Room::GetUserCount()
{
	return this->UserCount;
}