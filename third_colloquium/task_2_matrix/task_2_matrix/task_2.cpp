#include <iostream>
#include <vector>
#include <windows.h>
#include <iomanip>
using namespace std;
CRITICAL_SECTION criticalSection;
HANDLE* hEvent;
HANDLE hEventT;
int temp;


struct MMatrix
{
    vector<vector<double>> mat1;
    vector<vector<double>> mat2;
    vector<vector<double>> res;
    int i, j;
    int rows1, cols1, rows2, cols2;

    MMatrix(vector<vector<double>> a, vector<vector<double>> b)
    {
        mat1 = a; mat2 = b;
        i = j = 0;
        rows1 = mat1.size();
        cols1 = mat1[0].size();
        rows2 = mat2.size();
        cols2 = mat2[0].size();
        res.resize(rows1);
        for(int i = 0; i < rows1; ++i)
        {
            res[i].resize(cols2, 0);
        }
    }
};


DWORD WINAPI FindElement(LPVOID mmatrix)
{
    if (temp < 2)
    {
        SetEvent(hEvent[temp]);
        ++temp;
    }
    WaitForSingleObject(hEventT, INFINITE);
    bool b = true;
    while (b)
    {
        std::cout << "Thread FindElment is started.\n";
        EnterCriticalSection(&criticalSection);
        int i = ((MMatrix*)mmatrix)->i;
        int j = ((MMatrix*)mmatrix)->j;
        if (i < ((MMatrix*)mmatrix)->rows1)
        {
            ((MMatrix*)mmatrix)->j = (j + 1) % ((MMatrix*)mmatrix)->cols2;
            if (0 == ((MMatrix*)mmatrix)->j) ++((MMatrix*)mmatrix)->i;
            LeaveCriticalSection(&criticalSection);

            for (int k = 0; k < ((MMatrix*)mmatrix)->cols1; ++k)
            {
                ((MMatrix*)mmatrix)->res[i][j] += ((MMatrix*)mmatrix)->mat1[i][k] * ((MMatrix*)mmatrix)->mat2[k][j];
            }
        }
        else
        {
            LeaveCriticalSection(&criticalSection);
            std::cout << "Thread FindElement is finished.\n";
            b = false;
        }
        Sleep(250);
    }
    return 0;
}


// Функция для вывода матрицы
void printMatrix(const vector<vector<double>>& mat)
{
    for (const auto& row : mat) 
    {
        for (double val : row) 
        {
            cout << val << " ";
        }
        cout << "\n";
    }
}

int main() 
{
    InitializeCriticalSection(&criticalSection);
    cout << "Enter amount of thread:\n";
    int amount;
    cin >> amount;
    hEvent = new HANDLE[amount];
    for (int i = 0; i < amount; ++i)
    {
        hEvent[i] = CreateEvent(NULL, TRUE, 0, NULL);
    }
    hEventT = CreateEvent(NULL, TRUE, 0, NULL);
    temp = 0;
    setlocale(LC_ALL, "rus");
    vector<vector<double>> matrix1 = { {1, 2, 3},
                                   {4, 5, 6},
                                   {7, 8, 9} };
    vector<vector<double>> matrix2 = { {9, 8, 7},
                                   {6, 5, 4},
                                   {3, 2, 1} };


    MMatrix matrix(matrix1, matrix2);

    if (matrix.cols1 != matrix.rows2)
    {
        cout << "Невозможно умножить матрицы: количество столбцов первой матрицы не равно количеству строк второй матрицы.\n";
        return 0;
    }
    

    HANDLE* hThread;
    DWORD* IDThread;

    hThread = new HANDLE[amount];
    IDThread = new DWORD[amount];

    for (int i = 0; i < amount; ++i)
    {
        hThread[i] = CreateThread(NULL, 0, FindElement, (LPVOID)&matrix, 0, &IDThread[0]);
        if (hThread[i] == NULL)
            return GetLastError();
    }

    WaitForMultipleObjects(2, hEvent, TRUE, INFINITE);
    SetEvent(hEventT);
    WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

    cout << "Первая матрица:\n";
    printMatrix(matrix1);
    cout << "\nВторая матрица:\n";
    printMatrix(matrix2);

    vector<vector<double>> result = matrix.res;

    cout << "\nРезультат умножения:\n";
    printMatrix(result);


    for (int i = 0; i < amount; ++i)
    {
        CloseHandle(hThread[i]);
    }
    DeleteCriticalSection(&criticalSection);
    return 0;
}
