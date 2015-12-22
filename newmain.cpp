/*************************************************************/
/*
			/f/lockdraw console chat client for Windows
				Version 2015W3510 (09/09/2015 00:17)
				  By Ninjihaku Software (C) 2015
*/
/*************************************************************/

#include "program.h"
#include "DefaultConfig.h"

int croom = -1;
std::string flockserver;
std::string flockport;
std::string flockversion;
bool AllowMain = false; //Avoid bans by leaving this as false, or risk one. I'm a comment, not a mod.

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

/*Function that prints a fancy ASCII art in the console,
not made by me. */
void PrintIntro()
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
return;
}

/* Function to make a string with the current time. */
void makectime()
{
	ctimestr.str(std::string());
	ctimestr.clear();
	clocktime = time(0);
	now = localtime(&clocktime);
	ctimestr << std::setfill('0') << std::setw(2) << now->tm_hour << ":" << std::setfill('0') << std::setw(2) << now->tm_min << ":" << std::setfill('0') << std::setw(2) << now->tm_sec;
}

/* Function that asks the user for login information. */
int AskForLogin()
{
	name.clear();
	password.clear();
	//Request the user for an username
	std::cout << "Enter username: ";
	std::getline (std::cin, name);
	
	//The name must contain some characters. Otherwise, abort.
	if(name.length() < 1)
	{
		std::cout << "Abort." << std::endl;
		return 0;
	}
	
	SetStdinEcho(false); //Mute the output
	//request for a password
	std::cout << std::endl << "Enter password (Or press ENTER for no password): ";
	std::getline (std::cin, password);
	SetStdinEcho(true);
	std::cout.flush();
	std::cin.clear();
	//std::cout << std::endl;
	return 1;
}

