#include "pch.h"
#include "CppUnitTest.h"
#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#pragma warning(disable: 4996)

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::ifstream;
using std::ofstream;
using std::fstream;
using std::string;
using std::vector;

vector<string> ReadFile(ifstream& in);
void WriteFile(ofstream& out, const vector<string>& content);
bool OpenFile(ifstream& in, const string& filename);
HANDLE CreateWritesSemaphore(int numOfEnters);
HANDLE CreateReadySemaphore(int numOfEnters);
HANDLE CreateDemoMutex();
HANDLE CreateDemoEvent();
bool CreateSenderProcesses(STARTUPINFO* si, PROCESS_INFORMATION* pi, const string& filename, int numOfSenders);
void TerminateSenderProcesses(PROCESS_INFORMATION* pi, int numOfSenders);


namespace UnitTest
{
    TEST_CLASS(UnitTestReadFile)
    {
    public:

        TEST_METHOD(TestMethodEmptyFile)
        {
            ofstream out("test.txt");
            out.close();
            ifstream in("test.txt");
            vector<string> test = ReadFile(in);
            in.close();
            Assert::IsTrue(0 == test.size());
        }

        TEST_METHOD(TestMethodFileWithInformation)
        {
            ofstream out("test.txt", fstream::binary);
            vector<string> needed = { "good", "bad", "lock" };
            for (int i = 0; i < needed.size(); ++i)
            {
                char mess[21];
                strcpy(mess, needed[i].c_str());
                out.write(mess, 21);
            }
            out.close();
            ifstream in("test.txt");
            vector<string> test = ReadFile(in);
            in.close();
            for (int i = 0; i < needed.size(); ++i)
            {
                Assert::AreEqual(needed[i], test[i]);
            }
        }
    };

    TEST_CLASS(UnitTestWriteFile)
    {
    public:

        TEST_METHOD(TestMethodWriteInformationFirst)
        {
            ofstream out("test.txt", ofstream::binary | ofstream::out | ofstream::trunc);
            vector<string> needed_first_half = { "skip", "bad", "lock", "hook", "groom", "dog" };
            WriteFile(out, needed_first_half);
            out.close();
            ifstream in("test.txt");
            vector<string> test = ReadFile(in); // Use cause have already test
            in.close();
            Assert::IsTrue(needed_first_half.size() == test.size() + 1);
            for (int i = 0; i < test.size(); ++i)
            {
                Assert::AreEqual(needed_first_half[i + 1], test[i]);
            }
        }

        TEST_METHOD(TestMethodWriteInformationSecond)
        {
            ofstream out("test.txt", ofstream::binary | ofstream::out | ofstream::trunc);
            vector<string> needed_second_half = { "skip", "groom", "dog" };
            WriteFile(out, needed_second_half);
            out.close();
            ifstream in("test.txt");
            vector<string> test = ReadFile(in); // Use cause have already test
            in.close();
            Assert::IsTrue(needed_second_half.size() == test.size() + 1);
            for (int i = 0; i < test.size(); ++i)
            {
                Assert::AreEqual(needed_second_half[i + 1], test[i]);
            }
        }

    };

    TEST_CLASS(UnitTestOpenFile)
    {
    public:

        TEST_METHOD(TestMethod)
        {
            ifstream in_test;
            Assert::IsTrue(OpenFile(in_test, "test.txt"));
        }
    };

    TEST_CLASS(UnitTestCreateWritesSemaphore)
    {
    public:

        TEST_METHOD(TestMethodCreateWritesSemaphore)
        {
            HANDLE hSemaphore = CreateWritesSemaphore(5);
            Assert::IsTrue(NULL != hSemaphore);
            CloseHandle(hSemaphore);
        }

        TEST_METHOD(TestMethodWritesSemaphoreExist)
        {
            HANDLE hSemaphoreTest = CreateSemaphore(NULL, 5, 5, (LPCWSTR)"SemaphoreWrites");
            HANDLE hSemaphore = CreateWritesSemaphore(5);
            Assert::IsTrue(ERROR_ALREADY_EXISTS == GetLastError());
            CloseHandle(hSemaphore);
            CloseHandle(hSemaphoreTest);
        }
    };

    TEST_CLASS(UnitTestCreateReadySemaphore)
    {
    public:

        TEST_METHOD(TestMethodCreateReadySemaphore)
        {
            HANDLE hSemaphore = CreateReadySemaphore(5);
            Assert::IsTrue(NULL != hSemaphore);
            CloseHandle(hSemaphore);
        }

        TEST_METHOD(TestMethodReadySemaphoreExist)
        {
            HANDLE hSemaphoreTest = CreateSemaphore(NULL, 0, 5, (LPCWSTR)"SemaphoreReady");
            HANDLE hSemaphore = CreateReadySemaphore(1);
            Assert::IsTrue(ERROR_ALREADY_EXISTS == GetLastError());
            CloseHandle(hSemaphore);
            CloseHandle(hSemaphoreTest);
        }
    };

    TEST_CLASS(UnitTestCreateDemoMutex)
    {
    public:

        TEST_METHOD(TestMethodCreateMutex)
        {
            HANDLE hMutex = CreateDemoMutex();
            Assert::IsTrue(NULL != hMutex);
            CloseHandle(hMutex);
        }

        TEST_METHOD(TestMethodMutexExist)
        {
            HANDLE hMutexTest = CreateMutex(NULL, FALSE, (LPCWSTR)"DemoMutex");
            HANDLE hMutex = CreateDemoMutex();
            Assert::IsTrue(ERROR_ALREADY_EXISTS == GetLastError());
            CloseHandle(hMutex);
            CloseHandle(hMutexTest);
        }
    };

    TEST_CLASS(UnitTestCreateDemoEvent)
    {
    public:

        TEST_METHOD(TestMethodCreateEvent)
        {
            HANDLE hEvent = CreateDemoEvent();
            Assert::IsTrue(NULL != hEvent);
            CloseHandle(hEvent);
        }

        TEST_METHOD(TestMethodEventExist)
        {
            HANDLE hEventTest = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"Event");
            HANDLE hEvent = CreateDemoEvent();
            Assert::IsTrue(ERROR_ALREADY_EXISTS == GetLastError());
            CloseHandle(hEvent);
            CloseHandle(hEventTest);
        }
    };

    TEST_CLASS(UnitTestCreateProcesses)
    {
    public:

        TEST_METHOD(TestMethodCreateProcesses)
        {
            int numOfSenders = 5;
            STARTUPINFO* si = new STARTUPINFO[numOfSenders];
            PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[numOfSenders];
            Assert::IsTrue(CreateSenderProcesses(si, pi, "test.txt", numOfSenders));
            TerminateSenderProcesses(pi, numOfSenders);
        }
    };
}