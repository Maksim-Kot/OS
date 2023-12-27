#include "pch.h"
#include "CppUnitTest.h"
#include <windows.h>
#include <vector>
#include <algorithm>
#include <numeric>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct Array
{
	double* a;
	int size;
	double average;
	double min_;
	double max_;
	Array(double* a_, int size_) : a(a_), size(size_), average(0), min_(0), max_(0) {}
};

DWORD WINAPI Min_Max(LPVOID arr);
DWORD WINAPI Average(LPVOID arr);
void Replace(Array* param);


namespace UnitTest
{
	TEST_CLASS(UnitTestMinMax)
	{
	public:
		
		TEST_METHOD(TestMethodMinMaxAuto)
		{
			srand(time(NULL));
			int size = rand() % 10;
			std::vector<double> test(size);
			double* a = new double[size];
			for (int i = 0; i < size; ++i)
			{
				a[i] = test[i] = rand() % 100;
			}
			auto param = new Array(a, size);
			Min_Max((LPVOID)param);
			double min = *std::min_element(test.begin(), test.end());
			double max = *std::max_element(test.begin(), test.end());
			Assert::IsTrue(abs(min - param->min_) < 1e-5);
			Assert::IsTrue(abs(max - param->max_) < 1e-5);
		}

		TEST_METHOD(TestMethodMinMaxManual)
		{
			double* a = new double[5];
			a[0] = 2; a[1] = 4; a[2] = 3; a[3] = 5; a[4] = 8;
			auto param = new Array(a, 5);
			Min_Max((LPVOID)param);
			Assert::IsTrue(abs(2 - param->min_) < 1e-5);
			Assert::IsTrue(abs(8 - param->max_) < 1e-5);
		}
	};

	TEST_CLASS(UnitTestAverage)
	{
	public:

		TEST_METHOD(TestMethodAverageAuto)
		{
			srand(time(NULL));
			int size = rand() % 10;
			std::vector<double> test(size);
			double* a = new double[size];
			for (int i = 0; i < size; ++i)
			{
				a[i] = test[i] = rand() % 100;
			}
			auto param = new Array(a, size);
			Average((LPVOID)param);
			double average = std::accumulate(test.begin(), test.end(), 0);
			average /= test.size();
			Assert::IsTrue(abs(average - param->average) < 1e-5);
		}

		TEST_METHOD(TestMethodAverageManual)
		{
			double* a = new double[5];
			a[0] = 2; a[1] = 4; a[2] = 3; a[3] = 5; a[4] = 8;
			auto param = new Array(a, 5);
			Average((LPVOID)param);
			Assert::IsTrue(abs(4.4 - param->average) < 1e-5);
		}
	};

	TEST_CLASS(UnitTestReplace)
	{
	public:

		TEST_METHOD(TestMethodReplaceAuto)
		{
			srand(time(NULL));
			int size = rand() % 10;
			std::vector<double> test(size);
			double* a = new double[size];
			for (int i = 0; i < size; ++i)
			{
				a[i] = test[i] = rand() % 100;
			}
			auto param = new Array(a, size);
			Min_Max((LPVOID)param);
			Average((LPVOID)param);
			Replace(param);

			double average = std::accumulate(test.begin(), test.end(), 0);
			average /= test.size();
			double min = *std::min_element(test.begin(), test.end());
			double max = *std::max_element(test.begin(), test.end());
			std::replace_if(test.begin(), test.end(), [min, max](double needed) {
				return abs(needed - min) < 1e-5 || abs(needed - max) < 1e-5;
				}, average);

			for (int i = 0; i < size; ++i)
			{
				Assert::IsTrue(abs(a[i] - test[i]) < 1e-5);
			}
		}

		TEST_METHOD(TestMethodReplaceManual1)
		{
			double* a = new double[5];
			a[0] = 2; a[1] = 4; a[2] = 3; a[3] = 5; a[4] = 8;
			auto param = new Array(a, 5);
			Min_Max((LPVOID)param);
			Average((LPVOID)param);
			Replace(param);

			std::vector<double> test = { 4.4, 4, 3, 5, 4.4 };

			for (int i = 0; i < 5; ++i)
			{
				Assert::IsTrue(abs(a[i] - test[i]) < 1e-5);
			}
		}

		TEST_METHOD(TestMethodReplaceManual2)
		{
			double* a = new double[5];
			a[0] = 2; a[1] = 4; a[2] = 3; a[3] = 8; a[4] = 8;
			auto param = new Array(a, 5);
			Min_Max((LPVOID)param);
			Average((LPVOID)param);
			Replace(param);

			std::vector<double> test = { 5, 4, 3, 5, 5 };

			for (int i = 0; i < 5; ++i)
			{
				Assert::IsTrue(abs(a[i] - test[i]) < 1e-5);
			}
		}

		TEST_METHOD(TestMethodReplaceManual3)
		{
			double* a = new double[5];
			a[0] = 2; a[1] = 4; a[2] = 3; a[3] = 2; a[4] = 8;
			auto param = new Array(a, 5);
			Min_Max((LPVOID)param);
			Average((LPVOID)param);
			Replace(param);

			std::vector<double> test = { 3.8, 4, 3, 3.8, 3.8 };

			for (int i = 0; i < 5; ++i)
			{
				Assert::IsTrue(abs(a[i] - test[i]) < 1e-5);
			}
		}
	};
}
