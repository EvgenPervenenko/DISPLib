#include <Windows.h>
#include <strsafe.h>


extern "C" void Init(HINSTANCE hinstDll);
extern "C" void Unhook();
// Hook procedures

extern "C" LRESULT WINAPI CallWndProc(int, WPARAM, LPARAM);
extern "C" LRESULT WINAPI CBTProc(int, WPARAM, LPARAM);
extern "C" LRESULT WINAPI DebugProc(int, WPARAM, LPARAM);
extern "C" LRESULT WINAPI GetMsgProc(int, WPARAM, LPARAM);
extern "C" LRESULT WINAPI KeyboardProc(int, WPARAM, LPARAM);
extern "C" LRESULT WINAPI MouseProc(int, WPARAM, LPARAM);
extern "C" LRESULT WINAPI MessageProc(int, WPARAM, LPARAM);

size_t LookUpTheMessage(PMSG, LPWSTR);
