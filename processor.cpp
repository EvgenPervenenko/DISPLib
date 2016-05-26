#include "processor.h"
#include <windows.h>
#include <strsafe.h>
#include <QFile>
#include <QDebug>

#pragma comment( lib, "user32.lib")
#pragma comment( lib, "gdi32.lib")

#define NUMHOOKS 7

// Global variables

typedef struct _MYHOOKDATA
{
    int nType;
    HOOKPROC hkprc;
    HHOOK hhook;
} MYHOOKDATA;

MYHOOKDATA myhookdata[NUMHOOKS];

const int IDM_CALLWNDPROC = 0;
const int IDM_CBT = 1;
const int IDM_DEBUG = 2;
const int IDM_GETMESSAGE = 3;
const int IDM_KEYBOARD = 4;
const int IDM_MOUSE = 5;
const int IDM_MSGFILTER = 6;

extern "C" void Init( HINSTANCE hinstDll )
{
  // Initialize structures with hook data. The menu-item identifiers are
  // defined as 0 through 6 in the header file app.h. They can be used to
  // identify array elements both here and during the WM_COMMAND message.

  myhookdata[IDM_CALLWNDPROC].nType = WH_CALLWNDPROC;
  myhookdata[IDM_CALLWNDPROC].hkprc = CallWndProc;
  myhookdata[IDM_CBT].nType = WH_CBT;
  myhookdata[IDM_CBT].hkprc = CBTProc;
  myhookdata[IDM_DEBUG].nType = WH_DEBUG;
  myhookdata[IDM_DEBUG].hkprc = DebugProc;
  myhookdata[IDM_GETMESSAGE].nType = WH_GETMESSAGE;
  myhookdata[IDM_GETMESSAGE].hkprc = GetMsgProc;
  myhookdata[IDM_KEYBOARD].nType = WH_KEYBOARD;
  myhookdata[IDM_KEYBOARD].hkprc = KeyboardProc;
  myhookdata[IDM_MOUSE].nType = WH_MOUSE;
  myhookdata[IDM_MOUSE].hkprc = MouseProc;
  myhookdata[IDM_MSGFILTER].nType = WH_MSGFILTER;
  myhookdata[IDM_MSGFILTER].hkprc = MessageProc;

  for ( auto index = 0; index <= IDM_MSGFILTER; ++index )
    {
      qDebug() << "Hook set : " << myhookdata[index].nType;
      myhookdata[index].hhook = SetWindowsHookEx(
            myhookdata[index].nType,
            myhookdata[index].hkprc,
            hinstDll, 0);
      if ( myhookdata[index].hhook == NULL )
        qDebug() << "Not set!";
    }
}

extern "C" void Unhook()
{
  for ( auto index = 0; index <= IDM_MSGFILTER; ++index )
    {
      UnhookWindowsHookEx(myhookdata[index].hhook);
      myhookdata[index].hhook = NULL;
    }
}

void WriteLog( const QString &str )
{
  qDebug() << "writerLog" << str;
	QFile file( "TempFile.txt" );
	if ( file.open( QIODevice::WriteOnly | QIODevice::Append ) )
	{
	    QByteArray array;
	    array.append( str );
		file.write( array );
		file.close();
	}
}

/****************************************************************
  WH_CALLWNDPROC hook procedure
 ****************************************************************/

extern "C" LRESULT WINAPI CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    WCHAR szCWPBuf[256];
    WCHAR szMsg[16];
    static int c = 0;
    size_t cch;
    HRESULT hResult;

    if (nCode < 0)  // do not process message
        return CallNextHookEx(myhookdata[IDM_CALLWNDPROC].hhook, nCode, wParam, lParam);

    // Call an application-defined function that converts a message
    // constant to a string and copies it to a buffer.

    LookUpTheMessage((PMSG) lParam, (LPWSTR)szMsg);

    switch (nCode)
    {
        case HC_ACTION:
            hResult = StringCchPrintf(szCWPBuf, 256/sizeof(TCHAR),
               L"CALLWNDPROC - tsk: %ld, msg: %s, %d times   ",
                wParam, szMsg, c++);
            if (FAILED(hResult))
            {
            // TODO: writer error handler
            }
            hResult = StringCchLength(szCWPBuf, 256/sizeof(TCHAR), &cch);
            if (FAILED(hResult))
            {
            // TODO: write error handler
            }
            break;

        default:
            break;
    }

    return CallNextHookEx(myhookdata[IDM_CALLWNDPROC].hhook, nCode, wParam, lParam);
}

/****************************************************************
  WH_GETMESSAGE hook procedure
 ****************************************************************/

