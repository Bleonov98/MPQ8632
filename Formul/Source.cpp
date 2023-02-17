#include <iostream>
#include <string>
#include <math.h>
#include <windows.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <thread>

#define ESC "\x1b"
#define CSI "\x1b["

using namespace std;

enum formVar {
	RFREQ,
	FSW
};

void SetPos(int x, int y)
{
	char coord[50];

	sprintf_s(coord, "%s%d;%dH", CSI, y, x);
	printf(coord);
};

double FindRfreq(double& knownValue, double& Vin, double& Vout, double& Tdelay)
{
	double result = 0;

	result = ( 1 / ((6.1 * Vin) / ((Vin - 0.4) * Vout)) ) * ((pow(10,6) / knownValue) - Tdelay);

	return result;
};

double FindFsw(double& knownValue, double& Vin, double& Vout, double& Tdelay)
{
	double result = 0;

	result = pow(10, 6) / (((6.1 * knownValue) / (Vin - 0.4)) * (Vin / Vout) + Tdelay);

	return result;
};

int ChooseVar()
{
	int startX = 50, Rfreq = 20, Fsw = 22;
	int x = startX, y = Rfreq;

	bool choose = false;

	SetPos(x, y - 2);
	cout << "Что известно?";
	SetPos(x + 6, y);
	cout << "Rfreq";
	SetPos(x + 6, y + 2);
	cout << "Fsw";

	SetPos(x + 1, y);
	cout << "->";

	do
	{
		SetPos(x + 1, y);
		cout << "  ";

		if (GetAsyncKeyState(VK_DOWN) & 0x8000) y += 2;
		else if (GetAsyncKeyState(VK_UP) & 0x8000) y -= 2;

		if (y > Fsw) y = Rfreq;
		else if (y < Rfreq) y = Fsw;

		SetPos(x + 1, y);
		cout << "->";

		if (GetAsyncKeyState(VK_RETURN) & 0x8000) choose = true;

		Sleep(100);
	} while (!choose);

	if (y == Rfreq) return RFREQ;
	else if (y == Fsw) return FSW;
};

void EnterVar(int knownVar, double& knownValue, double& Vin, double& Vout, double& Tdelay)
{
	system("cls");

	Sleep(500);

	if (knownVar == RFREQ) cout << "Enter Pfreq: ";
	else if (knownVar == FSW) cout << "Enter Fsw: ";
	cin >> knownValue;
	
	if (knownValue < 0) {
		string strVal = to_string(knownValue);
		throw exception(strVal.c_str());
	}

	cout << "Enter Vin: ";
	cin >> Vin;
	cout << "Enter Vout: ";
	cin >> Vout;
	cout << "Enter Tdelay: ";
	cin >> Tdelay;
};

void OutResult(double result, string varName)
{
	system("cls");

	cout << varName << " = " << result;
};

int main()
{
	printf(CSI "?1049h");
	printf(CSI "?25l");

	setlocale(LC_ALL, "Russian");

	bool exitP = false, nextVar = false;

	thread exitProg([&exitP, &nextVar]() {
		while (!exitP)
		{
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) exitP = true;
			else exitP = false;

			if (GetAsyncKeyState(VK_RETURN) & 0x8000) nextVar = true;
			else nextVar = false;

			this_thread::sleep_for(chrono::milliseconds(5));
		}
		});

	do
	{
		system("cls");
		SetPos(0, 0);

		double Vin, Vout, Tdelay, knownValue;

		int knownVar = ChooseVar();

		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		try
		{
			EnterVar(knownVar, knownValue, Vin, Vout, Tdelay);

			if (knownVar == RFREQ) OutResult(FindFsw(knownValue, Vin, Vout, Tdelay), "Fsw");
			else if (knownVar == FSW) OutResult(FindRfreq(knownValue, Vin, Vout, Tdelay), "Rfreq");
		}
		catch (const exception &ex)
		{
			cout << "Value is less then 0" << endl;
			cout << "Value = " << ex.what();
		}

		SetPos(50, 25);
		cout << "Press Enter to continue" << endl;
		SetPos(50, 27);
		cout << "Press ESC to exit" << endl;

		if (_kbhit() == 0) _getch();
		Sleep(1000);

	} while (!exitP);

	exitProg.join();

	printf(CSI "?1049l");

	return 0;
}