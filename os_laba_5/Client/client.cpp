#include <iostream>
#include <conio.h>
#include <windows.h>
#include "..\\Server\employee.h"
#pragma warning(disable: 4996)

const char pipeName[30] = "\\\\.\\pipe\\pipe_name";
const int MESSAGE_SIZE = 10;

const int CONNECTION_WAIT_TIME = 5000;

void messaging(HANDLE hPipe) 
{
    std::cout << "\n\nTo quit press Ctrl+Z" << "\n";
    while (true) 
    {
        char command[MESSAGE_SIZE];
        std::cout << "Enter r/w (to read/write) command and ID of employee: \n>";
        std::cin.getline(command, MESSAGE_SIZE, '\n');
        if (std::cin.eof()) 
        {
            return;
        }
        DWORD bytesWritten;
        bool isSent = WriteFile(hPipe, command, MESSAGE_SIZE, &bytesWritten, NULL);
        if (!isSent) 
        {
            std::cerr << "Message cannot be sent" << "\n";
            getch();
            return;
        }
        //receiving the answer
        employee emp;
        DWORD readBytes;
        bool isRead = ReadFile(hPipe, &emp, sizeof(emp), &readBytes, NULL);
        if (!isRead) 
        {
            std::cerr << "Error in receiving answer." << "\n";
        }
        else 
        {
            if (emp.num < 0) 
            {
                std::cerr << "Employee not found or being modified." << "\n";
                continue;
            }
            else 
            {
                //if everything is correct, received an emp
                emp.print(std::cout);
                if ('w' == command[0]) 
                {
                    std::cout << "Enter ID, name and working hours of the employee:\n>" << std::flush;
                    std::cin >> emp.num >> emp.name >> emp.hours;
                    std::cin.ignore(2, '\n');
                    isSent = WriteFile(hPipe, &emp, sizeof(emp), &bytesWritten, NULL);
                    if (isSent)
                        std::cout << "New record is sent to the server" << "\n";
                    else 
                    {
                        std::cerr << "Error in sending." << "\n";
                        getch();
                        break;
                    }
                }
            }
        }
    }
}

int main(int argc, char* argv[]) 
{
    //std::cout << argv[1] << "\n";
    WCHAR* name = new WCHAR[strlen(argv[1]) + 1];
    mbstowcs(name, argv[1], strlen(argv[1]));
    //std::cout << cmdargs << "\n";
    name[strlen(argv[1])] = 0;
    //HANDLE hReadyEvent = CreateEvent(NULL, TRUE, FALSE, name);
    HANDLE hReadyEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, name);
    HANDLE hStartEvent = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"START_ALL");
    //OpenEvent(SYNCHRONIZE, FALSE, (LPCWSTR)"START_ALL");
    if (NULL == hStartEvent) 
    {
        std::cerr << "Error in opening start Event." << "\n";
        std::cout << GetLastError() << "\n";
        getch();
        return GetLastError();
    }
    if (NULL == hReadyEvent)
    {
        std::cerr << "Error in opening ready Event." << "\n";
        std::cout << GetLastError() << "\n";
        getch();
        return GetLastError();
    }
    SetEvent(hReadyEvent);
    std::cout << "Process is ready." << "\n";
    WaitForSingleObject(hStartEvent, INFINITE);
    std::cout << "Process is started." << "\n";

    //connecting to pipe
    HANDLE hPipe;
    while (true) 
    {
        hPipe = CreateFile(L"\\\\.\\pipe\\pipe_name", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, 0, NULL);
        //if valid, stop trying to connect
        if (INVALID_HANDLE_VALUE != hPipe) 
        {
            break;
        }
        if (!WaitNamedPipe(L"\\\\.\\pipe\\pipe_name", NMPWAIT_USE_DEFAULT_WAIT))
        {
            std::cout << "5 second wait timed out." << "\n";
            getch();
            return 0;
        }
    }
    std::cout << "Connected to pipe." << "\n";
    messaging(hPipe);
    return 0;
}