extern "C" LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    WCHAR szMSGBuf[256];
    WCHAR szRem[16];
    WCHAR szMsg[16];
    HDC hdc;
    static int c = 0;
    size_t cch;
    HRESULT hResult;

    if (nCode < 0) // do not process message
        return CallNextHookEx(myhookdata[IDM_GETMESSAGE].hhook, nCode,
            wParam, lParam);

    switch (nCode)
    {
        case HC_ACTION:
            switch (wParam)
            {
                case PM_REMOVE:
                    hResult = StringCchCopy((STRSAFE_LPWSTR)szRem, 16/sizeof(WCHAR), L"PM_REMOVE");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
                    break;

                case PM_NOREMOVE:
                    hResult = StringCchCopy((STRSAFE_LPWSTR)szRem, 16/sizeof(WCHAR), L"PM_NOREMOVE");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
                    break;

                default:
                    hResult = StringCchCopy((STRSAFE_LPWSTR)szRem, 16/sizeof(WCHAR), L"Unknown");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
                    break;
            }

            // Call an application-defined function that converts a
            // message constant to a string and copies it to a
            // buffer.

            LookUpTheMessage((PMSG) lParam, (LPWSTR)szMsg);

            hResult = StringCchPrintf(szMSGBuf, 256/sizeof(WCHAR),
                L"GETMESSAGE - wParam: %s, msg: %s, %d times   ",
                szRem, szMsg, c++);
            if (FAILED(hResult))
            {
            // TODO: write error handler
            }
            hResult = StringCchLength((STRSAFE_PCNZWCH)szMSGBuf, 256/sizeof(WCHAR), &cch);
            if (FAILED(hResult))
            {
            // TODO: write error handler
            }
            break;

        default:
            break;
    }

    return CallNextHookEx(myhookdata[IDM_GETMESSAGE].hhook, nCode, wParam, lParam);
}

/****************************************************************
  WH_DEBUG hook procedure
 ****************************************************************/

extern "C" LRESULT CALLBACK DebugProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    WCHAR szBuf[128];
    static int c = 0;
    size_t cch;
    HRESULT hResult;

    if (nCode < 0)  // do not process message
        return CallNextHookEx(myhookdata[IDM_DEBUG].hhook, nCode,
            wParam, lParam);

    switch (nCode)
    {
        case HC_ACTION:
            hResult = StringCchPrintf(szBuf, 128/sizeof(WCHAR),
                L"DEBUG - nCode: %d, tsk: %ld, %d times   ",
                nCode,wParam, c++);
            if (FAILED(hResult))
            {
            // TODO: write error handler
            }
            hResult = StringCchLength(szBuf, 128/sizeof(WCHAR), &cch);
            if (FAILED(hResult))
            {
            // TODO: write error handler
            }
            break;

        default:
            break;
    }

    return CallNextHookEx(myhookdata[IDM_DEBUG].hhook, nCode, wParam, lParam);
}

/****************************************************************
  WH_CBT hook procedure
 ****************************************************************/

extern "C" LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    WCHAR szBuf[128];
    WCHAR szCode[128];
    static int c = 0;
    size_t cch;
    HRESULT hResult;

    if (nCode < 0)  // do not process message
        return CallNextHookEx(myhookdata[IDM_CBT].hhook, nCode, wParam,
            lParam);

    switch (nCode)
    {
        case HCBT_ACTIVATE:
            hResult = StringCchCopy(szCode, 128/sizeof(WCHAR), L"HCBT_ACTIVATE");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;

        case HCBT_CLICKSKIPPED:
            hResult = StringCchCopy(szCode, 128/sizeof(WCHAR), L"HCBT_CLICKSKIPPED");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;

        case HCBT_CREATEWND:
            hResult = StringCchCopy(szCode, 128/sizeof(WCHAR), L"HCBT_CREATEWND");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;

        case HCBT_DESTROYWND:
            hResult = StringCchCopy(szCode, 128/sizeof(WCHAR), L"HCBT_DESTROYWND");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;

        case HCBT_KEYSKIPPED:
            hResult = StringCchCopy(szCode, 128/sizeof(WCHAR), L"HCBT_KEYSKIPPED");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;

        case HCBT_MINMAX:
            hResult = StringCchCopy(szCode, 128/sizeof(WCHAR), L"HCBT_MINMAX");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;

        case HCBT_MOVESIZE:
            hResult = StringCchCopy(szCode, 128/sizeof(WCHAR), L"HCBT_MOVESIZE");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;

        case HCBT_QS:
            hResult = StringCchCopy(szCode, 128/sizeof(WCHAR), L"HCBT_QS");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;

        case HCBT_SETFOCUS:
            hResult = StringCchCopy(szCode, 128/sizeof(WCHAR), L"HCBT_SETFOCUS");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;

        case HCBT_SYSCOMMAND:
            hResult = StringCchCopy(szCode, 128/sizeof(WCHAR), L"HCBT_SYSCOMMAND");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;

        default:
            hResult = StringCchCopy(szCode, 128/sizeof(WCHAR), L"Unknown");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;
    }
    hResult = StringCchPrintf(szBuf, 128/sizeof(WCHAR), L"CBT -  nCode: %s, tsk: %ld, %d times   ",
        szCode, wParam, c++);
    if (FAILED(hResult))
    {
    // TODO: write error handler
    }
    hResult = StringCchLength(szBuf, 128/sizeof(WCHAR), &cch);
    if (FAILED(hResult))
    {
    // TODO: write error handler
    }

    return CallNextHookEx(myhookdata[IDM_CBT].hhook, nCode, wParam, lParam);
}

