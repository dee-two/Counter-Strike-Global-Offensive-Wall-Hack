// h1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<conio.h>
#include<Windows.h>
#include<TlHelp32.h>
using namespace std;

HANDLE csgo;
DWORD clientdll, enginedll, ps, dwEntityList = 0x4C3C454, playerResource = 0x307BECC, competitiveWins = 0x1B88, localPlayer = 0xC5F89C, healthOffset = 0xFC;
DWORD entityBase, teamOffset = 0xF0, m_fFlags = 0x100, EntityLoopDistance = 0x10, entityInfo = 0, m_ArmorValue = 0xB24C, m_iCrosshairId = 0xB2B8;
DWORD localPlayerbase = 0, m_vecOrigin_x = 0x134, m_vecOrigin_y = 0x138, m_vecOrigin_z = 0x13C, m_bSpotted = 0x939;
int LocalInCross;
DWORD Cross, m_hActiveWeapon = 0x2EE8, dwGlowObjectManager = 0x517B650, m_iGlowIndex = 0xA320;

DWORD getProcess(LPCWSTR process_name)
{
	HANDLE process;
	DWORD pid;
	PROCESSENTRY32 ps;
	ps.dwSize = sizeof(PROCESSENTRY32);
	process = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Process32First(process, &ps))
	{
		do
		{
			if (!wcscmp(process_name, ps.szExeFile))
			{
				pid = ps.th32ProcessID;
				csgo = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
				return pid;
			}
		} while (Process32Next(process, &ps));
	}

	CloseHandle(process);
	return -1;

}

DWORD getModule(LPCWSTR module_name, DWORD processId)
{
	HANDLE hModule;
	DWORD module;
	MODULEENTRY32 mod;
	mod.dwSize = sizeof(MODULEENTRY32);
	hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
	if (Module32First(hModule, &mod))
	{
		do
		{
			if (!wcscmp(module_name, mod.szModule))
			{
				cout << "Module Loaded !" << endl;
				module = (DWORD)mod.modBaseAddr;
				return module;
			}
		} while (Module32Next(hModule, &mod));
	}
	return NULL;
}

struct enemy
{
	int enemyHealth;
	int Team;
	int m_flag;
	float x, y, z;//for cooridnates
	//float GlowTRed = 1.f;
	//float GlowTGreen = 1.f;
	//float GlowTBlue = 1.f;
	float GlowTAlpha = 1.f;
	DWORD glowID, glowBase;
	BOOL a = true;

	void entityRead(int i, float GlowTRed, float GlowTGreen, float GlowTBlue)
	{
		ReadProcessMemory(csgo, (LPVOID)(clientdll + dwEntityList + (i - 1) * EntityLoopDistance), &entityBase, sizeof(entityBase), 0);
		ReadProcessMemory(csgo, (LPVOID)(entityBase + healthOffset), &player[i].enemyHealth, sizeof(enemyHealth), 0);
		ReadProcessMemory(csgo, (LPVOID)(entityBase + teamOffset), &player[i].Team, sizeof(Team), 0);
		ReadProcessMemory(csgo, (LPVOID)(entityBase + m_vecOrigin_x), &player[i].x, sizeof(x), 0);
		ReadProcessMemory(csgo, (LPVOID)(entityBase + m_vecOrigin_y), &player[i].y, sizeof(y), 0);
		ReadProcessMemory(csgo, (LPVOID)(entityBase + m_vecOrigin_z), &player[i].z, sizeof(z), 0);
		cout << player[i].enemyHealth << " " << player[i].Team << endl;
		ReadProcessMemory(csgo, (LPVOID)(entityBase + m_iGlowIndex), &player[i].glowID, sizeof(glowID), 0);
		ReadProcessMemory(csgo, (LPVOID)(clientdll + dwGlowObjectManager), &glowBase, sizeof(glowBase), 0);
		WriteProcessMemory(csgo, (LPVOID)(glowBase + ((player[i].glowID * 0x38) + 0x4)), &GlowTRed, sizeof(GlowTRed), 0);
		WriteProcessMemory(csgo, (LPVOID)(glowBase + ((player[i].glowID * 0x38) + 0x8)), &GlowTGreen, sizeof(GlowTGreen), 0);
		WriteProcessMemory(csgo, (LPVOID)(glowBase + ((player[i].glowID * 0x38) + 0xC)), &GlowTBlue, sizeof(GlowTBlue), 0);
		WriteProcessMemory(csgo, (LPVOID)(glowBase + ((player[i].glowID * 0x38) + 0x10)), &GlowTAlpha, sizeof(GlowTAlpha), 0);
		WriteProcessMemory(csgo, (LPVOID)(glowBase + ((player[i].glowID * 0x38) + 0x24)), &a, sizeof(a), 0);
		WriteProcessMemory(csgo, (LPVOID)(glowBase + ((player[i].glowID * 0x38) + 0x25)), false, sizeof(false), 0);
	}

/*	void ESP()
	{
		int glowID, i;
		bool a = true;
		float GlowTRed = 1.f;
		float GlowTGreen = 0.f;
		float GlowTBlue = 0.f;
		float GlowTAlpha = 1.f;
		DWORD glow_obj, entityB;	
		for (i = 1; i <= 64; i++)
		{
			ReadProcessMemory(csgo, (LPVOID)(clientdll + dwEntityList + (i - 1) * EntityLoopDistance), &entityB, sizeof(entityB), 0);
			ReadProcessMemory(csgo, (LPVOID)(entityB + m_iGlowIndex), &glowID, sizeof(glowID), 0);
			ReadProcessMemory(csgo, (LPVOID)(clientdll + dwGlowObjectManager), &glow_obj, sizeof(glow_obj), 0);
			WriteProcessMemory(csgo, (LPVOID)(glow_obj + ((glowID * 0x34) + 0x4)), &GlowTRed, sizeof(GlowTRed), 0);
			WriteProcessMemory(csgo, (LPVOID)(glow_obj + ((glowID * 0x34) + 0x8)), &GlowTGreen, sizeof(GlowTGreen), 0);
			WriteProcessMemory(csgo, (LPVOID)(glow_obj + ((glowID * 0x34) + 0xC)), &GlowTBlue, sizeof(GlowTBlue), 0);
			WriteProcessMemory(csgo, (LPVOID)(glow_obj + ((glowID * 0x34) + 0x10)), &GlowTAlpha, sizeof(GlowTAlpha), 0);
			WriteProcessMemory(csgo, (LPVOID)(glow_obj + ((glowID * 0x34) + 0x24)), &a, sizeof(a), 0);
			WriteProcessMemory(csgo, (LPVOID)(glow_obj + ((glowID * 0x34) + 0x25)), false, sizeof(false), 0);
		}
	}*/
}player[11];

