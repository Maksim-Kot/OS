#include <windows.h>
#include <iostream>
using namespace std;

struct Array
{
	int* a;
	int size;
	int average;
	int min_;
	int max_;
	Array (int* a_, int size_): a(a_), size(size_), average(0), min_(0), max_(0){}
};

DWORD WINAPI Min_Max(LPVOID arr)
{
	cout << "Thread Min_Max is started." << endl;
	int min_ = static_cast<Array*>(arr)->a[0];
	int max_ = min_;
	int index_min = 0, index_max = 0;
	for (int i = 1; i < static_cast<Array*>(arr)->size; i++)
	{
		if (static_cast<Array*>(arr)->a[i] > max_) 
		{
			max_ = static_cast<Array*>(arr)->a[i];
			index_max = i;
		}
		if (static_cast<Array*>(arr)->a[i] < min_) 
		{
			min_ = static_cast<Array*>(arr)->a[i];
			index_min = i;
		}
		Sleep(7);
	}
	static_cast<Array*>(arr)->max_ = index_max;
	static_cast<Array*>(arr)->min_ = index_min;

	cout << "Min: " << min_ << '\n' << "Max: " << max_ << '\n';
	cout << "Thread Min_Max is finished." << endl;
	return 0;
}

int main()
{
	HANDLE hThread;
	DWORD IDThread;
	int n;
	cout << "Enter count of elements: ";
	cin >> n;
	int* a = new int[n];
	cout << "Enter elements:\n";
	for (int i = 0; i < n; ++i)
	{
		cin >> a[i];
	}
	auto param = new Array(a, n);
	hThread = CreateThread(NULL, 0, Min_Max, (LPVOID)param, 0, &IDThread);
	if (hThread == NULL)
		return GetLastError();
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	return 0;
}