#ifndef __ROOM_H__
#define __ROOM_H__

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include "User.h"

class Room{
	public:
		Room(void);
		Room(std::string RName);
		void SetName(std::string RName);
		void SetDescription(std::string RDesc);
		void SetDate(std::string RDate);
		void SetCreator(std::string RCr);
		void SetUserCount(std::string RUC);
		std::string GetName();
		std::string GetDescription();
		std::string GetDate();
		std::string GetCreator();
		unsigned int GetUserCount();
	private:
		std::string Name;
		std::string Description;
		std::string Date;
		unsigned int UserCount;
		std::string Creator;
		bool isAutoMuted;
		std::vector<User> UserList;
};
#endif