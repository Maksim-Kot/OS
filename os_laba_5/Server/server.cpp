#include <iostream>
#include <conio.h>
#include <fstream>
#include <string>
#include <time.h>
#include <algorithm>
#include <process.h>
#include <windows.h>
#include "employee.h"
#pragma warning(disable: 4996)

int empCount;
employee* emps;
HANDLE* hReadyEvents;
HANDLE* hThreads;
HANDLE* hPipeArray;
HANDLE* hProcess;
CRITICAL_SECTION empsCS;
bool* empIsModifying;
char pipeName[30] = "\\\\.\\pipe\\pipe_name_";
const int MESSAGE_SIZE = 10;

void sortEmps() 
{
    qsort(emps, empCount, sizeof(employee), empCmp);
}

void writeData(const std::string& fileName) 
{
    std::fstream fin(fileName.c_str(), std::ios::binary | std::ios::out);
    fin.write(reinterpret_cast<char*>(emps), sizeof(employee) * empCount);
    fin.close();
}

void readDataSTD() 
{
    emps = new employee[empCount];
    std::cout << "Enter ID, name and working hours of each employee:\n";
    for (int i = 1; i <= empCount; ++i) {
        std::cout << "#" << i << ">";
        std::cin >> emps[i - 1].num >> emps[i - 1].name >> emps[i - 1].hours;
    }
}

employee* findEmp(int id) 
{
    employee key;
    key.num = id;
    return reinterpret_cast<employee*>(bsearch(reinterpret_cast<const char*>(&key), reinterpret_cast<const char*>(emps),
        empCount, sizeof(employee), empCmp));
}

void startPocesses(int count) 
{
    char buff[10];
    hProcess = new HANDLE[count];
    for (int i = 0; i < count; i++) 
    {
        char cmdargs[80] = "Client.exe ";
        char eventName[50] = "READY_EVENT_";
        itoa(i + 1, buff, MESSAGE_SIZE);
        strcat(eventName, buff);
        strcat(cmdargs, buff);
        WCHAR* name = new WCHAR[strlen(cmdargs) + 1];
        mbstowcs(name, cmdargs, strlen(cmdargs));
        std::cout << cmdargs <<"\n";
        name[strlen(cmdargs)] = 0;
        STARTUPINFO* si = new STARTUPINFO[count];
        PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[count];
        ZeroMemory(&si[i], sizeof(STARTUPINFO));
        si[i].cb = sizeof(STARTUPINFO);
        ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));
        WCHAR* nameEvent = new WCHAR[strlen(eventName) + 1];
        mbstowcs(nameEvent, eventName, strlen(eventName));
        nameEvent[strlen(eventName)] = 0;
        hReadyEvents[i] = CreateEvent(NULL, TRUE, FALSE, nameEvent);
        if (!CreateProcess(NULL, name, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
            NULL, NULL, &si[i], &pi[i])) 
        {
            std::cout << GetLastError();
            ExitProcess(0);
        }
        hProcess[i] = pi[i].hProcess;
    }
}

