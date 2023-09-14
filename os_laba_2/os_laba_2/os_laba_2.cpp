#include <windows.h>
#include <iostream>
#include <iomanip>

struct Array
{
	double* a;
	int size;
	double average;
	int min_;
	int max_;
	Array (double* a_, int size_): a(a_), size(size_), average(0), min_(0), max_(0){}
};

DWORD WINAPI Min_Max(LPVOID arr)
{
	std::cout << "Thread Min_Max is started.\n";
	double min_ = static_cast<Array*>(arr)->a[0];
	double max_ = min_;
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

	std::cout << "Min: " << min_ << '\n' << "Max: " << max_ << '\n' 
		<< "Thread Min_Max is finished.\n";
	return 0;
}

DWORD WINAPI Average(LPVOID arr)
{
	std::cout << "Thread Average is started.\n";
	double sum = 0;
	for (int i = 0; i < static_cast<Array*>(arr)->size; i++)
	{
		sum += static_cast<Array*>(arr)->a[i];
		Sleep(12);
	}
	static_cast<Array*>(arr)->average = sum / static_cast<Array*>(arr)->size;
	

	std::cout << "Average: " << static_cast<Array*>(arr)->average << '\n' 
		<< "Thread Average is finished.\n";
	return 0;
}

int main()
{
	HANDLE hThread;
	DWORD IDThread;
	int n;
	std::cout << "Enter count of elements: ";
	std::cin >> n;
	double* a = new double[n];
	std::cout << "Enter elements:\n";
	for (int i = 0; i < n; ++i)
	{
		std::cin >> a[i];
	}
	auto param = new Array(a, n);
	hThread = CreateThread(NULL, 0, Min_Max, (LPVOID)param, 0, &IDThread);
	if (hThread == NULL)
		return GetLastError();
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);

	HANDLE hAverage;
	DWORD IDAverage;

	hAverage = CreateThread(NULL, 0, Average, (LPVOID)param, 0, &IDAverage);
	if (hAverage == NULL)
		return GetLastError();
	WaitForSingleObject(hAverage, INFINITE);
	CloseHandle(hAverage);

	double max_ = param->a[param->max_], min_ = param->a[param->min_];
	std::cout << "New array: ";
	for (int i = 0; i < param->size; i++)
	{
		if (param->a[i] == max_ || param->a[i] == min_)
			param->a[i] = param->average;
		std::cout << std::setprecision(3) << param->a[i] << " ";
	}
	std::cout << '\n';
	return 0;
}