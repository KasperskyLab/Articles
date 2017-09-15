/*
    Copyright 2017 AO Kaspersky Lab. All Rights Reserved.

    Anti-Sandboxing: Wait for Mouse Click PoC: https://wikileaks.org/ciav7p1/cms/page_2621847.html

    RU: https://securelist.ru/a-modern-hypervisor-as-a-basis-for-a-sandbox/80739/
    EN: https://securelist.com/a-modern-hypervisor-as-a-basis-for-a-sandbox/81902/
*/

#include "stdafx.h"

#include <windows.h>

#include <iostream>
#include <thread>
#include <atomic>

HHOOK global_hook = nullptr;
std::atomic<bool> global_ready(true);

void ExecuteEvil() {
    std::cout << "This will never be executed in Sandbox" << std::endl;

    // TODO: add your EVIL code here

    UnhookWindowsHookEx(global_hook);
    ExitProcess(42);
}

LRESULT CALLBACK LowLevelMouseProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam) {
    if ( nCode < 0 ) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if ( nCode == HC_ACTION && wParam == WM_LBUTTONUP && global_ready == true ) {
        global_ready = false;
        std::thread(ExecuteEvil).detach();  // execute EVIL thread detached
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

int _tmain(int argc, _TCHAR* argv[]) {
    FreeConsole();  // hide console window

    global_hook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);

    // emulate message queue
    MSG msg;

    while ( GetMessage(&msg, NULL, 0, 0) ) {
        Sleep(0);
    }

    return 0;
}
