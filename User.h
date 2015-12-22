#ifndef __USER_H__
#define __USER_H__
#include <string>
class User
{
	public:
		User(std::string mName);
		void SetStatus(std::string mstat);
		void SetMuteStatus(bool vstat);
		void SetIgnoreStatus(bool vstat);
		std::string GetName();
		std::string GetStatus();
		bool GetMuteStatus();
		bool GetIgnoreStatus();
	private:
		std::string Name;
		std::string Status;
		bool isMuted;
		bool isIgnored;
};

#endif