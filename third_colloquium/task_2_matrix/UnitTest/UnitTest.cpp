#include "pch.h"
#include "CppUnitTest.h"
#include <vector>
#include <fstream>
#include <windows.h>
using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct MMatrix
{
	vector<vector<double>> mat1;
	vector<vector<double>> mat2;
	vector<vector<double>> res;
	int i, j;
	int rows1, cols1, rows2, cols2;
	int amount;
	string nameOfFile;

	MMatrix()
	{
		rows1 = 0;
		cols1 = 0;
		rows2 = 0;
		cols2 = 0;
	}
	MMatrix(vector<vector<double>> a, vector<vector<double>> b, int count, string s)
	{
		mat1 = a; mat2 = b;
		i = j = 0;
		rows1 = mat1.size();
		cols1 = mat1[0].size();
		rows2 = mat2.size();
		cols2 = mat2[0].size();
		res.resize(rows1);
		amount = count;
		nameOfFile = s;
		for (int i = 0; i < rows1; ++i)
		{
			res[i].resize(cols2, 0);
		}
	}
};
void GoodToMultiply(MMatrix& a);
vector<vector<double>> GetMatrix(ifstream& fin);
void PrintMatrixOnConsole(const vector<vector<double>>& mat);
DWORD WINAPI PrintMatrix(LPVOID mmatrix);
DWORD WINAPI FindElement(LPVOID mmatrix);
int main(int argc, char* argv[]);

namespace Tests
{
	TEST_CLASS(UnitTestGoodToMultiply)
	{
	public:

		TEST_METHOD(Test1)
		{
			try
			{
				GoodToMultiply(MMatrix(
					vector<vector<double>>(3, vector<double>(2)), vector<vector<double>>(2, vector<double>(3)), 3, ""
				));
			}
			catch (string s)
			{
				Assert::Fail(L"It was expected that matrices could be multiplied");
			}
		}
		TEST_METHOD(Test2)
		{
			try
			{
				GoodToMultiply(MMatrix(
					vector<vector<double>>(3, vector<double>(2)), vector<vector<double>>(5, vector<double>(3)), 3, ""
				));
				Assert::Fail(L"It was not expected that matrices could be multiplied");
			}
			catch (const char* s)
			{
			}
		}
		TEST_METHOD(Test3)
		{
			try
			{
				GoodToMultiply(MMatrix(
					vector<vector<double>>(5, vector<double>(100)), vector<vector<double>>(4, vector<double>(12)), 3, ""
				));
				Assert::Fail(L"It was not expected that matrices could be multiplied");
			}
			catch (const char* s)
			{
			}
		}
	};

	TEST_CLASS(UnitTestGetMatrix)
	{
	public:

		TEST_METHOD(Test1)
		{
			vector<vector<double>> matr1 =
			{ {1.1, 2.2},
			  {3.3, 4.4},
			  {5.5, 6.6} };
			ofstream fout("input.txt");
			fout << 3 << " " << 2 << "\n";
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					fout << matr1[i][j] << ' ';
				}
				fout << '\n';
			}
			fout.close();
			ifstream fin("input.txt");
			vector<vector<double>> matr2 = GetMatrix(fin);
			int row, col;
			fin >> row >> col;
			for (int i = 0; i < row; ++i)
			{
				for (int j = 0; j < col; ++j)
				{
					fin >> matr2[i][j];
				}
			}

