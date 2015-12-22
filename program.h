#ifndef __FLOCKDRAW_CHAT_H__
#define __FLOCKDRAW_CHAT_H__

#define _WIN32_WINNT 0x501 //This should be on windows.h
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>

#ifndef WIN32
#include <termios.h>
#include <unistd.h>
#endif

#include <iostream>
#include <string>
#include "EvenAwesomerSockets.h"
#include "ServerMessages.h"
#include "Room.h"

/* Versions are ISO 8601 dates, preceded of a letter that indicates which
kind of compilation the executable is (R for Release, D for Debug, and E for Experimental).
So a Release executable compiled in 09/09/2015 would get the R2015W3510 version label.*/

#ifdef _debug_
	#define VERSTRING "B50B20151222"
#elif _experimental_
	#include "experimental/dmodule.h"
	#define VERSTRING "SB50D20151222"
#else
	#define VERSTRING "B50B20151222"
#endif

#define CLIENTVERSION "32" //<- If you get a "client too old" error, update this value to the current major version of the official client.
#define CLIENTSERVER "drawserver.skycow.us"
#define CLIENTPORT "443"

/* TIME */
std::stringstream ctimestr;
time_t clocktime;
struct tm* now;

/* GLOBAL VARIABLES */
char keyesc = 0x0, keyret = 0x0, rk = 0x0;
HWND myConsole;
std::string outcmdm;
std::string outcmd;
std::vector<std::string> mypms;
std::string fullcmd;
std::string lastpmuser;

/* USER VARIABLES */
std::string name;
std::string password;
std::string selectedroom;
std::string mycmd;
bool isLoggedIn = false;
bool isInRoom = false;
bool isRoomOwner = false;
bool isRoomMod = false;
bool isAdmin = false;
bool isRegistered = false;
bool loginRequested = false;
bool roomRequested = false;
bool joinRequested = false;

/* Room Variables*/
std::vector<Room> RoomList;
std::vector<User> UserList;
Room CurrentRoom;

/* Chat Log file */
std::ofstream chatlogfile;

/*Ignore list*/
std::fstream iglist;
std::vector<std::string> IgnoreList;

#endif