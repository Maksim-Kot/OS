#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#pragma warning(disable: 4996)

using std::cin;
using std::cout;
using std::ofstream;
using std::string;

HANDLE OpenWritesSemaphore()
{
    return OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (LPCWSTR)"SemaphoreWrites");
}

HANDLE OpenReadySemaphore()
{
    HANDLE hSemaphoreReady = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (LPCWSTR)"SemaphoreReady");
    return hSemaphoreReady;
}

HANDLE OpenDemoMutex()
{
    return OpenMutex(SYNCHRONIZE, FALSE, (LPCWSTR)"DemoMutex");
}

HANDLE OpenDemoEvent()
{
    return CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"Event");
}

bool OpenFile(ofstream& out, const string& filename)
{
    out.open(filename, std::ofstream::binary | std::ios_base::app);
    return out.is_open();
}


int main(int argc, char* argv[])
{
    string filename = argv[1];
    cout << filename << "\n";


    HANDLE hSemaphoreWrites = OpenWritesSemaphore();
    if (NULL == hSemaphoreWrites)
    {
        cout << "Open writes semaphore failed.\n";
        cout << "Press any key to exit.\n";
        cin.get();
        return GetLastError();
    }

    HANDLE hSemaphoreReady = OpenReadySemaphore();
    if (NULL == hSemaphoreReady)
    {
        cout << "Create ready semaphore failed.\n";
        cout << "Press any key to exit.\n";
        cin.get();
        return GetLastError();
    }

    HANDLE hMutex = OpenDemoMutex();
    if (NULL == hMutex)
    {
        cout << "Open mutex failed.\n";
        cout << "Press any key to exit.\n";
        cin.get();
        return GetLastError();
    }

    HANDLE hEvent = OpenDemoEvent();
    if (NULL == hEvent)
    {
        cout << "Open event failed.\n";
        cout << "Press any key to exit.\n";
        cin.get();
        return GetLastError();
    }

    string message;
    ofstream out;
    ReleaseSemaphore(hSemaphoreReady, 1, NULL);

    while (true)
    {
        bool q = true;
        cout << "Input 'send' to send your message or input 'close' to stop:\n";
        cin >> message;
        if (message == "close")
        {
            break;
        }
        else if (message == "send")
        {
            while (q)
            {
                WaitForSingleObject(hSemaphoreWrites, INFINITE);
                cout << "Input your message (it must not be more than 20 characters):\n";
                cin >> message;
                if (message.size() > 20)
                {
                    cout << "The message must not be more than 20 characters. Try again.\n";
                    ReleaseSemaphore(hSemaphoreWrites, 1, NULL);
                }
                else
                {
                    q = false; // Make false to exit from while and write message
                }
            }

            char mess[21];
            strcpy(mess, message.c_str());
            WaitForSingleObject(hMutex, INFINITE);

            if (!OpenFile(out, filename))
            {
                cout << "Open file failed.\n";
                return GetLastError();
            }
            out.write(mess, 21);
            out.close();
            ReleaseMutex(hMutex);
            SetEvent(hEvent);
        }
        else
        {
            cout << "Input error. Try again.\n";
        }
    }


    CloseHandle(hSemaphoreWrites);
    CloseHandle(hSemaphoreReady);
    CloseHandle(hMutex);
    CloseHandle(hEvent);

    return 0;
}
