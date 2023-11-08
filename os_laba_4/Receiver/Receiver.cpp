#pragma warning(disable : 4996)
#include <Windows.h>
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <conio.h>
#include <process.h>
using std::cin;
using std::cout;
using std::ifstream;
using std::fstream;
using std::string;
using std::to_string;


int main() 
{
	string filename;
	cout << "Input filename:\n";
	cin >> filename;
	int numOfEnters;
	cout << "Input num of enters:\n";
	cin >> numOfEnters;
	int numOfSenders;
	cout << "Input num of senders:\n";
	cin >> numOfSenders;
	std::ofstream out;
	out.open(filename, fstream::binary);
	out.close();

	HANDLE hSemaphoreWrites = CreateSemaphore(NULL, numOfEnters, numOfEnters, (LPCWSTR)"SemaphoreWrites");
	if (hSemaphoreWrites == NULL)
	{
		cout << "Create semaphore failed.\n";
		cout << "Press any key to exit.\n";
		cin.get();
		return GetLastError();
	}

	HANDLE hMutex = CreateMutex(NULL, FALSE, (LPCWSTR)"DemoMutex"); 
	if (hMutex == NULL)
	{
		cout << "Create mutex failed.\n";
		cout << "Press any key to exit.\n";
		cin.get();
		return GetLastError();
	}

	STARTUPINFO* si = new STARTUPINFO[numOfSenders];
	PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[numOfSenders];


	string arg = "Sender.exe " + filename;
	char args1[255];
	strcpy(args1, arg.c_str());
	cout << args1 << "\n";
	WCHAR* name_of_sender = new WCHAR[arg.size() + 1];
	mbstowcs(name_of_sender, args1, arg.size());
	name_of_sender[arg.size()] = 0;

	for (int i = 0; i < numOfSenders; i++) 
	{
		ZeroMemory(&si[i], sizeof(STARTUPINFO));
		si[i].cb = sizeof(STARTUPINFO);
		ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));


		if (!CreateProcess(NULL, name_of_sender, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si[i], &pi[i])) 
		{
			cout << GetLastError();
			ExitProcess(0);
		}
	}

	while (true/*WaitForMultipleObjects(numOfSenders, senders, TRUE, 0) == WAIT_TIMEOUT*/) 
	{
		cout << "Input 'read' to read file, or 'close' to stop the process:\n";
		string text;
		cin >> text;
		if (text == "close")
		{
			break;
		}
		WaitForSingleObject(hMutex, INFINITE);
		ifstream in;
		in.open(filename, fstream::binary);
		string message;
		in.seekg(0, in.end);
		int pos = in.tellg() / 21;
		in.seekg(in.beg);
		for (int i = 0; i < pos; ++i)
		{
			char mess[21];
			in.read(mess, 21);
			cout << mess << "\n";
		}
		in.close();
		ReleaseMutex(hMutex);
	}

	//// закрываем дескриптор мьютекса 
	//CloseHandle(hMutex);
	//// ждем пока дочерний процесс закончит работу 
	//WaitForSingleObject(pi.hProcess, INFINITE);
	//// закрываем дескрипторы дочернего процесса в текущем процессе 
	//CloseHandle(pi.hThread);
	//CloseHandle(pi.hProcess);


	for (int i = 0; i < numOfSenders; i++) 
	{
		CloseHandle(pi[i].hThread);
		CloseHandle(pi[i].hProcess);
	}
	CloseHandle(hMutex);
	return 0;
}