/* Function that processes the next element in the packet stack. */
int ProcessNextQueue()
{
	outcmd.clear();
	//Check if there is something in the stack
	if(CheckQueue() > 0)
	{
		std::vector<char> NextElement = PopQueue(); //Get the next element (FIFO format)
		std::vector<std::string> ElementStrip = BestCommandStrip(NextElement); //Get every command inside
		
		for(unsigned int nx = 0; nx < ElementStrip.size(); nx++)
		{
			std::vector<std::string> Cmd = GetCmdParams(ElementStrip[nx]); //Strip the command into parts
			
			//This actually displays the content of the command, in the debug version only.
			#ifdef _debug_
			for(int z = 0; z < Cmd.size(); z++)
				std::cout << Cmd[z] << " ";
			std::cout << std::endl;
			#endif
			
			if(Cmd[0].compare(CMDPING)==0) //Ping request
			{
				continue; //Ping requests are processed by the listener.
			}
			if(Cmd[0].compare(CMDPASSWORDREQUEST)==0) //Password request
			{
				if(password.length() < 1)
				{
					std::cout << "This user is registered, and no password was provided. Please enter a password: " << std::endl;
					SetStdinEcho(false); //Mute the output
					//request for a password
					std::cout << std::endl << "Enter password (Or no password to abort and exit): ";
					std::getline (std::cin, password);
					SetStdinEcho(true);
					std::cout << std::endl;
					if(password.length() < 1)
						return 1;
				}
				outcmd = "P " + password;
			}
			if(Cmd[0].compare(CMDCLIENTTOOOLD)==0) //Client is old error
			{
				std::cout << "ERROR: This client version is outdated. Please check for updates." << std::endl;
				return 1;
			}
			if(Cmd[0].compare(CMDLOGINADMIN)==0) //When Admin logs in, this is what he sees.
			{
				isLoggedIn = true;
				isAdmin = true;
				std::cout << "Wellcome, Admin :) " << std::endl;
			}
			if(Cmd[0].compare(CMDLOGINREG)==0) //We are logged in as a registered user
			{
				isLoggedIn = true;
				isRegistered = true;
				std::cout << "Logged in as " << name << " (Registered)" << std::endl;
			}
			if(Cmd[0].compare(CMDLOGINUNREG)==0) //Logged in as a normal user
			{
				isLoggedIn = true;
				std::cout << "Logged in as " << name << " (Unregistered)" << std::endl;
			}
			if(Cmd[0].compare(CMDROOMNAME)==0) //Got a room name (make new entry in the list)
			{
				Room nroom(Cmd[1]);
				RoomList.push_back(nroom);
			}
			if(Cmd[0].compare(CMDROOMDESC)==0) //Got a room description
			{
				std::string fulldesc;
				for(int n = 1; n < Cmd.size(); n++)
					fulldesc += Cmd[n] + " ";
				
				RoomList[croom].SetDescription(fulldesc);
			}
			if(Cmd[0].compare(CMDROOMCREATOR)==0) //Got the room creator
			{
				RoomList[croom].SetCreator(Cmd[1]);
			}
			if(Cmd[0].compare(CMDROOMDATE)==0) //Got the room creation date
			{
				RoomList[croom].SetDate(Cmd[1]);
			}
			if(Cmd[0].compare(CMDROOMUCNT)==0) //Got the user count of the room
			{
				RoomList[croom].SetUserCount(Cmd[1]);
			}
			if(Cmd[0].compare(CMDROOMENTRYEND)==0) //End of room list entry
			{
				croom++;
			}
			if(Cmd[0].compare(CMDROOMLISTEND)==0) //End of room list (display it)
			{
				std::cout << "\n --  R O O M   L I S T  --" << std::endl;
				for(int x = 0; x < RoomList.size(); x++)
				{
					std::cout << x << ". [" << RoomList[x].GetName() << "] -By: " << RoomList[x].GetCreator() << ", Users online: (" << RoomList[x].GetUserCount() << ")" << std::endl;
				}
				std::cout << "\nPress CNTRL to open the command prompt. Use the command /join to join a room (/join roomname)" << std::endl;
				std::cout << "Use the /rinfo command to get the detailed information for a specific room (/rinfo)" << std::endl;
			}
			if(Cmd[0].compare(CMDROOMDESREQ)==0) //Creating a new room, ask for the description, and if the user wants it to be private.
			{
				rk = 0;
				std::string roomDesc;
				std::string completecmd;
				std::cout << "Creating room " << selectedroom << ". Enter room description (or press ENTER for no description)." << std::endl;
				std::getline (std::cin, roomDesc);
				std::cout << "\n\nMake this room private? (Y)es or (N)o: ";
				std::cin >> rk;
				std::cout << std::endl;
				
				if(roomDesc.length() < 1)
					roomDesc = "This room was created with no description";
				if(rk == 'Y' || rk == 'y')
				completecmd = "RS 1 " + roomDesc;
					else
				completecmd = "RS 0 " + roomDesc;
			
				WriteMessage(completecmd);
			}
			if(Cmd[0].compare(CMDEMPTYROOM)==0) //This room is empty
			{
				if(!isInRoom)
				{
					std::cout << "- Wellcome to " << selectedroom << "! -" << std::endl;
					//Open up log file
					chatlogfile.open("chatlog.txt", std::ios::out | std::ios::ate | std::ios::app);
					for(int nx = 0; nx < RoomList.size(); nx++)
						if(RoomList[nx].GetName().compare(selectedroom) == 0)
							CurrentRoom = RoomList[nx];
					makectime();
					chatlogfile << "\n - SESSION LOG FROM " << asctime(now) << " IN ROOM " << selectedroom << " - \n" << std::endl;
					isInRoom = true;
				}
				std::cout << "This room is currently empty!" << std::endl;
			}
			if(Cmd[0].compare(CMDADDUSER)==0) //Add a new user to the room user list
			{
				if(!isInRoom)
				{
					std::cout << "- Wellcome to " << selectedroom << "! -" << std::endl;
					//Open up log file
					chatlogfile.open("chatlog.txt", std::ios::out | std::ios::ate | std::ios::app);
					for(int nx = 0; nx < RoomList.size(); nx++)
						if(RoomList[nx].GetName().compare(selectedroom) == 0)
							CurrentRoom = RoomList[nx];
						
					makectime();
					chatlogfile << "\n - SESSION LOG FROM " << asctime(now) << " IN ROOM " << selectedroom << " - \n" << std::endl;
					isInRoom = true;
				}
				User muser(Cmd[1]);
				if(std::find(IgnoreList.begin(), IgnoreList.end(), Cmd[1]) != IgnoreList.end())
					muser.SetIgnoreStatus(true);
				makectime();
				std::cout << ctimestr.str() << " -- User " << Cmd[1] << " has joined. --" << std::endl;
				chatlogfile << ctimestr.str() << " -- User " << Cmd[1] << " has joined. --" << std::endl;
				UserList.push_back(muser);
			}
			if(Cmd[0].compare(CMDREMOVEUSER)==0) //Remove a user from the room user list
			{
				std::vector<User> templist;
				for(int nx = 0; nx < UserList.size(); nx++)
					if(UserList[nx].GetName().compare(Cmd[1]) != 0)
						templist.push_back(UserList[nx]);
				
				UserList.clear();
				UserList = std::vector<User>(templist.begin(), templist.end());
				templist.clear();
				makectime();
				std::cout << ctimestr.str() << " -- User " << Cmd[1] << " has disconnected. --" << std::endl;
				chatlogfile << ctimestr.str() << " -- User " << Cmd[1] << " has disconnected. --" << std::endl;
			}
			if(Cmd[0].compare(CMDUPDATEUI)==0) //Request to update our UI. I interprete this as a "successfully joined" since the updates are live.
			{
				if(!isInRoom)
				{
					std::cout << "- Wellcome to " << selectedroom << "! -" << std::endl;
					//Open up log file
					chatlogfile.open("chatlog.txt", std::ios::out | std::ios::ate | std::ios::app);
					for(int nx = 0; nx < RoomList.size(); nx++)
						if(RoomList[nx].GetName().compare(selectedroom) == 0)
							CurrentRoom = RoomList[nx];
					makectime();
					chatlogfile << "\n - SESSION LOG FROM " << asctime(now) << " IN ROOM " << selectedroom << " - \n" << std::endl;
					isInRoom = true;
				}
			}
			if(Cmd[0].compare(CMDINMESSAGE)==0) //Incomming message
			{
				std::string completemsg;
				for(int z = 2; z < Cmd.size(); z++)
					completemsg += Cmd[z] + " ";
				makectime();
				//If the sender is not in the ignore list, display and log the message.
				for(int x = 0; x < UserList.size(); x++)
					if(UserList[x].GetName().compare(Cmd[1]) == 0 && !UserList[x].GetIgnoreStatus())
					{
						std::cout << ctimestr.str() << " " << Cmd[1] << ": " << completemsg << std::endl;
						chatlogfile << ctimestr.str() << " " << Cmd[1] << ": " << completemsg << std::endl;
					}
			}
			if(Cmd[0].compare(CMDINPRIVATE)==0) //Incomming private message
			{
				std::string completemsg;
				for(int z = 2; z < Cmd.size(); z++)
					completemsg += Cmd[z] + " ";
				makectime();
				for(int x = 0; x < UserList.size(); x++)
					if(UserList[x].GetName().compare(Cmd[1]) == 0 && !UserList[x].GetIgnoreStatus())
					{
						std::cout << ctimestr.str() << " (PRIVATE)" << Cmd[1] << ": " << completemsg << std::endl;
						chatlogfile << ctimestr.str() << " (PRIVATE)" << Cmd[1] << ": " << completemsg << std::endl;
					}
			}
			if(Cmd[0].compare(CMDMSGECHO)==0) //Echo of our message
			{
				std::string completemsg;
				for(int z = 1; z < Cmd.size(); z++)
					completemsg += Cmd[z] + " ";
				makectime();
				std::cout << ctimestr.str() << " (" << name << "): " << completemsg << std::endl;
				chatlogfile << ctimestr.str() << " (" << name << "): " << completemsg << std::endl;
			}
			if(Cmd[0].compare(CMDPRIVATEECHO)==0) //Echo of our private message
			{
				std::string completemsg;
				for(int z = 1; z < Cmd.size(); z++)
					completemsg += Cmd[z] + " ";
				makectime();
				std::cout << ctimestr.str() << " (" << name << ")->(" << lastpmuser << "): " << completemsg << std::endl;
				chatlogfile << ctimestr.str() << " (" << name << ")->(" << lastpmuser << "): " << completemsg << std::endl;
			}
			if(Cmd[0].compare(CMDINALERT)==0) //A global message sent probably by admin
			{
				std::string completemsg;
				for(int z = 2; z < Cmd.size(); z++)
					completemsg += Cmd[z] + " ";
				makectime();
				std::cout << ctimestr.str() << " !!(GLOBAL MESSAGE)!!: " << completemsg << std::endl;
				chatlogfile << ctimestr.str() << " !!(GLOBAL MESSAGE)!!: " << completemsg << std::endl;
			}
			if(Cmd[0].compare(CMDBANNED)==0) //We got banned from the room ;__;
			{
				makectime();
				std::cout << ctimestr.str() << " -- YOU HAVE BEEN BANNED FROM " << selectedroom << " --" << std::endl;
				chatlogfile << ctimestr.str() << " -- YOU HAVE BEEN BANNED FROM " << selectedroom << " --" << std::endl;
				isInRoom = false;
			}
			if(Cmd[0].compare(CMDAUTOMUTED)==0) //Room is auto-muted. This is irrelevant for us since we are not drawing, but it's good to know.
			{
				makectime();
				std::cout << ctimestr.str() << " -- You are auto-muted. This should not affect your chat experience though. -- " << std::endl;
			}
			if(Cmd[0].compare(CMDMUTED)==0) //We are muted. This is irrelevant for us since we are not drawing, but it's good to know.
			{
				makectime();
				std::cout << ctimestr.str() << " -- You have been muted. This should not affect your chat experience though. -- " << std::endl;	
			}
			if(Cmd[0].compare(CMDUNMUTED)==0) //We got unmuted. This is irrelevant for us since we are not drawing, but it's good to know.
			{
				makectime();
				std::cout << ctimestr.str() << " -- You have been unmuted. This should not affect your chat experience though. -- " << std::endl;	
			}
			if(Cmd[0].compare(CMDSERVERMAINTENANCE)==0) //Admin is shutting the server down for maintenance.
			{
				makectime();
				std::cout << ctimestr.str() << " !!(WARNING)!! THE SERVER IS GOING DOWN FOR MAINTENANCE! !!(WARNING)!!" << std::endl;
				chatlogfile << ctimestr.str() << " !!(WARNING)!! THE SERVER IS GOING DOWN FOR MAINTENANCE! !!(WARNING)!!" << std::endl;
			}
			if(Cmd[0].compare(CMDERROR)==0) //We got an error. Process it.
			{
				if(Cmd[1].compare("UserDoesNotExist")==0)
					std::cout << "(ERROR): That user does not exist." << std::endl;
				if(Cmd[1].compare("RoomDoesNotExist")==0)
					std::cout << "(ERROR): That room does not exist." << std::endl;
				if(Cmd[1].compare("NoUnregisteredRoomMods")==0)
					std::cout << "(ERROR): Can not promote an unregistered user." << std::endl;
				if(Cmd[1].compare("RoomModsNotAllowed")==0)
					std::cout << "(ERROR): Only the room owner can manage moderation rights." << std::endl;
				if(Cmd[1].compare("AlreadyRoomMod")==0)
					std::cout << "(ERROR): This user is already a mod." << std::endl;
				if(Cmd[1].compare("NotARoomMod")==0)
					std::cout << "(ERROR): This user is not a room mod." << std::endl;
				if(Cmd[1].compare("CanNotDemoteRoomOwner")==0)
					std::cout << "(ERROR): You can not demote the room owner!" << std::endl;
				if(Cmd[1].compare("AlreadyInRoom")==0)
					std::cout << "(ERROR): This username is already in the room. Check if you're logged in somewhere else." << std::endl;
				if(Cmd[1].compare("CanNotJoinRoomWhileBanned")==0)
					std::cout << "(ERROR): You are currently banned from this room!" << std::endl;
				if(Cmd[1].compare("MustBeRegisteredToCreateRoom")==0)
					std::cout << "(ERROR): You must be registered to create a room. Check the README file for instructions." << std::endl;
				if(Cmd[1].compare("IncorrectPassword")==0)
				{
					std::cout << "(ERROR): The password provided was incorrect. Please try again!" << std::endl;		
					loginRequested = false;
				}
				if(Cmd[1].compare("LockedOut")==0)
					std::cout << "(ERROR): Too many password failures. You have been locked out for 24 hours. This is a safety lock to prevent password thefts by brute force attacks." << std::endl;
				if(Cmd[1].compare("InvalidPassword")==0)
				{
					std::cout << "(ERROR): The password provided is not valid. Please try again!" << std::endl;				
					loginRequested = false;
				}
				if(Cmd[1].compare("HasNoCurrentBans")==0)
					std::cout << "(ERROR): This user is not banned." << std::endl;
				if(Cmd[1].compare("AlreadyBanned")==0)
					std::cout << "(ERROR): This user is already banned!" << std::endl;
			}
		}
		//TODO: Process the OK message (similar to the error, but for successful operations.).
		if(outcmd.length() > 0)
			WriteMessage(outcmd);
	}
	return 0;
}

