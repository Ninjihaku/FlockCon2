#pragma once
#ifndef __SERVER_MESSAGES__H
#define __SERVER_MESSAGES__H

#define MAXBUFLEN 4096 //<- Size of the packet buffer. I think this is deprectated?
#define MAX_TIMEOUT 60 //<- maximum number of idling seconds before the connection is considered lost.

#include <vector>
#include <string>

#define CMDPASSWORDREQUEST "PR" //Password request for registered user
#define CMDCLIENTTOOOLD "O" //Client too old. The version does not match
#define CMDLOGINADMIN "LA" //Logged in as an admin (Yo admin)
#define CMDLOGINREG "LI" //Logged in as a registered user
#define CMDLOGINUNREG "LU" //Logged in as a normal user (unregistered)
#define CMDPING "PI" //Ping request
#define CMDROOMNAME "LRN" //Room name
#define CMDROOMDESC "LRD" //Room description
#define CMDROOMCREATOR "LRU" //Room creator name
#define CMDROOMDATE "LRT" //Room creation date
#define CMDROOMUCNT "LRC" //Room user count
#define CMDROOMMUTE "LRM" //Is room muted
#define CMDMAINROOM "LRF" //Is default room (main, banned, etc)
#define CMDROOMENTRYEND "LRE" //End of room entry (new one might come next)
#define CMDROOMLISTEND "ARF" //End of room list
#define CMDROOMDESREQ "RRS" //Requesting room description (for brand new rooms)
#define CMDROOMPREVIEW "PRI" //Room preview incomming
#define CMDEMPTYROOM "N" //Room is empty (no users but you)
#define CMDADDUSER "A" //User logged in
#define CMDREMOVEUSER "R" //User logged out
#define CMDUPDATEUI "AUF" //Update interface request
#define CMDINMESSAGE "BC" //Incomming message
#define CMDMSGECHO "BCU" //Message echo
#define CMDINPRIVATE "UC" //Incomming private message
#define CMDPRIVATEECHO "UCU" //Private message echo
#define CMDINALERT "CBC" //Global alert message
#define CMDBANNED "X" //You are b& ;;___;;
#define CMDINBOARD "BU" //The server is sending you the board
#define CMDINBOARDREQ "BUQ" //The server asks you for your board
#define CMDAUTOMUTED "AMT" //You are auto-muted.
#define CMDMUTED "MT" //You were muted.
#define CMDUNMUTED "UM" //You were unmuted
#define CMDROOMMUTED "RMT" //The room is muted
#define CMDROOMUNMUTED "RUM" //The room is unmuted
#define CMDUSERMUTED "UMT" //The following user is muted
#define CMDUSERUNMUTED "UUM" //The following user was unmuted
#define CMDUNDOING "UDA" //Undoing
#define CMDINUNDO "UDN" //Incomming undo
#define CMDMODDED "JRR" //You are now a mod
#define CMDDEMOTED "JDR" //You are no longer a mod
#define CMDINFO "UIS" //User info
#define CMDREG "UIR" //User is registered
#define CMDMOD "UIM" //User is a mod
#define CMDADMIN "UIA" //Admin is admin
#define CMDROOMBAN "UB" //User has been b&
#define CMDIDLETIMEOUT "RT" //You were outside of a room for too long.
#define CMDSERVERMAINTENANCE "S" //The server is going down for maintenance
#define CMDERROR "ERR" //Error
#define CMDSUCCEED "OK" //Operation was performed OK

std::vector<char> CreatePacket(std::string message);
int WriteMessage(std::string message);
std::vector<std::string> BestCommandStrip(std::vector<char> dBuf);
std::vector<std::string> GetCmdParams(std::string command);
DWORD WINAPI ServerListener(LPVOID pParam);
std::vector<char> PopQueue();
int StartListener(void);
void TeminateListener(void);
void ClearPacketQueue(void);
int CheckQueue();
bool ConnectionStatus();
int StartNuclearStrike(const char* location);

#endif