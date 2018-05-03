// MulThread.cpp : 定义控制台应用程序的入口点。
//


#include <iostream>
#include <windows.h>
#include <process.h>
#include <fstream>
#include <stdlib.h>
#include <cstdio>
using namespace std;


DWORD WINAPI Fun1Proc(
	LPVOID lpParameter   // thread data
	);

DWORD WINAPI Fun2Proc(
	LPVOID lpParameter   // thread data
	);
int index = 0;
int tickets = 50;

void UseSleep()
{
       Sleep(1);
       cout<<" i was used"<<endl;
}

HANDLE hMutex;
int main()
{
	HANDLE hThread1;
	HANDLE hThread2;
	hThread1 = CreateThread(NULL, 0, Fun1Proc, NULL, 0, NULL);
	hThread2 = CreateThread(NULL, 0, Fun2Proc, NULL, 0, NULL);
	CloseHandle(hThread1);
	CloseHandle(hThread2);

	//    hMutex=CreateMutex(NULL,TRUE,NULL);
        //如果是VS2013 hMutex = CreateMutex(NULL, TRUE, L"queue");
        hMutex = CreateMutex(NULL, TRUE, "queue");
	if (hMutex)
	{
		if (ERROR_ALREADY_EXISTS == GetLastError())
		{
			cout << "only one instance can run!" << endl;
			return 0;
		}
	}
	//WaitForSingleObject(hMutex, INFINITE);
	//ReleaseMutex(hMutex);
	ReleaseMutex(hMutex);

	//Sleep(4000);
	system("pause");
	return 0;
}

DWORD WINAPI Fun1Proc(
	LPVOID lpParameter   // thread data
	)
{
	   while(TRUE)
	{
        WaitForSingleObject(hMutex,INFINITE);
        if(tickets>0)
        {
        UseSleep();
        cout<<"thread1 sell ticket : "<<tickets--<<endl;
        }
        else
        break;
        ReleaseMutex(hMutex);
	}
	return 0;
}


DWORD WINAPI Fun2Proc(
	LPVOID lpParameter   // thread data
	)
{
    while(TRUE)
	{
        WaitForSingleObject(hMutex,INFINITE);
        if(tickets>0)
        {
        Sleep(1);
        cout<<"thread2 sell ticket : "<<tickets--<<endl;
        }
        else
        break;
        ReleaseMutex(hMutex);
	}
	return 0;
}
