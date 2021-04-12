// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

// ApiHook.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include <windows.h>

byte* VABase;
typedef int(__stdcall* Func)(DWORD, DWORD, DWORD, DWORD, DWORD);
Func func;

void AttackFunc(IN HANDLE hProcess, IN PVOID BaseAddress, IN PVOID Buffer, IN ULONG BytesToWrite, OUT PULONG BytesWritten)
{
    //func((DWORD)hProcess, (DWORD)BaseAddress, (DWORD)Buffer, BytesToWrite, (DWORD)BytesWritten);
    MessageBox(0, L"infected", 0, 0);
}

int modifyApi()
{
    HMODULE ntdll = LoadLibraryA("ntdll.dll");
    byte* ZwBase = (byte*)GetProcAddress(ntdll, "NtOpenProcess");
    DWORD lpflOldProtect = 0;
    if (VirtualProtect(ZwBase, 10, PAGE_EXECUTE_READWRITE, &lpflOldProtect))
    {
        VABase = (byte*)VirtualAlloc(0, 10, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        memcpy(VABase, ZwBase, 5);
        VABase[5] = 0xE9;
        *(int*)(VABase + 6) = ZwBase - VABase - 5;//set jmp addr point
        ZwBase[0] = 0xE9;
        *(int*)(ZwBase + 1) = (byte*)AttackFunc - ZwBase - 5;
        if (!VirtualProtect(VABase, 10, PAGE_EXECUTE_READWRITE, &lpflOldProtect))
        {
            return -1;
        }
        func = (int(__stdcall*)(DWORD, DWORD, DWORD, DWORD, DWORD))VABase;

    }
    return 1;
}


int main()
{
    /*
    确定要几个数据
    ZwWriteVirtualMemory的第一条指令
    跳转到第二条指令
    把ZwWriteVirtualMemory第一条指令修改为跳转到指定内存空间
*/
    STARTUPINFOA st = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    if (modifyApi())
    {
        //OpenProcess()
        //CreateProcessA("C:\\Windows\\System32\\calc.exe", 0, 0, 0, 0, 0, 0, 0, &st, &pi);
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 18924);
        int err = GetLastError();
    }
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