/****************************************************************
  WH_MOUSE hook procedure
 ****************************************************************/

extern "C" LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    WCHAR szBuf[128];
    WCHAR szMsg[16];
    static int c = 0;
    size_t cch;
    HRESULT hResult;

    if (nCode < 0)  // do not process the message
        return CallNextHookEx(myhookdata[IDM_MOUSE].hhook, nCode,
            wParam, lParam);

    // Call an application-defined function that converts a message
    // constant to a string and copies it to a buffer.

    LookUpTheMessage((PMSG) lParam, szMsg);

    hResult = StringCchPrintf(szBuf, 128/sizeof(TCHAR),
        L"MOUSE - nCode: %d, msg: %s, x: %d, y: %d, %d times   ",
        nCode, szMsg, LOWORD(lParam), HIWORD(lParam), c++);
    if (FAILED(hResult))
    {
    // TODO: write error handler
    }
    hResult = StringCchLength(szBuf, 128/sizeof(TCHAR), &cch);
    if (FAILED(hResult))
    {
    // TODO: write error handler
    }

    return CallNextHookEx(myhookdata[IDM_MOUSE].hhook, nCode, wParam, lParam);
}

/****************************************************************
  WH_KEYBOARD hook procedure
 ****************************************************************/

extern "C" LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    WCHAR szBuf[128];
    static int c = 0;
    size_t cch;
    HRESULT hResult;

    if (nCode < 0)  // do not process message
        return CallNextHookEx(myhookdata[IDM_KEYBOARD].hhook, nCode,
            wParam, lParam);

    hResult = StringCchPrintf(szBuf, 128/sizeof(WCHAR), L"KEYBOARD - nCode: %d, vk: %d, %d times ", nCode, wParam, c++);
    if (FAILED(hResult))
    {
    // TODO: write error handler
    }
    hResult = StringCchLength(szBuf, 128/sizeof(WCHAR), &cch);
    if (FAILED(hResult))
    {
    // TODO: write error handler
    }

    auto i( 0 );
    QString str;
    while( szBuf[i] != '\0' )
      {
        str.append( QChar( szBuf[i] ) );
        ++i;
      }

    WriteLog( str );

    return CallNextHookEx(myhookdata[IDM_KEYBOARD].hhook, nCode, wParam, lParam);
}

/****************************************************************
  WH_MSGFILTER hook procedure
 ****************************************************************/

extern "C" LRESULT CALLBACK MessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    WCHAR szBuf[128];
    WCHAR szMsg[16];
    WCHAR szCode[32];
    static int c = 0;
    size_t cch;
    HRESULT hResult;

    if (nCode < 0)  // do not process message
        return CallNextHookEx(myhookdata[IDM_MSGFILTER].hhook, nCode,
            wParam, lParam);

    switch (nCode)
    {
        case MSGF_DIALOGBOX:
            hResult = StringCchCopy(szCode, 32/sizeof(WCHAR), L"MSGF_DIALOGBOX");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;

        case MSGF_MENU:
            hResult = StringCchCopy(szCode, 32/sizeof(WCHAR), L"MSGF_MENU");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;

        case MSGF_SCROLLBAR:
            hResult = StringCchCopy(szCode, 32/sizeof(WCHAR), L"MSGF_SCROLLBAR");
                    if (FAILED(hResult))
                    {
                    // TODO: write error handler
                    }
            break;

        default:
            hResult = StringCchPrintf(szCode, 128/sizeof(WCHAR), L"Unknown: %d", nCode);
    if (FAILED(hResult))
    {
    // TODO: write error handler
    }
            break;
    }

    // Call an application-defined function that converts a message
    // constant to a string and copies it to a buffer.

    LookUpTheMessage((PMSG) lParam, szMsg);

    hResult = StringCchPrintf(szBuf, 128/sizeof(WCHAR),
        L"MSGFILTER  nCode: %s, msg: %s, %d times    ",
        szCode, szMsg, c++);
    if (FAILED(hResult))
    {
    // TODO: write error handler
    }
    hResult = StringCchLength(szBuf, 128/sizeof(WCHAR), &cch);
    if (FAILED(hResult))
    {
    // TODO: write error handler
    }

    return CallNextHookEx(myhookdata[IDM_MSGFILTER].hhook, nCode, wParam, lParam);
}


size_t LookUpTheMessage(PMSG wParam, LPWSTR szBuf)
{
  size_t cch(0);
  auto hResult = StringCchPrintf((STRSAFE_LPWSTR)szBuf, 128/sizeof(WCHAR),
                            L"%d",
                            wParam);
  if (FAILED(hResult))
  {
  // TODO: write error handler
  }

  hResult = StringCchLength((STRSAFE_LPWSTR)szBuf, 128/sizeof(WCHAR), &cch);

  return cch;
}
