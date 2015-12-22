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

#ifdef _debug_
	#define VERSTRING "D2015W3507"
#elif _experimental_
	#include "experimental/dmodule.h"
	#define VERSTRING "E2015W3507"
#else
	#define VERSTRING "R2015W3507"
#endif

std::string name;
std::string password;
char keyesc = 0x0, keyret = 0x0;
HWND myConsole = GetConsoleWindow();
time_t clocktime;
struct tm* now;
std::vector<std::string> trim;
std::string proccmd;
std::vector<char> servcmd;
std::stringstream ctimestr;

/* ROOM VARIABLES AND VECTORS */
std::string roomname;
std::vector<std::string> UserList;
std::vector<std::string> IgnoreList;

/* Chat Log file */
std::ofstream chatlogfile;

/*Ignore list*/
std::fstream iglist;

/* Function to mute the output (for password input) */
void SetStdinEcho(bool enable)
{
#ifdef WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if( !enable )
        mode &= ~ENABLE_ECHO_INPUT;
    else
        mode |= ENABLE_ECHO_INPUT;

    SetConsoleMode(hStdin, mode );

#else
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if( !enable )
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

void makectime()
{
	ctimestr.str(std::string());
	ctimestr.clear();
	clocktime = time(0);
	now = localtime(&clocktime);
	ctimestr << std::setfill('0') << std::setw(2) << now->tm_hour << ":" << std::setfill('0') << std::setw(2) << now->tm_min << ":" << std::setfill('0') << std::setw(2) << now->tm_sec;
}

int main(int argc, char** argv)
{
std::cout << "	      .andAHHAbnn. "<< std::endl;
std::cout << "           .aAHHHAAUUAAHHHAn. "<< std::endl;
std::cout << "          dHP^~\"        \"~^THb. "<< std::endl;
std::cout << "    .   .AHF                YHA.   . " << std::endl;
std::cout << "    |  .AHHb.              .dHHA.  | " << std::endl;
std::cout << "    |  HHAUAAHAbn      adAHAAUAHA  | " << std::endl;
std::cout << "    I  HF~\"_____        ____ ]HHH  I " << std::endl;
std::cout << "   HHI HAPK\"\"~^YUHb  dAHHHHHHHHHH IHH " << std::endl;
std::cout << "   HHI HHHD> .andHH  HHUUP^~YHHHH IHH " << std::endl;
std::cout << "   YUI ]HHP     \"~Y  P~\"     THH[ IUP " << std::endl;
std::cout << "    \"  `HK                   ]HH'  \" " << std::endl;
std::cout << "        THAn.  .d.aAAn.b.  .dHHP " << std::endl;
std::cout << "        ]HHHHAAUP\" ~~ \"YUAAHHHH[ " << std::endl;
std::cout << "        `HHP^~\"  .annn.  \"~^YHH' " << std::endl;
std::cout << "         YHb    ~\" \"\" \"~    dHF " << std::endl;
std::cout << "          \"YAb..abdHHbndbndAP\" " << std::endl;
std::cout << "           THHAAb.  .adAHHF " << std::endl;
std::cout << "            \"UHHHHHHHHHHU\" " << std::endl;
std::cout << "              ]HHUUHHHHHH[ " << std::endl;
std::cout << "            .adHHb \"HHHHHbn. " << std::endl;
std::cout << "     ..andAAHHHHHHb.AHHHHHHHAAbnn.. " << std::endl;
std::cout << ".ndAAHHHHHHUUHHHHHHHHHHUP^~\"~^YUHHHAAbn. " << std::endl;
std::cout << "  \"~^YUHHP\"   \"~^YUHHUP\"        \"^YUP^\" " << std::endl;
std::cout << "       \"\"         \"~~\" " << std::endl;
	std::cout << "/f/lockdraw  chat console client interface for Windows\nBy Ninjihaku Software (C) 2015 - Version " << VERSTRING << std::endl << std::endl;
	clocktime = time(0);
	//Request the user for an username
	std::cout << "Enter username: ";
	std::getline (std::cin, name);
	std::string mycmd;
	
	//The name must contain some characters. Otherwise, abort.
	if(name.length() < 1)
	{
		std::cout << "Abort." << std::endl;
		return EXIT_SUCCESS;
	}
	
	SetStdinEcho(false); //Mute the output
	//request for a password
	std::cout << std::endl << "Enter password (Or press ENTER for no password): ";
	std::getline (std::cin, password);
	SetStdinEcho(true);
	std::cout << std::endl;
	
	//Connect to the server
	std::cout << "Connecting to drawserver.skycow.us..." << std::endl;
	if(NetworkManager::Connect("drawserver.skycow.us", true, "443") > 0)
	{
		std::cout << "Failed to connect to drawserver.skycow.us" << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << "Connected to drawserver.skycow.us" << std::endl;
	
	//Start the network listener in a parallel process.
	StartListener();
	
	//Attempt to log in using the provided information.
	if(ServerLogin(name, password) < 1)
	{
			TeminateListener();
			NetworkManager::Disconnect();
			std::cout << "Server login failed." << std::endl;
			return EXIT_SUCCESS;
	}
	
	std::cout << "Logged in drawserver.skycow.us as " << name << std::endl;
	
	//Request and display the room list
	//FIXME: Sometimes the list comes empty for some reason.
	std::vector<std::string> allroominfo = ProcessRoomList();
	std::cout << "\n---   R O O M   L I S T   ---" << std::endl;
	if(allroominfo.size() < 1)
		std::cout << " -!- NO ROOMS -!- " << std::endl;
	else
		for(int n = 0; n < allroominfo.size(); n++)
			if(allroominfo[n].find("/f/lockdraw")==std::string::npos)
				std::cout << " - "<< allroominfo[n] << std::endl;
	
	//Ask the user for a room name
	std::cout << "\nEnter a room name to enter that room. If the room does not exist, it will be created." << std::endl;
	std::cout << "You can not access the main room using this client due to the room policy.\n" << std::endl;
	std::cout << "Room name: ";
	std::getline (std::cin, roomname);
	std::cout << std::endl;
	
	//If the room name is empty, abort.
	if(roomname.length() < 1)
	{
		std::cout << "Aborted. Disconnecting." << std::endl;
		TeminateListener();
		NetworkManager::Disconnect();
		std::cout << std::endl;
		return EXIT_SUCCESS;
	}
	
	//Admin does not let me connect to the main room.
	if(roomname.compare("/f/lockdraw") == 0)
	{
		std::cout << "Sorry, the Administrator does not allow bots in the main room. Aborting..." << std::endl;
		TeminateListener();
		NetworkManager::Disconnect();
		std::cout << std::endl;
		return EXIT_SUCCESS;
	}
	
	//Send the room join command.
	std::string concommd = "C " + roomname;
	WriteMessage(concommd);
	
	//Check if we can join or create the room
	bool rlcheck = false;
	do{
	Sleep(10);
		if(CheckQeue() > 0)
		{
			servcmd.clear();
			servcmd = PopQueue();
			proccmd = StripMessage(servcmd);
			char rk;
			
			std::vector<std::string> parseall = BestCommandStrip(servcmd);
			for(unsigned int n = 0; n < parseall.size(); n++)
			{
				std::vector<std::string> cmdsplit = GetCmdParams(parseall[n]);
				if(cmdsplit.size() < 1)
					continue;
				if(cmdsplit[0].compare("RRS") == 0)
				{
					std::string roomDesc;
					concommd.clear();
					std::cout << "Creating room " << roomname << ". Enter room description (or press ENTER for no description)." << std::endl;
					std::getline (std::cin, roomDesc);
					std::cout << "\n\nMake this room private? (Y)es or (N)o: ";
					std::cin >> rk;
					std::cout << std::endl;
					if(roomDesc.length() < 1)
						roomDesc = "This room was created with no description";
					if(rk == 'Y' || rk == 'y')
						concommd = "RS 1 " + roomDesc;
					else
						concommd = "RS 0 " + roomDesc;
					WriteMessage(concommd);
				}
				if(cmdsplit[0].compare("A") == 0)
				{
					std::cout << cmdsplit[1] << " ( NEW USER JOIN IN ) " << std::endl;
					chatlogfile << cmdsplit[1] << " ( NEW USER JOIN IN ) " << std::endl;
					UserList.push_back(cmdsplit[1]);
					rlcheck = true;
				}
				if(cmdsplit[0].compare("JPR") == 0 || cmdsplit[0].compare("JRR") == 0 || cmdsplit[0].compare("AUF") == 0)
				{
					rlcheck = true;
				}
				if(cmdsplit[0].compare("ERR")==0)
				{
					if(cmdsplit[1].compare("RoomDoesNotExist")==0)
						std::cout << "Error: Room does not exist and can not be created." << std::endl;
					else if(cmdsplit[1].compare("AlreadyInRoom")==0)
						std::cout << "Error: Your name is already in that room! Check if you are already logged in somewhere else." << std::endl;
					else if(cmdsplit[1].compare("CanNotJoinRoomWhileBanned")==0)
						std::cout << "Error: You are banned from this room!" << std::endl;
					else if(cmdsplit[1].compare("InvalidRoomName")==0)
						std::cout << "Error: Invalid room name. Try another name." << std::endl;
					else if(cmdsplit[1].compare("MustBeRegisteredToCreateRoom")==0)
						std::cout << "Error: You must be registered in order to create a room." << std::endl;
				}
				//parse
			}
		}
		if(ConnectionStatus())
		{
			std::cout << "Lost the connection with the server (time out)." << std::endl;
			TeminateListener();
			NetworkManager::Disconnect();
			std::cout << std::endl;
			return EXIT_SUCCESS;
		}
	}while(!rlcheck);
	
	//Open up log file
	chatlogfile.open("chatlog.txt", std::ios::out | std::ios::ate | std::ios::app);
	
	makectime(); //Get time string and print a wellcoming message
	std::cout << "\n- You are now connected to " << roomname << "-" << std::endl;
	std::cout << " - Login date: " << (now->tm_year+1900) << "/" << now->tm_mon+1 << "/" << now->tm_mday <<
		" " << ctimestr.str() << std::endl;
	chatlogfile << "- CHATLOG: SESSION IN ROOM " << roomname << "-" << std::endl;
	chatlogfile << " - Log date: " << (now->tm_year+1900) << "/" << now->tm_mon+1 << "/" << now->tm_mday <<
		" " << ctimestr.str() << std::endl << std::endl;
	chatlogfile << "Logged in as " << name << std::endl << std::endl;
	
	std::cout << "Reading ignore list... ";
	
	iglist.open("ignore.lst", std::fstream::in | std::fstream::out);
	if(!iglist.is_open())
		iglist.open("ignore.lst", std::fstream::in | std::fstream::out | std::fstream::trunc);
	
	iglist.seekg(iglist.beg);
	std::string newstr;
	while(std::getline(iglist, newstr))
	{
		IgnoreList.push_back(newstr);
	}
	
	std::cout << "done!\n" << std::endl;
	std::cout << "Press CTRL key to send a message. Press ESC to disconnect.\n" << std::endl;
	
	#ifdef _experimental_
	StartTheMayhem();
	#endif
	
	//MAIN LOOP
	do{
		Sleep(10);
		servcmd.clear();
		trim.clear();
		proccmd.clear();
		makectime();
		keyesc = GetKeyState(VK_ESCAPE); //If the ESC key is pressed, exit the program.
		keyret = GetKeyState(VK_CONTROL); //CONTROL key starts the input mode (send chat).
		if(CheckQeue() > 0)
		{
			servcmd = PopQueue(); //Get next packet from the queue
			trim = BestCommandStrip(servcmd); //Process it
			
			for(int x = 0; x < trim.size(); x++)
			{
				//Get the command and the parameters separatedly
				std::vector<std::string> cmdparsed = GetCmdParams(trim[x]);
				
				#ifdef _debug_
				std::cout << "-> Deb: " << trim[x] << std::endl;
				#endif
				
				if(cmdparsed[0].compare("BC") == 0) //If we get a message, print it
				{
					std::string completetext;
					for(int n = 2; n < cmdparsed.size(); n++)
						completetext += cmdparsed[n] + " "; //Join all the text pieces
					std::string nocmd(trim[x].begin()+3, trim[x].end());
					if(std::find(IgnoreList.begin(), IgnoreList.end(), cmdparsed[1]) == IgnoreList.end())
					{
						std::cout << ctimestr.str() << " " << cmdparsed[1] << ": " << completetext << std::endl;
						chatlogfile << ctimestr.str() << " " << cmdparsed[1] << ": " << completetext << std::endl;
					}
				}
				if(cmdparsed[0].compare("UC") == 0) //Private message
				{
					std::string completetext;
					for(int n = 2; n < cmdparsed.size(); n++)
						completetext += cmdparsed[n] + " ";
					std::string nocmd(trim[x].begin()+3, trim[x].end());
					if(std::find(IgnoreList.begin(), IgnoreList.end(), cmdparsed[1]) == IgnoreList.end())
					{
						std::cout << ctimestr.str() << " (PRIVATE) " << cmdparsed[1] << ": " << completetext << std::endl;
						chatlogfile << ctimestr.str() << " (PRIVATE) " << cmdparsed[1] << ": " << completetext << std::endl;
					}
				}
				if(cmdparsed[0].compare("BCU") == 0) //This is the echo of our message.
				{
					std::string completetext;
					for(int n = 1; n < cmdparsed.size(); n++)
						completetext += cmdparsed[n] + " ";
					std::string nocmd(trim[x].begin()+4, trim[x].end());
					std::cout << ctimestr.str() << " (" << name << "): " << completetext << std::endl;
					chatlogfile << ctimestr.str() << " (" << name << "): " << completetext << std::endl;
				}
				if(cmdparsed[0].compare("A") == 0) //User has joined the room
				{
					std::cout << ctimestr.str() << " " << cmdparsed[1] << " ( NEW USER JOIN IN ) " << std::endl;
					chatlogfile << ctimestr.str() << " " << cmdparsed[1] << " ( NEW USER JOIN IN ) " << std::endl;
					UserList.push_back(cmdparsed[1]);
				}
				if(cmdparsed[0].compare("R") == 0) //User has disconnected
				{
					for(int x = 0; x < UserList.size(); x++)
					{
						if(UserList[x].compare(cmdparsed[1]) == 0)
						{
							UserList.erase(UserList.begin()+x);
							break;
						}
					}
					std::cout << ctimestr.str() << " ( USER DISCONNECTED ) " << cmdparsed[1] << std::endl;
					chatlogfile << ctimestr.str() << " ( USER DISCONNECTED ) " << cmdparsed[1] << std::endl;
				}
				if(cmdparsed[0].compare("X") == 0) //We got b& ;__;
				{
					std::cout << "Sorry! You have been banned from " << roomname << std::endl;
					break;
				}
			}
		}
		
		if((keyret&0x8000)&&GetForegroundWindow() == myConsole)
		{
			mycmd.clear();
			//Ask the user what should we say
			std::cout << "Chat> ";
			std::getline (std::cin, mycmd);
			mycmd.erase(mycmd.length(), 1);
			//If we have something to say, send it to the server.
			if(mycmd.length() > 0)
			{
				std::string fullcmd;
				std::vector<std::string> mypms = GetCmdParams(mycmd);
				
				if(mypms[0].compare("/me") == 0)
				{
					if(mypms.size() > 1)
					{
						if(mypms.size() > 1 && mypms[1].length() <= 15)
						{
							fullcmd = "ME ";
							for(int x = 1; x < mypms.size(); x++)
							 fullcmd += mypms[x] + " ";
						}else if (mypms[1].length() > 15){
							std::cout << "-E- Error: /me parameter 1 must can't be over 15 characters long. Parameter 1 is actually " << mypms[1].length() << " characters long.\n" << std::endl;
						}
					}else{
						fullcmd = "ME ";
					}
				}
				if(mypms[0].compare("/pm") == 0)
				{
					if(mypms.size() < 3)
						std::cout << "-E- Error: /pm incorrect number of parameters (must be /pm username message)\n" << std::endl;
					else{
						if(std::find(UserList.begin(), UserList.end(), mypms[1]) == UserList.end())
						{
							std::cout << "-E- Error: User " << mypms[1] << " is not in the list." << std::endl;
						}else{
							fullcmd = "CU " + mypms[1] + " ";
							for(int x = 2; x < mypms.size(); x++)
								fullcmd += mypms[x] + " ";
						}
					}
				}
				if(mypms[0].compare("/list") == 0)
				{
					std::cout << " -- USER LIST FOR ROOM " << roomname << " -- " << std::endl;
					if(UserList.size() < 1)
						std::cout << "You are all alone here! ;;__;;" << std::endl;
					else
						for(int x = 0; x < UserList.size(); x++)
							std::cout << " - " << UserList[x] << std::endl;
				}
				if(mypms[0].compare("/ignorelist") == 0)
				{
					std::cout << " -- IGNORED USERS -- " << std::endl;
					if(IgnoreList.size() < 1)
						std::cout << "You don't have anyone in your ignore list!" << std::endl;
					else
						for(int x = 0; x < IgnoreList.size(); x++)
							std::cout << " - " << IgnoreList[x] << std::endl;
				}
				if(mypms[0].compare("/ignore") == 0)
				{
					if(mypms.size() < 2)
					{
						std::cout << " -E- Error: /ignore must provide a name (/ignore username)" << std::endl;
						continue;
					}
					else if(std::find(IgnoreList.begin(), IgnoreList.end(), mypms[1]) != IgnoreList.end())
						std::cout << " -E- Error: That user is already ignored!" << std::endl;
					else{
						IgnoreList.push_back(mypms[1]);
						iglist << mypms[1] << std::endl;
						std::cout << " -!- " << mypms[1] << " is being ignored. You won't see any more messages from " << mypms[1] << std::endl;
					}
				}
				if(mypms[0].compare("/unignore") == 0)
				{
					if(mypms.size() < 2)
					{
						std::cout << " -E- Error: /unignore must provide a name (/unignore username)" << std::endl;
						continue;
					}
					int fnd = 0;
					if(fnd = std::find(IgnoreList.begin(), IgnoreList.end(), mypms[1]) != IgnoreList.end())
					{
						IgnoreList.erase(IgnoreList.begin()+fnd);
						iglist.close();
						iglist.open("ignore.lst", std::fstream::in | std::fstream::out | std::fstream::trunc);
						for(int nx = 0; nx < IgnoreList.size(); nx++)
							iglist << IgnoreList[nx] << std::endl;
						std::cout << " -!- Username " << mypms[1] << " is no longer ignored. You will receive messages from " << mypms[1] << std::endl;
					}else{
						std::cout << " -E- Error: user " << mypms[1] << " is not in your ignore list." << std::endl;
					}
				}
				if(mypms[0].compare("/help") == 0)
				{
					std::cout << "\nCommands:\n"
					<< "/me (message)	     : Set own status (only visible in the original client)\n"
					<< "/pm (username)	     : Send a private message to (username)\n"
					<< "/list		     : Shows a list of the users in the room.\n"
					<< "/ignorelist	     : Show a list of ignored users\n"
					<< "/ignore (username)   : Stop showing incomming messages from (username)\n"
					<< "/unignore (username) : Allow incomming messages from ignred user (username)\n"
					<< "/help		     : Shows this\n"
					<< " -- END OF HELP -- " << std::endl;
					
				}
				if(mycmd[0] != '/')
				{
					fullcmd = "CB " + mycmd;
				}
				if(fullcmd.length() > 0)
					WriteMessage(fullcmd);
			}
		}
		
		if(ConnectionStatus())
		{
			std::cout << " -E- Lost the connection with the server." << std::endl;
			break;
		}
		
	}while(!((keyesc&0x8000)&&GetForegroundWindow() == myConsole)); //On ESC key pressed, exit
	
	chatlogfile << " - SESSION END -" << std::endl;
	std::cout << "Buh bye! :)" << std::endl;
	iglist.close();
	chatlogfile.close();
	TeminateListener(); //Tell the listener his job is done.
	NetworkManager::Disconnect(); //Dissconnect from the server
	std::cout << std::endl;
	return EXIT_SUCCESS;
}