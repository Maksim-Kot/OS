#include <iostream>
#include <conio.h>
#include <windows.h>
#include <string>
#include <cstring>
#include "..\Reporter\Reporter.h"
#pragma warning(disable: 4996)

void Console_report(char* name) 
{
	std::ifstream in(name);
	std::string next_line;
	while (getline(in, next_line)) 
	{
		std::cout << next_line << std::endl;
	}
}

char* InputForCreator(std::istream& is, char* binfile_name, int& number) 
{
	std::cout << "The name of binary file: ";
	char str[50];
	std::cin >> str;
	strcpy(binfile_name, str);

	std::cout << "The number of employees: ";
	std::cin >> number;
	std::string s = std::to_string(number);
	char const* num = s.c_str();

	char* command_line = new char[strlen(binfile_name) + strlen(num) + 2];
	strcpy(command_line, binfile_name);
	strcat(command_line, " ");
	strcat(command_line, num);
	return command_line;
}

char* InputForReporter(std::istream& is, char* binfile_name, char* reportfile_name, double& payment_by_hour) 
{
	std::cout << "The name of report file: ";
	char str[50];
	std::cin >> str;
	strcpy(reportfile_name, str);

	std::cout << "The payment by hour: ";
	std::cin >> payment_by_hour;
	std::string s = std::to_string(payment_by_hour);
	char const* num = s.c_str();

	char* command_line = new char[strlen(binfile_name) + strlen(reportfile_name) + strlen(num) + 3];
	strcpy(command_line, binfile_name);
	strcat(command_line, " ");
	strcat(command_line, reportfile_name);
	strcat(command_line, " ");
	strcat(command_line, num);
	return command_line;
}



int main()
{
	char binfile_name[50];
	int number = 0;

	std::string creator_command_line = InputForCreator(std::cin, binfile_name, number);
	std::string arg1 = "Creator.exe " + creator_command_line;
	char args1[255];
	strcpy(args1, arg1.c_str());
	WCHAR* name_creator = new WCHAR[arg1.size()];
	mbstowcs(name_creator, args1, arg1.size());

	STARTUPINFO si1;
	PROCESS_INFORMATION piCom1;
	ZeroMemory(&si1, sizeof(STARTUPINFO));
	si1.cb = sizeof(STARTUPINFO);
	if (!CreateProcess(NULL, name_creator, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si1, &piCom1))
	{
		_cputs("The new process is not created.\n");
		_cputs("Check a name of the process.\n");
		_cputs("Press any key to finish.\n");
		_getch();
		return 0;
	}
	_cputs("The Creator process is created.\n");
	WaitForSingleObject(piCom1.hProcess, INFINITE);
	CloseHandle(piCom1.hThread);
	CloseHandle(piCom1.hProcess);

	char reportfile_name[50];
	double payment_by_hour = 0;

	std::string reporter_command_line = InputForReporter(std::cin, binfile_name, reportfile_name, payment_by_hour);
	std::string arg2 = "Reporter.exe " + reporter_command_line;
	char args2[255];
	strcpy(args2, arg2.c_str());
	WCHAR* name_reporter = new WCHAR[arg2.size()];
	mbstowcs(name_reporter, args2, arg2.size());

	STARTUPINFO si2;
	PROCESS_INFORMATION piCom2;
	ZeroMemory(&si2, sizeof(STARTUPINFO));
	si2.cb = sizeof(STARTUPINFO);
	if (!CreateProcess(NULL, name_reporter, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si2, &piCom2))
	{
		_cputs("The new process is not created.\n");
		_cputs("Check a name of the process.\n");
		_cputs("Press any key to finish.\n");
		_getch();
		return 0;
	}
	_cputs("The Reporter process is created.\n");
	WaitForSingleObject(piCom2.hProcess, INFINITE);
	CloseHandle(piCom2.hThread);
	CloseHandle(piCom2.hProcess);
	Console_report(reportfile_name);

	delete[] name_creator;
	delete[] name_reporter;

	return 0;
}