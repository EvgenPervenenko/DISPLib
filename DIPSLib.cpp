/******************************************************************************
Module:  DIPSLib.cpp
Notices: Copyright (c) 2000 Jeffrey Richter
******************************************************************************/
#pragma comment( lib, "user32.lib") 

#include "CmnHdr.h"
#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include "DIPSLib.h"
#include "Resource.h"
#include <QDebug>
#include <QFile>
#include <strsafe.h>
#include "processor.h"

///////////////////////////////////////////////////////////////////////////////

// Forward references
extern "C" LRESULT WINAPI GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////////////


// Instruct the compiler to put the g_hhook data variable in 
// its own data section called Shared. We then instruct the 
// linker that we want to share the data in this section 
// with all instances of this application.
#pragma data_seg("Shared")
HHOOK g_keyBoardHook = NULL;
DWORD g_dwThreadIdDIPS = 0;
#pragma data_seg()

// Instruct the linker to make the Shared section
// readable, writable, and shared.
#pragma comment(linker, "/section:Shared,rws")


///////////////////////////////////////////////////////////////////////////////


// Nonshared variables
HINSTANCE g_hinstDll = NULL;


///////////////////////////////////////////////////////////////////////////////


BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, PVOID fImpLoad) {
	
	switch (fdwReason) {
	
	case DLL_PROCESS_ATTACH:
		// DLL is attaching to the address space of the current process.
		g_hinstDll = hinstDll;
		break;
		
	case DLL_THREAD_ATTACH:
		// A new thread is being created in the current process.
		break;
		
	case DLL_THREAD_DETACH:
		// A thread is exiting cleanly.
		break;
		
	case DLL_PROCESS_DETACH:
		// The calling process is detaching the DLL from its address space.
		Unhook();
		break;
	}
	return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////

extern "C" BOOL WINAPI SetDIPSHook(DWORD dwThreadId) {
	
	BOOL fOk = FALSE;
	
//	if (dwThreadId != 0)
	{
		Init( g_hinstDll );
	}
	
	return(fOk);
}


///////////////////////////////////////////////////////////////////////////////
