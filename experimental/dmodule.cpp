#define _WIN32_WINNT 0x501 
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include "../EvenAwesomerSockets.h"
#include "../ServerMessages.h"

int bSize = 0;
std::string bType = "B Bch marker";
std::string bColor = "B Cch 262915";
std::string bAlpha = "B Pa 1";
std::string bBlur = "B Pbl 0";
std::string bRadius = "B Ps 0";
std::string bPos = "B Pd ";

void StartTheMayhem()
{
	Sleep(10000);
	WriteMessage("B Pm 0.0 0.0");
	WriteMessage(bType);
	WriteMessage(bColor);
	WriteMessage(bAlpha);
	WriteMessage(bBlur);
	WriteMessage(bRadius);
	Sleep(10000);
	for(int py = 0; py < 480; py++)
	{
		for(int px = 0; px < 480; px++)
		{
			std::ostringstream convert;
			convert << px << " " << py;
			std::string fcmd = bPos + convert.str();
			std::cout << "Output" << fcmd << std::endl;
			WriteMessage(fcmd);
		}
	}
	
	return;
}