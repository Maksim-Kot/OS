#pragma warning(disable: 4996)
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
using std::cin;
using std::cout;
using std::ofstream;
using std::string;
int main(int argc, char* argv[]) 
{
	HANDLE hSemaphoreWrites = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (LPCWSTR)"SemaphoreWrites");
	if (hSemaphoreWrites == NULL)
	{
		cout << "Create semaphore failed.\n";
		cout << "Press any key to exit.\n";
		cin.get();
		return GetLastError();
	}

	HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, (LPCWSTR)"DemoMutex");
	if (hMutex == NULL)
	{
		cout << "Open mutex failed.\n";
		cout << "Press any key to exit.\n";
		cin.get();
		return GetLastError();
	}

	string s = argv[1];
	cout << s << "\n";

	ofstream out;
	out.open(s, ofstream::binary | std::ios_base::app);
	if(!out.is_open()) cout << "File not found.\n";
	string message;
	while (true) 
	{
		bool q = true;
		while (q)
		{
			cout << "Input message or input 'close' to stop:\n";
			cin >> message;
			if (message.size() > 20)
			{
				cout << "Message need be no more than 20 character. Try again.\n";
			}
			else q = false;
		}
		if (message == "close") 
		{
			break;
		}
		WaitForSingleObject(hSemaphoreWrites, INFINITE);
		char mess[21];
		strcpy(mess, message.c_str());
		WaitForSingleObject(hMutex, INFINITE);
		
		if(!out.is_open()) out.open(s, ofstream::binary | std::ios_base::app);
		out.write(mess, 21);
		out.close();
		ReleaseMutex(hMutex);
	}
	CloseHandle(hMutex);
	return 0;
}