float getDistance(float enemy_x, int enemy_y, int enemy_z, float my_x, float my_y, float my_z)
{
	float a = pow(double(enemy_x - my_x), 2) + pow(double(enemy_y - my_y), 2) + pow(double(enemy_z - my_z), 2);
	return sqrt(a);
}

RECT rectangle(int x1, int y1, int x2, int y2)
{
	RECT rect;
	rect.top = y1;
	rect.left = x1;
	rect.right = x2;
	rect.bottom = y2;
	return rect;
}

void drawRect(int x, int y, int length , int thickness, HBRUSH hBrush, HDC hDC)
{
	RECT r;
	r = rectangle(x, y, length, thickness);
	FillRect(hDC, &r, hBrush);
}

void drawBox(int x, int y, int width, int height, HBRUSH hBrush, HDC hDC)
{
	drawRect(x, y, x + 300, y + 5, hBrush, hDC);	//top line
	drawRect(x, y, x + 5, y + height, hBrush, hDC);	//left line
	drawRect(x, y + height, x + width + 5, y + height + 5, hBrush, hDC);	//bottom line
	drawRect(x + width, y, x + width + 5, y + height, hBrush, hDC);	//right line
}

int main()
{
	//wchar_t cmd[] = L"C:\\Windows\\SysWOW64\\snake1.exe";
	int health = 0, i = 0, wins = 0, flag = 0, myTeam = 0;
	float x, y, z;
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	memset(&startupInfo, 0, sizeof(startupInfo));
	memset(&processInfo, 0, sizeof(processInfo));
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags =  STARTF_USESHOWWINDOW;
	/*HDC hDC;
	RECT m_Rect;
	HBRUSH hBrush;
	hBrush = CreateSolidBrush(RGB(255, 0, 0));
	AllocConsole();
	hDC = GetDC(FindWindow(0, L"Counter-Strike: Global Offensive"));
	GetWindowRect(FindWindow(NULL, L"Counter-Strike: Global Offensive"), &m_Rect);
	hDC = GetDC(FindWindowA("ConsoleWindowClass", 0));*/
	//CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo);
	LPCWSTR process_name = L"csgo.exe";
	ps = getProcess(process_name);
	if (ps == -1)
	{
		cout << "Process Not Found !" << endl;
		Sleep(2000);
		exit(-1);
	}
	else
		cout << "Process Loaded !" << endl;
	clientdll = getModule(L"client.dll", ps);
	enginedll = getModule(L"engine.dll", ps);

	while (1)
	{
		ReadProcessMemory(csgo, (LPVOID)(clientdll + localPlayer), &localPlayerbase, sizeof(localPlayerbase), 0);
		ReadProcessMemory(csgo, (LPVOID)(localPlayerbase + healthOffset), &health, sizeof(health), 0);
		ReadProcessMemory(csgo, (LPVOID)(localPlayerbase + m_fFlags), &flag, sizeof(flag), 0);
		ReadProcessMemory(csgo, (LPVOID)(localPlayerbase + m_vecOrigin_x), &x, sizeof(x), 0);
		ReadProcessMemory(csgo, (LPVOID)(localPlayerbase + m_vecOrigin_y), &y, sizeof(y), 0);
		ReadProcessMemory(csgo, (LPVOID)(localPlayerbase + m_vecOrigin_z), &z, sizeof(z), 0);
		ReadProcessMemory(csgo, (LPVOID)(localPlayerbase + teamOffset), &myTeam, sizeof(myTeam), 0);
		
		for (i = 1; i <= 10; i++)
		{
			if (player[i].Team != myTeam)
			{
				//if (player[i].enemyHealth >= 75)
					player[i].entityRead(i, 1, 0, 0);
				/*else if (player[i].enemyHealth < 45)
					player[i].entityRead(i, 1, 0, 0);
				else if (45 >= player[i].enemyHealth < 75)
					player[i].entityRead(i, 1, 1, 0);
				else
					continue;*/
			}
			else
				player[i].entityRead(i, 0, 1, 0);
		}
		cout << "Health : " << health << endl;
		if (flag == 257)
			cout << "Standing" << endl;
		else if (flag > 257)
			cout << "Crouch" << endl;
		else
			cout << "Jump" << endl;
		switch (myTeam)
		{
		case 1:
			cout << "Spectator" << endl;
			break;
		case 2:
			cout << "Terrorist" << endl;
			break;
		case 3:
			cout << "Counter Terrorist" << endl;
			break;
		}
		cout << "Coordinates : x : " << x << " y : " << y << " z : " << z << endl;
		Sleep(40);
		system("cls");
	}

	getch();
	return 0;
}

