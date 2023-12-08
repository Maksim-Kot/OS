#include <iostream>
#include <conio.h>
#include <windows.h>
#include "..\\Server\employee.h"
#pragma warning(disable: 4996)

char pipeName[30] = "\\\\.\\pipe\\pipe_name_";
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
    char eventName[50] = "READY_EVENT_";
    strcat(eventName, argv[1]);
    WCHAR* name = new WCHAR[strlen(eventName) + 1];
    mbstowcs(name, eventName, strlen(eventName));
    //std::cout << cmdargs << "\n";
    name[strlen(eventName)] = 0;
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
        char namePipe[30];
        strcpy(namePipe, pipeName);
        strcat(namePipe, argv[1]);
        std::cout << namePipe << "\n";

        Sleep(500);

        if (!WaitNamedPipeA(namePipe, NMPWAIT_WAIT_FOREVER))
        {
            std::cout << "5 second wait timed out." << "\n";
            std::cout << GetLastError() << "\n";
            getch();
            return 0;
        }
        hPipe = CreateFileA(namePipe, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, 0, NULL);
        //if valid, stop trying to connect
        if (INVALID_HANDLE_VALUE != hPipe) 
        {
            break;
        }
    }
    std::cout << "Connected to pipe." << "\n";
    SetEvent(hReadyEvent);
    messaging(hPipe);
    return 0;
}