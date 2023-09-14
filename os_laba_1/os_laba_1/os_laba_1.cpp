#include <iostream>
#include <conio.h>
#include <windows.h>
#include <string>
#include <cstring>
#include "..\Reporter\Reporter.h"
#pragma warning(disable: 4996)

void Console_report(char* name) 
{
	
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
	return 0;
}