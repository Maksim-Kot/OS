#include "pch.h"
#include "CppUnitTest.h"
#include <windows.h>
#include <string>
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::ifstream;
using std::ofstream;
using std::string;

HANDLE OpenWritesSemaphore();
HANDLE OpenReadySemaphore();
HANDLE OpenDemoMutex();
HANDLE OpenDemoEvent();
bool OpenFile(ofstream& out, const string& filename);
int numOfEnters = 4;

namespace UnitTestForSender
{
    TEST_CLASS(UnitTestOpenWritesSemaphore)
    {
    public:

        TEST_METHOD(TestMethodOpenWritesSemaphore)
        {
            HANDLE hSemaphoreWrites = CreateSemaphore(NULL, numOfEnters, numOfEnters, (LPCWSTR)"SemaphoreWrites");
            HANDLE hSemaphore = OpenWritesSemaphore();
            Assert::IsTrue(NULL != hSemaphore);
            CloseHandle(hSemaphoreWrites);
            CloseHandle(hSemaphore);
        }

        TEST_METHOD(TestMethodOpenSemaphoreNoExist)
        {
            //HANDLE hSemaphoreWrites = CreateSemaphore(NULL, numOfEnters, numOfEnters, (LPCWSTR)"SemaphoreWrites");
            HANDLE hSemaphore = OpenWritesSemaphore();
            Assert::IsNull(hSemaphore);
            CloseHandle(hSemaphore);
        }
    };

    TEST_CLASS(UnitTestOpenReadySemaphore)
    {
    public:

        TEST_METHOD(TestMethodOpenReadySemaphore)
        {
            HANDLE hSemaphoreReady = CreateSemaphore(NULL, 0, numOfEnters, (LPCWSTR)"SemaphoreReady");
            HANDLE hSemaphore = OpenReadySemaphore();
            Assert::IsTrue(NULL != hSemaphore);
            CloseHandle(hSemaphoreReady);
            CloseHandle(hSemaphore);
        }

        TEST_METHOD(TestMethodOpenReadySemaphoreNoExist)
        {
            HANDLE hSemaphore = OpenReadySemaphore();
            Assert::IsNull(hSemaphore);
            CloseHandle(hSemaphore);
        }
    };

    TEST_CLASS(UnitTestOpenDemoMutex)
    {
    public:

        TEST_METHOD(TestMethodOpenMutex)
        {
            HANDLE hMutexTest = CreateMutex(NULL, FALSE, (LPCWSTR)"DemoMutex");
            HANDLE hMutex = OpenDemoMutex();
            Assert::IsTrue(NULL != hMutex);
            CloseHandle(hMutexTest);
            CloseHandle(hMutex);
        }

        TEST_METHOD(TestMethodOpenMutexNoExist)
        {
            HANDLE hMutex = OpenDemoMutex();
            Assert::IsNull(hMutex);
            CloseHandle(hMutex);
        }
    };

    TEST_CLASS(UnitTestOpenDemoEvent)
    {
    public:

        TEST_METHOD(TestMethodOpenEvent)
        {
            HANDLE hEventTest = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"Event");
            HANDLE hEvent = OpenDemoEvent();
            Assert::IsTrue(NULL != hEvent);
            CloseHandle(hEventTest);
            CloseHandle(hEvent);
        }

        TEST_METHOD(TestMethodOpenEventNoExist)
        {
            HANDLE hEvent = OpenDemoEvent();
            Assert::IsTrue(NULL != hEvent);
            CloseHandle(hEvent);
        }
    };

    TEST_CLASS(UnitTestOpenFile)
    {
    public:

        TEST_METHOD(TestMethod)
        {
            ofstream out;
            Assert::IsTrue(OpenFile(out, "test.txt"));
            out.close();
        }
    };
}
