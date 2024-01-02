#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <fstream>
#pragma warning(disable: 4996)

using std::cin;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::string;
using std::vector;

HANDLE CreateWritesSemaphore(int numOfEnters)
{
    HANDLE hSemaphoreWrites = CreateSemaphore(NULL, numOfEnters, numOfEnters, (LPCWSTR)"SemaphoreWrites");
    return hSemaphoreWrites;
}

HANDLE CreateReadySemaphore(int numOfEnters)
{
    HANDLE hSemaphoreReady = CreateSemaphore(NULL, 0, numOfEnters, (LPCWSTR)"SemaphoreReady");
    return hSemaphoreReady;
}

HANDLE CreateDemoMutex()
{
    return CreateMutex(NULL, FALSE, (LPCWSTR)"DemoMutex");
}

HANDLE CreateDemoEvent()
{
    return CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"Event");
}

bool OpenFile(ifstream& in, const string& filename)
{
    in.open(filename, std::ofstream::binary);
    return in.is_open();
}

bool CreateSenderProcesses(STARTUPINFO* si, PROCESS_INFORMATION* pi, const string& filename, int numOfSenders)
{
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
            return false; // Return false if process not created
        }
    }

    delete[] name_of_sender;

    return true; // Return true for simplicity
}

void TerminateSenderProcesses(PROCESS_INFORMATION* pi, int numOfSenders)
{
    for (int i = 0; i < numOfSenders; i++)
    {
        TerminateProcess(pi[i].hProcess, 0);
        CloseHandle(pi[i].hThread);
        CloseHandle(pi[i].hProcess);
    }
}

vector<string> ReadFile(ifstream& in)
{
    vector<string> fileContent;
    string message;
    in.seekg(0, in.end);
    int pos = in.tellg() / 21;
    in.seekg(in.beg);
    for (int i = 0; i < pos; ++i)
    {
        char mess[21];
        in.read(mess, 21);
        string s = mess;
        fileContent.push_back(s);
    }
    return fileContent;
}

void WriteFile(ofstream& out, const vector<string>& content)
{
    for (int i = 1; i < content.size(); ++i)
    {
        char mess[21];
        strcpy(mess, content[i].c_str());
        out.write(mess, 21);
    }
}


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


    HANDLE hSemaphoreWrites = CreateWritesSemaphore(numOfEnters);
    if (NULL == hSemaphoreWrites)
    {
        cout << "Create writes semaphore failed.\n";
        cout << "Press any key to exit.\n";
        cin.get();
        return GetLastError();
    }

    HANDLE hSemaphoreReady = CreateReadySemaphore(numOfEnters);
    if (NULL == hSemaphoreReady)
    {
        cout << "Create ready semaphore failed.\n";
        cout << "Press any key to exit.\n";
        cin.get();
        return GetLastError();
    }

    HANDLE hMutex = CreateDemoMutex();
    if (NULL == hMutex)
    {
        cout << "Create mutex failed.\n";
        cout << "Press any key to exit.\n";
        cin.get();
        return GetLastError();
    }

    HANDLE hEvent = CreateDemoEvent();
    if (NULL == hEvent)
    {
        cout << "Create event failed.\n";
        cout << "Press any key to exit.\n";
        cin.get();
        return GetLastError();
    }

    STARTUPINFO* si = new STARTUPINFO[numOfSenders];
    PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[numOfSenders];
    if (!CreateSenderProcesses(si, pi, filename, numOfSenders))
    {
        cout << "Create processes failed.\n";
        return GetLastError();
    }

    vector<string> fileContent; // Vector for saving information on file

    WaitForSingleObject(hSemaphoreReady, INFINITE);

    while (true)
    {
        cout << "Input 'read' to read file, or 'close' to stop the process:\n";
        string text;
        cin >> text;
        if (text == "close")
        {
            break;
        }
        else if (text == "read")
        {
            ifstream in;
            if (!OpenFile(in, filename))
            {
                cout << GetLastError();
                ExitProcess(0);
            }
            if (in.peek() == ifstream::traits_type::eof())
            {
                ResetEvent(hEvent);
                WaitForSingleObject(hEvent, INFINITE);
                ResetEvent(hEvent);
            }
            WaitForSingleObject(hMutex, INFINITE);
            fileContent = ReadFile(in); // Get information from file
            in.close();


            ofstream out;
            out.open(filename, ofstream::binary | ofstream::out | ofstream::trunc);
            WriteFile(out, fileContent); // Write information without first message
            out.close();

            cout << fileContent[0] << "\n";
            ReleaseSemaphore(hSemaphoreWrites, 1, NULL);
            ReleaseMutex(hMutex);
        }
        else cout << "Input error. Try again.\n";
    }

    TerminateSenderProcesses(pi, numOfSenders);

    CloseHandle(hSemaphoreWrites);
    CloseHandle(hSemaphoreReady);
    CloseHandle(hMutex);
    CloseHandle(hEvent);

    delete[] pi;
    delete[] si;

    return 0;
}
