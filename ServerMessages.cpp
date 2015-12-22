#define _WIN32_WINNT 0x501 //This should be on windows.h
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ctime>
#include "EvenAwesomerSockets.h"
#include "ServerMessages.h"

CRITICAL_SECTION mcs;
HANDLE thread = 0L;
bool terminateFlag = false;
std::vector<std::vector<char>> bQueue;
std::ofstream dfile;
bool connectionTimeOut = false;
//Function to create a packet
std::vector<char> CreatePacket(std::string message)
{
	int dataLength = message.length();
	char* dlPtr = (char*)&dataLength;
	std::vector<char> myPacket(message.length()+5);
	myPacket[0] = 0x17; //Still not sure if this is correct at all, but it works this way
	myPacket[1] = 0x0A;
	myPacket[2] = dlPtr[2]; //little-endian format for the size
	myPacket[3] = dlPtr[1];
	myPacket[4] = dlPtr[0];
	std::copy(message.begin(), message.end(), myPacket.begin()+5); //Copy the data
	return myPacket;
}

//Function to send a message to the server
int WriteMessage(std::string message)
{
		//Create the packet.
		std::vector<char> packet = CreatePacket(message);
		//Send it through the current socket.
		int datasent = send(NetworkManager::GetSocket(),(char*)&packet[0],packet.size(),0);
		//Done.
		return datasent;
}

//Strip a data buffer into different string commands.
//It's called 'Best' because there was a similar function that was complete garbage.
//Ninji saw it and ate it.
std::vector<std::string> BestCommandStrip(std::vector<char> dBuf)
{
	std::vector<std::string> results; //<- this vector hold the commands
	std::string res;
	int cnt = 0;
	int size = 0;
	
	for(unsigned int x = 0; x < dBuf.size(); x++)
	{
		if(dBuf[x] != 0x17 && dBuf[x] >= 0x20 && dBuf[x] <= 0x7E)
		{
			res.append(1,dBuf[x]);
			size++;
		}
		if(dBuf[x] == 0x17 || x == dBuf.size()-1)
		{
			if(res.length() > 0)
				results.push_back(res);
			res.clear();
			x+=4;
			size=0;
		}
	}
	return results;
}

//Divide a string command and it's parameters into different strings
std::vector<std::string> GetCmdParams(std::string command)
{
	std::vector<std::string> result;
	std::string tmp;
	for(unsigned int x = 0; x < command.length(); x++)
	{
		if(command[x] > 0x20 && command[x] <= 0x7E)
			tmp += command[x];
		else if(command[x] == 0x20 || x == command.length()-1)
		{
			if(tmp.length() > 0)
				result.push_back(tmp);
			tmp.clear();
		}
	}
	if(tmp.length() > 0)
		result.insert(result.end(), tmp);
	return result;
}

/* Parallel thread in charge of managing the incoming packets.
Since the main process is too busy with the user interface and handling all the
incoming packets, I though it was a good idea to separate the listener from the
main process, into a new parallel process. And it seems to work pretty well.*/
DWORD WINAPI ServerListener(LPVOID pParam){
	UNREFERENCED_PARAMETER(pParam);
	std::vector<char> mainBuffer;
	clock_t stt, endt, difft;
	double tsecs = 0;
	int bRec = 0;
	
	#ifdef _debug_
	char endpkg[]={(char)0xFF,(char)0xFF};
	dfile.open("data.bin", std::ios::binary | std::ios::out);
	#endif
	
	stt = std::clock();
	do{
		mainBuffer.clear();
		mainBuffer.resize(MAXBUFLEN);
		bRec = recv(NetworkManager::GetSocket(), &mainBuffer[0], mainBuffer.size(), 0);
		//Check if we received something.
		if(bRec > 0)
		{
			mainBuffer.resize(bRec);
			//Automatically handle and reply to any PING request.
			if(mainBuffer[5] == 'P' && mainBuffer[6] == 'I')
			{
				WriteMessage("PO");
				stt = std::clock();
				continue;
			}
				//Add the packet to the queue, using SEMAPHORES to avoid possible data races.
				EnterCriticalSection(&mcs);
				bQueue.push_back(mainBuffer);
				LeaveCriticalSection(&mcs);
			//If this is a debug executable, it also prints the content of the buffer in a log file.
			//Use an hex editor to inspect it.
			#ifdef _debug_
				dfile.write(&mainBuffer[0], mainBuffer.size());
				dfile.write(endpkg, (std::streamsize)2);
				std::string bufStr(mainBuffer.begin()+5, mainBuffer.end());
				std::cout << bufStr << std::endl;
			#endif
				stt = std::clock();
		}else{
			endt = std::clock();
			difft = endt - stt;
			tsecs = difft / (double) CLOCKS_PER_SEC;
			if(tsecs > MAX_TIMEOUT)
			{
				connectionTimeOut = true;
				terminateFlag = true;
			}
			Sleep(1); //If nothing is to be processed, we might as well take a break or something.
		}
	}while(!terminateFlag);
	#ifdef _debug_
		dfile.close();
	#endif
}

//Pop the first element of the queue. The queue is FIFO.
std::vector<char> PopQueue()
{
	std::vector<char> qEl = bQueue.front();
	bQueue.erase(bQueue.begin());
	return qEl;
}

//Start the incoming connection listener.
int StartListener()
{
	InitializeCriticalSection(&mcs);
	thread = CreateThread(NULL, 0L, ServerListener, NULL, 0, NULL);
	return (int)thread;
}

//Terminate the incoming connection listener.
void TeminateListener()
{
	terminateFlag = true;
	Sleep(1); //Wait for pending instructions to be processed
	CloseHandle(thread); //Cleanup
	return;
}

//Clear the queue
void ClearPacketQueue()
{
	bQueue.clear();
	return;
}

//Check if there is something in the queue to be processed.
int CheckQueue()
{
	return bQueue.size();
}

//Return the status of the connection. TRUE means timeout.
bool ConnectionStatus()
{
	return connectionTimeOut;
}

//Nerd jokes.
int StartNuclearStrike(const char* location)
{
	//LaunchNukesAt(location); :)
	return (int)EXIT_SUCCESS;
}