			for (int i = 0; i < matr2.size(); ++i)
			{
				for (int j = 0; j < matr2[i].size(); ++j)
				{
					Assert::IsTrue(matr2[i][j] - matr1[i][j] < 1e-5);
				}
			}
		}
		TEST_METHOD(Test2)
		{
			vector<vector<double>> matr1 =
			{ {5.1, 8.4, 6.4},
			  {3.6, 7.4, 4.3},
			  {5.3, 9.8, 1.2} };
			ofstream fout("input.txt");
			fout << 3 << " " << 3 << "\n";
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					fout << matr1[i][j] << ' ';
				}
				fout << '\n';
			}
			fout.close();
			ifstream fin("input.txt");
			vector<vector<double>> matr2 = GetMatrix(fin);
			int row, col;
			fin >> row >> col;
			for (int i = 0; i < row; ++i)
			{
				for (int j = 0; j < col; ++j)
				{
					fin >> matr2[i][j];
				}
			}

			for (int i = 0; i < matr2.size(); ++i)
			{
				for (int j = 0; j < matr2[i].size(); ++j)
				{
					Assert::IsTrue(matr2[i][j] - matr1[i][j] < 1e-5);
				}
			}
		}
		TEST_METHOD(Test3)
		{
			vector<vector<double>> matr1 =
			{ {1., 0., 0., 0.},
			  {0., 1., 0., 0.},
			  {0., 0., 1., 0.},
			  {0., 0., 0., 1.} };
			ofstream fout("input.txt");
			fout << 4 << " " << 4 << "\n";
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					fout << matr1[i][j] << ' ';
				}
				fout << '\n';
			}
			fout.close();
			ifstream fin("input.txt");
			vector<vector<double>> matr2 = GetMatrix(fin);
			int row, col;
			fin >> row >> col;
			for (int i = 0; i < row; ++i)
			{
				for (int j = 0; j < col; ++j)
				{
					fin >> matr2[i][j];
				}
			}

			for (int i = 0; i < matr2.size(); ++i)
			{
				for (int j = 0; j < matr2[i].size(); ++j)
				{
					Assert::IsTrue(matr2[i][j] - matr1[i][j] < 1e-5);
				}
			}
		}
		TEST_METHOD(Test4)
		{
			try
			{
				ofstream fout("input.txt");
				fout << 0 << " " << 0 << "\n";
				fout.close();
				ifstream fin("input.txt");
				vector<vector<double>> matr2 = GetMatrix(fin);
				Assert::Fail(L"It was not expected that the matrix could be read");
			}
			catch (...)
			{
			}
		}
		TEST_METHOD(Test5)
		{
			try
			{
				ofstream fout("input.txt");
				fout << -4 << " " << 6 << "\n";
				fout.close();
				ifstream fin("input.txt");
				vector<vector<double>> matr2 = GetMatrix(fin);
				Assert::Fail(L"It was not expected that the matrix could be read");
			}
			catch (...)
			{
			}
		}
	};

	TEST_CLASS(UnitTestPrintMatrixOnConsole)
	{
		TEST_METHOD(Test1)
		{
			try
			{
				vector<vector<double>> matr = { {3.5, 4.6, 5.7, 6.8},
														{8.0, 9.1, 1.4, 2.5},
														{4.7, 5.8, 6.9, 7.0},
														{9.2, 1.5, 2.6, 3.7} };
				PrintMatrixOnConsole(matr);
			}
			catch (...)
			{
				Assert::Fail(L"It was expected that the matrix could be print");
			}
		}
		TEST_METHOD(Test2)
		{
			try
			{
				vector<vector<double>> matr;
				PrintMatrixOnConsole(matr);
				Assert::Fail(L"It was not expected that the matrix could be print");
			}
			catch (...)
			{
			}
		}
	};

	TEST_CLASS(UnitTestPrintMatrix)
	{
		TEST_METHOD(Test1)
		{
			try
			{
				MMatrix matr;
				matr.res = { {3.5, 4.6, 5.7, 6.8},
							 {8.0, 9.1, 1.4, 2.5},
							 {4.7, 5.8, 6.9, 7.0},
							 {9.2, 1.5, 2.6, 3.7} };
				matr.nameOfFile = "output.txt";
				PrintMatrix((LPVOID)&matr);
			}
			catch (...)
			{
				Assert::Fail(L"It was expected that the matrix could be print");
			}
		}
		TEST_METHOD(Test2)
		{
			try
			{
				MMatrix matr;
				matr.res = { {1.7, 3.6},
							 {5., 2.1} };
				PrintMatrix((LPVOID)&matr);
				Assert::Fail(L"It was not expected that the matrix could be print");
			}
			catch (...)
			{
			}
		}
		TEST_METHOD(Test3)
		{
			try
			{
				MMatrix matr;
				matr.nameOfFile = "output.txt";
				PrintMatrix((LPVOID)&matr);
				Assert::Fail(L"It was not expected that the matrix could be print");
			}
			catch (...)
			{
			}
		}
	};

	TEST_CLASS(UnitTestCommon)
	{
	public:
		TEST_METHOD(Test1)
		{
			vector<vector<double>> matr1 = { {1.1, 2.2},
											 {3.3, 4.4} };
			vector<vector<double>> matr2 = { {5.5, 6.6},
											 {7.7, 8.8} };
			vector<vector<double>> resTest = { {22.99, 26.62},
											   {52.03, 60.5} };

			int row1 = matr1.size(), row2 = matr2.size(),
				col1 = matr1[0].size(), col2 = matr2[0].size();

			ofstream fout("input.txt");

			fout << row1 << " " << col1 << "\n";
			for (int i = 0; i < row1; ++i)
			{
				for (int j = 0; j < col1; ++j)
				{
					fout << matr1[i][j] << ' ';
				}
				fout << '\n';
			}
			fout << "\n";
			fout << row2 << " " << col2 << "\n";
			for (int i = 0; i < row2; ++i)
			{
				for (int j = 0; j < col2; ++j)
				{
					fout << matr2[i][j] << ' ';
				}
				fout << '\n';
			}

			fout.close();

			char* argv[] = { "Task_2.exe", "input.txt", "3" };
			main(3, argv);

			ifstream fin("input.txt");
			GetMatrix(fin);
			GetMatrix(fin);
			vector<vector<double>> res(matr1.size(), vector<double>(matr2[0].size()));

			for (int i = 0; i < matr1.size(); ++i)
			{
				for (int j = 0; j < matr2[0].size(); ++j)
				{
					fin >> res[i][j];
				}
			}

			for (int i = 0; i < matr1.size(); ++i)
			{
				for (int j = 0; j < matr2[0].size(); ++j)
				{
					Assert::IsTrue(resTest[i][j] - res[i][j] < 1e-6);
				}
			}
		}
		TEST_METHOD(Test2)
		{
			vector<vector<double>> matr1 = { {1.1, 2.2},
													   {3.3, 4.4},
													   {5.5, 6.6} };
			vector<vector<double>> matr2 = { {7.7, 8.8, 9.9},
													   {10.10, 11.11, 12.12} };
			vector<vector<double>> resTest = { {30.69, 34.122, 37.554},
														 {69.85, 77.924, 85.998},
														 {109.01, 121.726, 134.442} };

			int row1 = matr1.size(), row2 = matr2.size(),
				col1 = matr1[0].size(), col2 = matr2[0].size();

			ofstream fout("input.txt");

			fout << row1 << " " << col1 << "\n";
			for (int i = 0; i < row1; ++i)
			{
				for (int j = 0; j < col1; ++j)
				{
					fout << matr1[i][j] << ' ';
				}
				fout << '\n';
			}
			fout << "\n";
			fout << row2 << " " << col2 << "\n";
			for (int i = 0; i < row2; ++i)
			{
				for (int j = 0; j < col2; ++j)
				{
					fout << matr2[i][j] << ' ';
				}
				fout << '\n';
			}

			fout.close();

			char* argv[] = { "Task_2.exe", "input.txt", "5" };
			main(3, argv);

			ifstream fin("input.txt");
			GetMatrix(fin);
			GetMatrix(fin);
			vector<vector<double>> res(matr1.size(), vector<double>(matr2[0].size()));

			for (int i = 0; i < matr1.size(); ++i)
			{
				for (int j = 0; j < matr2[0].size(); ++j)
				{
					fin >> res[i][j];
				}
			}

			for (int i = 0; i < matr1.size(); ++i)
			{
				for (int j = 0; j < matr2[0].size(); ++j)
				{
					Assert::IsTrue(resTest[i][j] - res[i][j] < 1e-6);
				}
			}
		}
		TEST_METHOD(Test3)
		{
			vector<vector<double>> matr1 = { {7.7, 8.8, 9.9},
													   {10.10, 11.11, 12.12} };
			vector<vector<double>> matr2 = { {1.1, 2.2},
													   {3.3, 4.4},
													   {5.5, 6.6} };

			vector<vector<double>> resTest = { {91.96, 121},
														 {114.433, 151.096} };

			int row1 = matr1.size(), row2 = matr2.size(),
				col1 = matr1[0].size(), col2 = matr2[0].size();

			ofstream fout("input.txt");

			fout << row1 << " " << col1 << "\n";
			for (int i = 0; i < row1; ++i)
			{
				for (int j = 0; j < col1; ++j)
				{
					fout << matr1[i][j] << ' ';
				}
				fout << '\n';
			}
			fout << "\n";
			fout << row2 << " " << col2 << "\n";
			for (int i = 0; i < row2; ++i)
			{
				for (int j = 0; j < col2; ++j)
				{
					fout << matr2[i][j] << ' ';
				}
				fout << '\n';
			}

			fout.close();

			char* argv[] = { "Task_2.exe", "input.txt", "4" };
			main(3, argv);

			ifstream fin("input.txt");
			GetMatrix(fin);
			GetMatrix(fin);
			vector<vector<double>> res(matr1.size(), vector<double>(matr2[0].size()));

			for (int i = 0; i < matr1.size(); ++i)
			{
				for (int j = 0; j < matr2[0].size(); ++j)
				{
					fin >> res[i][j];
				}
			}

			for (int i = 0; i < matr1.size(); ++i)
			{
				for (int j = 0; j < matr2[0].size(); ++j)
				{
					Assert::IsTrue(resTest[i][j] - res[i][j] < 1e-6);
				}
			}
		}
		TEST_METHOD(Test4)
		{
			vector<vector<double>> matr1 = { {1.1, 2.2, 3.3, 4.4},
														{5.5, 6.6, 7.7, 8.8},
														{9.9, 1.2, 2.3, 3.4},
														{4.5, 5.6, 6.7, 7.8},
														{8.9, 9.1, 1.3, 2.4} };

			vector<vector<double>> matr2 = { {3.5, 4.6, 5.7, 6.8},
														{8.0, 9.1, 1.4, 2.5},
														{4.7, 5.8, 6.9, 7.0},
														{9.2, 1.5, 2.6, 3.7} };

			vector<vector<double>> resTest = { {77.44, 50.82, 43.56, 52.36},
													 {189.2, 143.22, 116.6, 140.36},
													 {86.34, 74.9, 82.82, 99},
													 {163.8, 122.22, 100, 120.36},
													 {132.14, 134.89, 78.68, 101.25} };

			int row1 = matr1.size(), row2 = matr2.size(),
				col1 = matr1[0].size(), col2 = matr2[0].size();

			ofstream fout("input.txt");

			fout << row1 << " " << col1 << "\n";
			for (int i = 0; i < row1; ++i)
			{
				for (int j = 0; j < col1; ++j)
				{
					fout << matr1[i][j] << ' ';
				}
				fout << '\n';
			}
			fout << "\n";
			fout << row2 << " " << col2 << "\n";
			for (int i = 0; i < row2; ++i)
			{
				for (int j = 0; j < col2; ++j)
				{
					fout << matr2[i][j] << ' ';
				}
				fout << '\n';
			}

			fout.close();

			char* argv[] = { "Task_2.exe", "input.txt", "10" };
			main(3, argv);

			ifstream fin("input.txt");
			GetMatrix(fin);
			GetMatrix(fin);
			vector<vector<double>> res(matr1.size(), vector<double>(matr2[0].size()));

			for (int i = 0; i < matr1.size(); ++i)
			{
				for (int j = 0; j < matr2[0].size(); ++j)
				{
					fin >> res[i][j];
				}
			}

			for (int i = 0; i < matr1.size(); ++i)
			{
				for (int j = 0; j < matr2[0].size(); ++j)
				{
					Assert::IsTrue(resTest[i][j] - res[i][j] < 1e-6);
				}
			}
		}
		TEST_METHOD(Test5)
		{
			try
			{
				vector<vector<double>> matr1 = { {1.1, 2.2, 3.3, 4.4},
															{9.9, 1.2, 2.3, 3.4},
															{4.5, 5.6, 6.7, 7.8},
															{8.9, 9.1, 1.3, 2.4} };
				vector<vector<double>> matr2 = { };
				vector<vector<double>> resTest = { };

				int row1 = matr1[0].size(), row2 = 0,
					col1 = matr1.size(), col2 = 0;

				ofstream fout("input.txt");

				fout << row1 << " " << col1 << "\n";
				for (int i = 0; i < row1; ++i)
				{
					for (int j = 0; j < col1; ++j)
					{
						fout << matr1[i][j] << ' ';
					}
					fout << '\n';
				}
				fout << "\n";
				fout << row2 << " " << col2 << "\n";

				fout.close();

				char* argv[] = { "Task_2.exe", "input.txt", "9" };
				main(3, argv);
				Assert::Fail(L"It was not expected that matrices could be multiplied");
			}
			catch (...)
			{
			}
		}
		TEST_METHOD(Test6)
		{
			try
			{
				vector<vector<double>> matr1 = { {1.1, 2.2},
														   {3.3, 4.4} };
				vector<vector<double>> matr2 = { {3.5, 4.6, 5.7, 6.8},
															{8.0, 9.1, 1.4, 2.5},
															{4.7, 5.8, 6.9, 7.0},
															{9.2, 1.5, 2.6, 3.7} };
				vector<vector<double>> resTest = { }; //Ex

				int row1 = matr1[0].size(), row2 = 0,
					col1 = matr1.size(), col2 = 0;

				ofstream fout("input.txt");

				fout << row1 << " " << col1 << "\n";
				for (int i = 0; i < row1; ++i)
				{
					for (int j = 0; j < col1; ++j)
					{
						fout << matr1[i][j] << ' ';
					}
					fout << '\n';
				}
				fout << "\n";
				fout << row2 << " " << col2 << "\n";

				fout.close();

				char* argv[] = { "Task_2.exe", "input.txt", "11" };
				main(3, argv);
				Assert::Fail(L"It was not expected that matrices could be multiplied");
			}
			catch (...)
			{
			}
		}
	};
}
