#include<iostream>
#include<Windows.h>
using namespace std;

DWORD WINAPI Fun1Proc(LPVOID lpParameter)
{
    cout << "thread function Fun1Proc!\n";

    return 0;
}

int main()
{
    HANDLE hThread1 = CreateThread(NULL, 0, Fun1Proc, NULL, 0, NULL);
    HANDLE hThread2 = CreateThread(NULL, 0, Fun1Proc, NULL, 0, NULL);
    CloseHandle(hThread1);

    Sleep(1000);
    cout << "main end!\n";
    system("pause");
    return 0;
}