DWORD WINAPI messaging(LPVOID p) 
{
    HANDLE hPipe = (HANDLE)p;
    //getting emp with id -1 means for client that error occurred
    employee* errorEmp = new employee;
    errorEmp->num = -1;
    while (true) 
    {
        DWORD readBytes;
        char message[MESSAGE_SIZE];
        //receiving a message
        bool isRead = ReadFile(hPipe, message, MESSAGE_SIZE, &readBytes, NULL);
        if (!isRead) 
        {
            if (ERROR_BROKEN_PIPE == GetLastError()) 
            {
                std::cout << "Client disconnected.\n";
                break;
            }
            else 
            {
                std::cerr << "Error in reading a message.\n";
                break;
            }
        }
        //sending answer
        if (strlen(message) > 0) 
        {
            char command = message[0];
            message[0] = ' ';
            int id = atoi(message);
            DWORD bytesWritten;
            EnterCriticalSection(&empsCS);
            employee* empToSend = findEmp(id);
            LeaveCriticalSection(&empsCS);
            if (NULL == empToSend) 
            {
                empToSend = errorEmp;
            }
            else 
            {
                int ind = empToSend - emps;
                if (empIsModifying[ind])
                    empToSend = errorEmp;
                else 
                {
                    switch (command) 
                    {
                    case 'w':
                        printf("Requested to modify ID %d.", id);
                        empIsModifying[ind] = true;
                        break;
                    case 'r':
                        printf("Requested to read ID %d.", id);
                        break;
                    default:
                        std::cout << "Unknown request. ";
                        empToSend = errorEmp;
                    }
                }
            }
            bool isSent = WriteFile(hPipe, empToSend, sizeof(employee), &bytesWritten, NULL);
            if (isSent)
                std::cout << " Answer is sent." << "\n";
            else
                std::cout << "Error in sending answer." << "\n";
            //receiving a changed record
            if ('w' == command && empToSend != errorEmp) 
            {
                isRead = ReadFile(hPipe, empToSend, sizeof(employee), &readBytes, NULL);
                if (isRead) 
                {
                    std::cout << "Employee record changed." << "\n";
                    empIsModifying[empToSend - emps] = false;
                    EnterCriticalSection(&empsCS);
                    sortEmps();
                    LeaveCriticalSection(&empsCS);
                }
                else 
                {
                    std::cerr << "Error in reading a message." << "\n";
                    break;
                }
            }
        }
    }
    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    delete errorEmp;
    return 0;
}

void openPipes(int clientCount) 
{
    hPipeArray = new HANDLE[clientCount];
    hThreads = new HANDLE[clientCount];
    for (int i = 0; i < clientCount; i++) 
    {
        char namePipe[30];
        strcpy(namePipe, pipeName);
        char buff[10];
        itoa(i + 1, buff, MESSAGE_SIZE);
        strcat(namePipe, buff);
        hPipeArray[i] = CreateNamedPipeA(namePipe, PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES, 0, 0, INFINITE, NULL);
        if (INVALID_HANDLE_VALUE == hPipeArray[i]) 
        {
            std::cerr << "Create named pipe failed." << "\n";
            std::cout << GetLastError();
            getch();
            return;
        }
        
    }
    for (int i = 0; i < clientCount; i++)
    {
        if (!ConnectNamedPipe(hPipeArray[i], NULL) && ERROR_PIPE_CONNECTED != GetLastError())
        {
            std::cout << "No connected clients." << i + 1 << "\n";
            std::cout << GetLastError() << "\n";
            throw;
        }
        hThreads[i] = CreateThread(NULL, 0, messaging, static_cast<LPVOID>(hPipeArray[i]), 0, NULL);
    }
    std::cout << "Clients connected to pipe." << "\n";
    WaitForMultipleObjects(clientCount, hThreads, TRUE, INFINITE);
}

int main() {
    //data input
    char filename[50];
    std::cout << "Enter the file name and the count of employees. \n>";
    std::cin >> filename >> empCount;
    readDataSTD();
    writeData(filename);
    sortEmps();

    //creating processes
    InitializeCriticalSection(&empsCS);
    int clientCount;
    std::cout << "Enter amount of clients. \n>";
    std::cin >> clientCount;
    HANDLE hstartALL = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"START_ALL");
    empIsModifying = new bool[empCount];
    for (int i = 0; i < empCount; ++i)
        empIsModifying[i] = false;
    hReadyEvents = new HANDLE[clientCount];
    startPocesses(clientCount);
    WaitForMultipleObjects(clientCount, hReadyEvents, TRUE, INFINITE);
    std::cout << "All processes are ready. Starting." << "\n";
    SetEvent(hstartALL);

    //creating pipes
    openPipes(clientCount);

    WaitForMultipleObjects(clientCount, hProcess, TRUE, INFINITE);
    for (int i = 0; i < empCount; i++)
        emps[i].print(std::cout);
    std::cout << "Press any key to exit." << "\n";
    getch();
    DeleteCriticalSection(&empsCS);
    delete[] empIsModifying;
    delete[] hReadyEvents;
    delete[] emps;
    delete[] hThreads;
    delete[] hPipeArray;
    return 0;
}