/* Load the configuration file */
void GetFlockInfo()
{
	std::ifstream verfile;
	verfile.open("CONFIG");
	char buff[256];
	buff[0] = '#';
	std::string line;
	
	if(!verfile)
	{
		flockserver = CLIENTSERVER;
		flockport = CLIENTPORT;
		flockversion = CLIENTVERSION;
		std::cout << "- WARNING - : Default configuration loaded (CONFIG file not found)." << std::endl;
		
		std::ofstream outf;
		outf.open("CONFIG");
		if(!outf)
		{
			std::cout << "COULD NOT CREATE DEFAULT CONFIGURATION FILE." << std::endl;
			return;
		}
		outf << ConfigContent;
		outf.close();
		std::cout << "DEFAULT CONFIGURATION FILE CREATED. MODIFY IT IF NEEDED AND RESTART THE PROGRAM" << std::endl;
		//std::cout << "Server: " << flockserver << " Port: " << flockport << " Version " << flockversion << std::endl;
		return;
	}
	while(!verfile.eof() || verfile.peek() != EOF)
	{
		verfile.getline(buff, 256);
		if(buff[0] == '#' || buff[0] == 0x0 || buff[0] == '\n')
			continue;
		line = buff;
		if(line.find("SERVER=") != std::string::npos)
			flockserver = line.substr(line.find_first_of(' ')+1, line.length());
		if(line.find("PORT=") != std::string::npos)
			flockport = line.substr(line.find_first_of(' ')+1, line.length());
		if(line.find("VERSION=") != std::string::npos)
			flockversion = line.substr(line.find_first_of(' ')+1, line.length());
		if(line.find("AllowMain") != std::string::npos)
			AllowMain = true; //Again, use this configuration at your own risk.
	}
	//std::cout << "Server: " << flockserver << " Port: " << flockport << " Version " << flockversion << std::endl;
	//version = buff;
	verfile.close();
	return;
}

