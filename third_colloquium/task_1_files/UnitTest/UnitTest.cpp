#include "pch.h"
#include "CppUnitTest.h"
#include "vector"
#include "fstream"
#include <Windows.h>
#include <ctime>
#include <string.h>
#pragma warning(disable: 4996)

double FindSolution(char type, std::vector<double> a);
std::ifstream CreateIfstream(std::string directory, int index);
struct Filename
{
	std::string s;
	int i;
	double result = 0;
};
DWORD WINAPI ReadFile(LPVOID param);
int main(int argc, char* argv[]);

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(FindSolutionTest)	//This block tests arithmetic function
	{
	public:

		TEST_METHOD(Test1)
		{
			char c = '1';
			std::vector <double> v = { 0, 3.4, 1, 2.2, 4, 1.2, 2.8, 2.8, 3.6, 0, 1.8, 3.4, 0.4, 2.2, 0, 2.2, 2.6, 0.4 };
			Assert::IsTrue(abs(FindSolution(c, v) - 34.) < 1e-6);;
		}
		TEST_METHOD(Test2)
		{
			char c = '2';
			std::vector <double> v = { 0, 3.4, 1, 2.2, 4, 1.2, 2.8, 2.8, 3.6, 0, 1.8, 3.4, 0.4, 2.2, 0, 2.2, 2.6, 0.4 };
			Assert::IsTrue(abs(FindSolution(c, v) - 0) < 1e-6);
		}
		TEST_METHOD(Test3)
		{
			char c = '3';
			std::vector <double> v = { 0, 3.4, 1, 2.2, 4, 1.2, 2.8, 2.8, 3.6, 0, 1.8, 3.4, 0.4, 2.2, 0, 2.2, 2.6, 0.4 };
			Assert::IsTrue(abs(FindSolution(c, v) - 95.04) < 1e-6);
		}
	};
	TEST_CLASS(CreateIfstreamTest)	//This block tests creation of input stream
	{
	public:

		TEST_METHOD(Test1)
		{
			std::string directory = "..\\";
			std::ofstream(directory + "in_1.dat");
			std::ofstream(directory + "in_5.dat");
			Assert::IsTrue(CreateIfstream(directory, 1).is_open());
			Assert::IsTrue(CreateIfstream(directory, 5).is_open());
			Assert::IsFalse(CreateIfstream(directory, 6).is_open());
		}
	};
	TEST_CLASS(ReadFileTest)	//This block tests file processing function
	{
	public:

		TEST_METHOD(Test1)
		{
			Filename filename;
			filename.i = 2;
			filename.s = "..\\test\\";
			ReadFile((LPVOID)&filename);
			Assert::IsTrue(abs(filename.result - 72.8) < 1e-6);
		}
		TEST_METHOD(Test2)
		{
			Filename filename;
			filename.i = 1;
			filename.s = "..\\test\\";
			ReadFile((LPVOID)&filename);
			Assert::IsTrue(abs(filename.result - 0) < 1e-6);
		}
		TEST_METHOD(Test3)
		{
			Filename filename;
			filename.i = 4;
			filename.s = "..\\test\\";
			ReadFile((LPVOID)&filename);
			Assert::IsTrue(abs(filename.result - 14.2) < 1e-6);
		}
	};
	TEST_CLASS(CommonTest)	//This test generates general tests to test the entire program
	{
		TEST_METHOD(TestGenerator)
		{
			srand(time(NULL));
			int N = rand() % 9 + 1;
			double result_1 = 0, result_2;
			for (int i = 0; i < N; ++i)
			{
				std::string filename = "..\\test_common\\in_" + std::to_string(i + 1) + ".dat";
				std::ofstream fout(filename);
				int num = rand() % 3 + 1;
				fout << num << "\n";
				if (num == 1)
				{
					result_2 = 0;
					int K = rand() % 19 + 2;
					for (int i = 0; i < K; i++)
					{
						double n = rand() % 21;
						n /= 5;
						result_2 += n;
						fout << n << " ";
					}
				}
				if (num == 2)
				{
					result_2 = 1;
					int K = rand() % 19 + 2;
					for (int i = 0; i < K; i++)
					{
						double n = rand() % 21;
						n /= 5;
						result_2 *= n;
						fout << n << " ";
					}
				}
				if (num == 3)
				{
					result_2 = 0;
					int K = rand() % 19 + 2;
					for (int i = 0; i < K; i++)
					{
						double n = rand() % 21;
						n /= 5;
						result_2 += n * n;
						fout << n << " ";
					}
				}
				result_1 += result_2;
				fout.close();
			}
			char** argv = new char* [3];
			(argv[1]) = new char;
			*(argv[1]) = rand() % 6 + 1 + '0';
			argv[2] = new char[20];
			strcpy(argv[2], "..\\test_common\\");
			main(3, argv);
			std::ifstream fin("..\\test_common\\out.dat");
			int count = 1;
			long long copy = result_1;
			while (copy /= 10)
			{
				++count;
			}
			if (fin.is_open())
			{
				std::string line;
				getline(fin, line);
				double number = stod(line);
				Assert::IsTrue(abs(number - result_1) < pow(10, (count - 6)));
				fin.close();
			}
			else
			{
				throw NULL;
			}
			for (int i = 0; i < N; ++i)
			{
				std::string filename = "..\\test_common\\in_" + std::to_string(i + 1) + ".dat";
				remove(filename.c_str());
			}
		}
	};
}
