#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <iomanip>
using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ios_base;
using std::cin;
using std::cout;
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

DWORD WINAPI FindElement(LPVOID mmatrix)
{
    if (temp < (reinterpret_cast<MMatrix*>(mmatrix))->amount)
    {
        SetEvent(hEvent[temp]);
        ++temp;
    }
    WaitForSingleObject(hEventT, INFINITE);
    bool b = true;
    while (b)
    {
        std::cout << "Thread FindElment is started.\n";
        EnterCriticalSection(&criticalSection); //Suspend all threads to retrieve indexes
        int i = (reinterpret_cast<MMatrix*>(mmatrix))->i;
        int j = (reinterpret_cast<MMatrix*>(mmatrix))->j;
        if (i < (reinterpret_cast<MMatrix*>(mmatrix))->rows1)
        {
            (reinterpret_cast<MMatrix*>(mmatrix))->j = (j + 1) % (reinterpret_cast<MMatrix*>(mmatrix))->cols2;
            if (0 == (reinterpret_cast<MMatrix*>(mmatrix))->j) ++(reinterpret_cast<MMatrix*>(mmatrix))->i;
            LeaveCriticalSection(&criticalSection); //then open access

            for (int k = 0; k < (reinterpret_cast<MMatrix*>(mmatrix))->cols1; ++k)
            {
                (reinterpret_cast<MMatrix*>(mmatrix))->res[i][j] += (reinterpret_cast<MMatrix*>(mmatrix))->mat1[i][k] * (reinterpret_cast<MMatrix*>(mmatrix))->mat2[k][j];
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

DWORD WINAPI PrintMatrix(LPVOID mmatrix)
{
    ofstream fout((reinterpret_cast<MMatrix*>(mmatrix))->nameOfFile, ios_base::app);
    if (!fout.is_open())    //check if the structure is empty
    {
        throw "Cannot open file";
    }
    fout << "\n";
    if ((reinterpret_cast<MMatrix*>(mmatrix))->res.empty())
    {
        throw "Empty matrix";
    }
    for (int i = 0; i < (reinterpret_cast<MMatrix*>(mmatrix))->rows1; ++i)
    {
        if ((reinterpret_cast<MMatrix*>(mmatrix))->res[i].empty())
        {
            throw "Empty matrix";
        }
        for (int j = 0; j < (reinterpret_cast<MMatrix*>(mmatrix))->cols2; ++j)
        {
            fout << (reinterpret_cast<MMatrix*>(mmatrix))->res[i][j] << " ";
        }
        fout << "\n";
    }
    return 0;
}

void PrintMatrixOnConsole(const vector<vector<double>>& mat)
{
    if (mat.empty())    //check if the structure is empty
    {
        throw "Empty matrix";
    }
    for (const auto& row : mat)
    {
        if (row.empty())
        {
            throw "Empty matrix";
        }
        for (double val : row)
        {
            cout << val << " ";
        }
        cout << "\n";
    }
}

vector<vector<double>> GetMatrix(ifstream& fin)
{
    int a, b;
    fin >> a >> b;
    if (a <= 0 || b <= 0)   //check if the input is empty
    {
        throw "Empty matrix";
    }
    vector<vector<double>> matrix(a, vector<double>(b));
    for (int i = 0; i < a; ++i)
    {
        for (int j = 0; j < b; ++j)
        {
            fin >> matrix[i][j];
        }
    }
    return matrix;
}

void GoodToMultiply(MMatrix& a)
{
    if (a.cols1 != a.rows2)
    {
        throw "Matrix cannot be multiplied.";   //count of colomn first matrix not equals 
    }                                           //count of rows second matrix
}

int main(int argc, char* argv[])
{
    InitializeCriticalSection(&criticalSection);
    cout << "Enter name of file:\n";
    string s;
    s = argv[1];
    //cin >> s;
    ifstream fin(s);
    cout << "Enter amount of thread:\n";
    int amount;
    amount = atoi(argv[2]);
    //cin >> amount;
    hEvent = new HANDLE[amount];
    for (int i = 0; i < amount; ++i)
    {
        hEvent[i] = CreateEvent(NULL, TRUE, 0, NULL);
    }
    hEventT = CreateEvent(NULL, TRUE, 0, NULL);
    temp = 0;

    vector<vector<double>> matrix1 = GetMatrix(fin);
    vector<vector<double>> matrix2 = GetMatrix(fin);

    fin.close();

    MMatrix matrix(matrix1, matrix2, amount, s);

    GoodToMultiply(matrix);

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

    WaitForMultipleObjects(amount, hEvent, TRUE, INFINITE); //Waiting for opening all threads
    SetEvent(hEventT);                                      //Signal the start of work 
    WaitForMultipleObjects(amount, hThread, TRUE, INFINITE);//Waiting for closing all threads

    cout << "First matrix:\n";
    PrintMatrixOnConsole(matrix1);
    cout << "\nSecond matrix:\n";
    PrintMatrixOnConsole(matrix2);

    vector<vector<double>> result = matrix.res;

    cout << "\nMultiplication result:\n";
    PrintMatrixOnConsole(result);
    HANDLE OutThread = CreateThread(NULL, 0, PrintMatrix, (LPVOID)&matrix, 0, &IDThread[0]);
    if (OutThread == NULL)
        return GetLastError();

    WaitForSingleObject(OutThread, INFINITE);

    for (int i = 0; i < amount; ++i)
    {
        CloseHandle(hThread[i]);
        CloseHandle(hEvent[i]);
    }
    delete[] hThread;
    delete[] hEvent;
    CloseHandle(OutThread);
    CloseHandle(hEventT);
    DeleteCriticalSection(&criticalSection);
    return 0;
}