/* MAIN entry point */
int main(int argc, char** argv)
{
	GetFlockInfo();
	PrintIntro(); //Print a cool logo on screen.
	myConsole = GetConsoleWindow(); //Get the handle to our console. We'll need it to check it's focus.
	do{
		keyesc = GetKeyState(VK_ESCAPE); //If the ESC key is pressed, exit the program.
		keyret = GetKeyState(VK_CONTROL); //CONTROL key starts the input mode (send chat).
		
		//NOTE: Since GetKeyState seems to work asynchronous, I use GetConsoleWindow and GetForegroundWindow
		//to check if the window is focused when pressing one of these keys. No focus means no input.
		
		Sleep(5); //Make this process less CPU intensive by idling for a while.
		
		outcmdm.clear();
		/* SERVER LOG IN */
		if(!isLoggedIn && !loginRequested)
		{
			if(AskForLogin() < 1)
				break;
			
			std::cout << "Connecting to " << flockserver << ":" << flockport << "..." << std::endl;
			if(NetworkManager::Connect(flockserver, true, flockport) > 0)
			{
				std::cout << "Failed to connect to " << flockserver << ":" << flockport << std::endl;
				return EXIT_SUCCESS; //I use EXIT_SUCCESS here since the fact we didn't connect, doesn't mean the program has an error.
			}
			std::cout << "Connected to " << flockserver << ":" << flockport << std::endl;
			StartListener();
			
			std::cout << "Loggin in..." << std::endl;
			outcmdm = "L " + name + " " + flockversion; //This is a command.
			WriteMessage(outcmdm); //Send the log in command
			loginRequested = true;
			Sleep(100);
		}
		
		/* GET ROOM LIST */
		if(isLoggedIn && !isInRoom && !roomRequested)
		{
			//First read the ignored users list.
			std::cout << "Reading ignore list...";
			iglist.open("ignore.lst", std::fstream::in | std::fstream::out);
			if(!iglist.is_open())
				iglist.open("ignore.lst", std::fstream::in | std::fstream::out | std::fstream::trunc);
			iglist.seekg(iglist.beg);
			std::string newstr;
			while(std::getline(iglist, newstr))
			{
				IgnoreList.push_back(newstr);
			}
			iglist.close();
			std::cout << " done." << std::endl;
			
			//Now request the room list.
			croom = 0;
			RoomList.clear();
			std::cout << "Requesting room list..." << std::endl;
			WriteMessage("LR");
			roomRequested = true;
			Sleep(100);
		}
		
		/* CHECK IF THE CONNECTION IS STILL UP */
		if(ConnectionStatus())
		{
			std::cout << "Connection with the server lost." << std::endl;
			break;
		}
		
		/* PROCESS NEXT ELEMENT IN THE QUEUE */
		if(ProcessNextQueue()) //<- TODO: Fix that typo. :)
			break;
		
		/* GET CNTRL KEY STATUS (CHAT COMMAND LINE) */
		if((keyret&0x8000)&&GetForegroundWindow() == myConsole && isLoggedIn)
		{
			//If CNTRL is pressed, start the command line.
			mycmd.clear();
			fullcmd.clear();
			std::cin.clear();
			//Ask the user what should we say
			std::cout << "Chat> ";
			std::getline (std::cin, mycmd);
			mycmd.erase(mycmd.length(), 1);
			if(mycmd.length() > 0)
			{
				mypms = GetCmdParams(mycmd); //Parse what we got from the user
				if(mypms[0].compare("/pm") == 0 && isInRoom) //This is a private message command
				{
					if(mypms.size() < 3)
						std::cout << "-E- Error: /pm incorrect number of parameters (must be /pm username message)\n" << std::endl;
					else{
						for(int nz = 0; nz < UserList.size(); nz++)
						{
							if(UserList[nz].GetName().compare(mypms[1]) == 0 && !UserList[nz].GetIgnoreStatus())
							{
								fullcmd = "CU " + mypms[1] + " ";
								for(int yz = 2; yz < mypms.size(); yz++)
									fullcmd += mypms[yz] + " ";
								lastpmuser.clear();
								lastpmuser = mypms[1];
								break;
							}else if(UserList[nz].GetName().compare(mypms[1]) == 0 && UserList[nz].GetIgnoreStatus())
							{
								std::cout << "-E- Error: /pm You are ignoring " << mypms[1] << "\n" << std::endl;
								break;
							}
							if(nz == UserList.size()-1)
							{
								std::cout << "-E- Error: /pm user " << mypms[1] << " is not in the room.\n" << std::endl;
							}
						}
					}
				}
				//The /list command lists all the users in the room
				if(mypms[0].compare("/list") == 0 && isInRoom)
				{
					std::cout << " -- USER LIST -- " << std::endl;
					if(UserList.size() < 1)
						std::cout << "There are no other users in this room. You're alone here." << std::endl;
					else
						for(int nz = 0; nz < UserList.size(); nz++)
						{
							std::cout << "- " << UserList[nz].GetName(); 
							if(UserList[nz].GetIgnoreStatus())
								std::cout << " (Ignored)";
							if(UserList[nz].GetName().compare(CurrentRoom.GetCreator()) == 0)
								std::cout << " (Owner)";
							std::cout << std::endl;
						}
				}
				// The /me command sets your status message. Not visible in this version yet, but it's there.
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
						std::cout << " *-*- Status: " << mypms[1] << " -*-*" << std::endl;
						chatlogfile << " *-*- Status: " << mypms[1] << " -*-*" << std::endl;
					}
				}
				//The /ignorelist command prints the list of currently ignored users
				if(mypms[0].compare("/ignorelist") == 0 && isInRoom)
				{
					std::cout << " -- IGNORED USERS -- " << std::endl;
					if(IgnoreList.size() < 1)
						std::cout << "You don't have anyone in your ignore list!" << std::endl;
					else
						for(int x = 0; x < IgnoreList.size(); x++)
							std::cout << " - " << IgnoreList[x] << std::endl;
				}
				//The /ignore command is used to ignore an user, and not see his messages anymore.
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
						for(int nx = 0; nx < UserList.size(); nx++)
							if(UserList[nx].GetName().compare(mypms[1]) == 0)
								UserList[nx].SetIgnoreStatus(true);
						std::cout << " -!- " << mypms[1] << " is being ignored. You won't see any more messages from " << mypms[1] << std::endl;
					}
				}
				//The /unignore command, does the opposite of the /ignore command. What did you expect it to do? Launch nukes?
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

						for(int nx = 0; nx < UserList.size(); nx++)
							if(UserList[nx].GetName().compare(mypms[1]) == 0)
								UserList[nx].SetIgnoreStatus(false);
						std::cout << " -!- Username " << mypms[1] << " is no longer ignored. You will receive messages from " << mypms[1] << std::endl;
					}else{
						std::cout << " -E- Error: user " << mypms[1] << " is not in your ignore list." << std::endl;
					}
					StartNuclearStrike(&mypms[1][0]); //I mean, you're in my ignore list for something you f*cker!
				}
				//The /join command makes you join a room if you are not in one yet.
				if(mypms[0].compare("/join") == 0)
				{
					if(mypms.size() < 2)
					{
						std::cout << "Error: /join Room name required." << std::endl;
						continue;
					}
					if(isInRoom)
					{
						std::cout << "Error: You are inside of a room!" << std::endl;
						continue;	
					}
					std::string fullname;
					for(int nx = 1; nx < mypms.size(); nx++)
						fullname += mypms[nx];
					if(fullname.compare("/f/lockdraw") == 0 && !AllowMain)
					{
						std::cout << "Error: You are not allowed to join /f/lockdraw using this client." << std::endl;
						continue;
					}
					fullcmd = "C " + fullname;
					selectedroom = fullname;
				}
				// /rinfo shows the information of a certain room.
				if(mypms[0].compare("/rinfo") == 0)
				{
					if(mypms.size() < 2)
					{
						std::cout << "Error: /rinfo Room name required (or \"this\" for the current room information)." << std::endl;
						continue;
					}
					if(isInRoom)
					{
						std::cout << "NOTE: The information might be outdated! Using the information gathered from the last room request " << std::endl;
					}
					//Use "this" as a parameter for rinfo, to see the information of the current room.
					//TODO: Register information for own room.
					if(mypms[1].compare("this") == 0)
					{
							std::cout << "\n- Room name    : " << CurrentRoom.GetName() << std::endl;
							std::cout << "- Room Owner   : " << CurrentRoom.GetCreator() << std::endl;
							std::cout << "- Users online : " << (int)(UserList.size()+1) << std::endl;
							std::cout << "- Creation date: " << CurrentRoom.GetDate() << std::endl;
							std::cout << "- Description  : " << CurrentRoom.GetDescription() << "\n" << std::endl;
							continue;
					}
					for(int nx = 1; nx < RoomList.size(); nx++)
					{
						if(RoomList[nx].GetName().compare(mypms[1]) == 0)
						{
							std::cout << "\n- Room name    : " << RoomList[nx].GetName() << std::endl;
							std::cout << "- Room Owner   : " << RoomList[nx].GetCreator() << std::endl;
							std::cout << "- Users online : " << RoomList[nx].GetUserCount() << std::endl;
							std::cout << "- Creation date: " << RoomList[nx].GetDate() << std::endl;
							std::cout << "- Description  : " << RoomList[nx].GetDescription() << "\n" << std::endl;
							break;
						}
						if(nx == RoomList.size()-1)
						{
						std::cout << "Error: That room is not on the list." << std::endl;
						break;
						}
					}
					
				}
				// The /where command shows which room you're logged into right now.
				if(mypms[0].compare("/where") == 0)
				{
					if(isInRoom)
						std::cout << "- You are currently on: " << selectedroom << std::endl;
					else
						std::cout << "- You are not in a room. Use /join roomname to join a room. " << std::endl;
				}
				// /time prints the current date and time.
				if(mypms[0].compare("/time") == 0)
				{
					makectime();
					std::cout << " -Clock- " << asctime(now) << std::endl;
				}
				// The /who command shows who you are (your username).
				if(mypms[0].compare("/who") == 0)
				{
					std::cout << "- You are currently logged in as: " << name << std::endl;
				}
				//The /help command list all available commands.
				if(mypms[0].compare("/help") == 0)
				{
					std::cout << "\n- LIST OF AVAILABLE COMMANDS -\n" << std::endl;
					std::cout << "/pm [user] [message]: Send a private message to [user]." << std::endl;
					std::cout << "/list: List all users currently in the room." << std::endl;
					std::cout << "/me [message]: Set your own status to [message]." << std::endl;
					std::cout << "/ignore [user]: Add [user] to your ignored list." << std::endl;
					std::cout << "/unignore [user]: Remove [user] from your ignored list." << std::endl;
					std::cout << "/ignorelist: Show a list of ignored users." << std::endl;
					std::cout << "/join [roomname]: Joins a room, if you are not in one already." << std::endl;
					std::cout << "/rinfo [roomname]: Shows the information of [roomname]. If [roomname]=this, shows the information about the current room." << std::endl;
					std::cout << "/where: Tells you which room you're currently on." << std::endl;
					std::cout << "/time: Prints the current date and time on screen." << std::endl;
					std::cout << "/who: Tells you which username you're currently loged on with." << std::endl;
					std::cout << "/help: Shows you this list.\n" << std::endl;
				}
				//Check if the very first character is a '/'. If it's not, send a regular message.
				if(mypms[0][0] != '/')
				{
					fullcmd = "CB " + mycmd;
				}
				//In case the first character is a '/', if the second one is also a '/', it ignores the first '/' and sends the second as a normal message.
				if(mypms[0].length() > 1)
					if(mypms[0][0] == '/' && mypms[0][1] == '/')
					{
						fullcmd.clear();
						std::string strip(mycmd.begin()+1, mycmd.end());
						fullcmd = "CB " + strip;
					}
				
				//The debug version allows you to send raw commands by typing '@@ ' in front of them. For developers only.
				#ifdef _debug_
				if(mypms[0].compare("@@") == 0)
				{
					fullcmd.clear();
					std::string commd;
					for(int nx = 1; nx < mypms.size(); nx++)
					{
						commd += mypms[nx];
						if(nx != mypms.size()-1)
							commd += " ";
					}
					fullcmd = commd;
					std::cout << "-deb- " << fullcmd << std::endl;
				}
				#endif
				//If there is something to send to the server, send it.
				if(fullcmd.length() > 0)
					WriteMessage(fullcmd);
			}
		}
		//Do everything you've seen, until the ESC key is pressed while the console is focused.
	}while(!((keyesc&0x8000)&&GetForegroundWindow() == myConsole));
	
	/* EXIT PROGRAM */
	//Save the ignore list.
	std::cout << "Saving lists..." << std::endl;
	iglist.open("ignore.lst", std::fstream::in | std::fstream::out | std::fstream::trunc);
	for(int x = 0; x < IgnoreList.size(); x++)
		iglist << IgnoreList[x] << std::endl;
	//End the chat log and ignore list.
	chatlogfile << " - SESSION END -" << std::endl;
	std::cout << "Buh bye! :)" << std::endl;
	iglist.close();
	chatlogfile.close();
	//Terminate the connection
	TeminateListener(); //Tell the listener his job is done.
	NetworkManager::Disconnect(); //Dissconnect from the server
	std::cout.flush();
	std::cin.clear();
	std::cout << std::endl;
	return EXIT_SUCCESS; //Finish the process.
}