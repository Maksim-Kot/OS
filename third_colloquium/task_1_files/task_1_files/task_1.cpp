#include <Windows.h>
#include <process.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

HANDLE event;

double FindSolution(char type, std::vector<double> a)
{
	if (type == '1')//sum
	{
		double res = 0;
		for (int i = 0; i < a.size(); ++i)
		{
			res += a[i];
		}
		return res;
	}

	if (type == '2')//multiplication
	{
		double res = 1;
		for (int i = 0; i < a.size(); ++i)
		{
			res *= a[i];
		}
		return res;
	}

	if (type == '3')//square sum
	{
		double res = 0;
		for (int i = 0; i < a.size(); ++i)
		{
			res += a[i] * a[i];
		}
		return res;
	}

	throw("Error");
}

std::ifstream CreateIfstream(std::string directory, int index)
{
	return std::ifstream(directory + "in_" + std::to_string(index) + ".dat");
}

struct Filename
{
	std::string s;
	int i;
	double result = 0;
};

DWORD WINAPI ReadFile(LPVOID param)
{
	std::ifstream fin = CreateIfstream(static_cast<Filename*>(param)->s, static_cast<Filename*>(param)->i);
	SetEvent(event);		//release event after creation of input stream, to open new threads
	if (!fin.is_open())
	{
		return -1;
	}
	char type;
	fin >> type;
	std::vector <double> v;
	double x;
	while (fin >> x)
	{
		v.push_back(x);
	}
	static_cast<Filename*>(param)->result += FindSolution(type, v);
	return 0;
}

int main(int argc, char* argv[])
{
	event = CreateEvent(NULL, FALSE, FALSE, NULL);
	int count;
	std::cout << "Enter count of threads: ";
	count = atoi(argv[1]);
	//std::cin >> count;
	HANDLE* hThread = new HANDLE[count];
	DWORD* IDThread = new DWORD[count];
	Filename filename;
	std::cout << "Enter directory: ";
	filename.s = argv[2];
	//std::cin >> filename.s;
	filename.i = 1;

	auto param = &filename;

	for (int i = 1; i <= count; ++i)
	{
		hThread[i - 1] = CreateThread(nullptr, 0, ReadFile, (LPVOID)&filename, 0, &IDThread[i - 1]);
		WaitForSingleObject(event, INFINITE);	//waiting for creation of input stream to change index of file
		++filename.i;
	}

	int exit_code;
	std::ofstream fout(filename.s + "out.dat");

	while (true)
	{
		int index = WaitForMultipleObjects(count, hThread, FALSE, INFINITE) - WAIT_OBJECT_0;	//compute the index of
		GetExitCodeThread(hThread[index], (LPDWORD)&exit_code);									//the completed thread 
		if (exit_code == -1)																	//and its exit code 
		{
			break;
		}
		CloseHandle(hThread[index]);
		hThread[index] = CreateThread(nullptr, 0, ReadFile, (LPVOID)&filename, 0, &IDThread[index]);
		WaitForSingleObject(event, INFINITE);	//waiting for creation of input stream to change index of file
		++filename.i;
	}

	WaitForMultipleObjects(count, hThread, TRUE, INFINITE);	//waiting for all processes to finish

	fout << filename.result << "\n";

	for (int i = 0; i < count; ++i)
	{
		CloseHandle(hThread[i]);
	}
	delete[] hThread;
	delete[] IDThread;
	return 0;
}