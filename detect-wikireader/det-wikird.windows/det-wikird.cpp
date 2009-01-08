/*
 * (C) Copyright 2008 OpenMoko, Inc.
 * Author: xiangfu liu <xiangfu@openmoko.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <dbt.h> 
#include <check-wikireader.h>

static char *AppTitle = "det-wikird Openmoko";
#define MSG_BUFFER_LENGTH 100
static char g_Msg[MSG_BUFFER_LENGTH];
static HWND g_hwnd;
static bool g_bInSysTray = TRUE;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
char FirstDriveFromMask(ULONG unitmask);
void MinimizeToSysTray();

int WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
{
    strcpy(g_Msg,"Openmoko \n        Detect WikiReader Program");
    WNDCLASS wc;
    HWND hwnd;
    MSG msg;

    wc.style=CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc=WindowProc;
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hInstance=hInst;
    wc.hIcon=LoadIcon(NULL,IDI_WINLOGO);
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground=(HBRUSH)COLOR_WINDOWFRAME;
    wc.lpszMenuName=NULL;
    wc.lpszClassName=AppTitle;

    if (!RegisterClass(&wc))
        return 0;

    hwnd = CreateWindow(AppTitle,AppTitle,
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,CW_USEDEFAULT,300,100,
                        NULL,NULL,hInst,NULL);

    if (!hwnd)
        return 0;
    g_hwnd = hwnd;

    MinimizeToSysTray();
    UpdateWindow(hwnd);

    while (GetMessage(&msg,NULL,0,0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    return 0;
}

int PaintMessage(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC dc;
    RECT r;
    GetClientRect(hwnd,&r);
    dc=BeginPaint(hwnd,&ps);
    DrawText(dc, g_Msg, -1, &r, DT_LEFT);
    EndPaint(hwnd,&ps);
    return 0;
}

int Message(char *msg)
{
    if (msg != NULL) {
        if (strlen(g_Msg) + strlen(msg) < MSG_BUFFER_LENGTH) {
            strcat(g_Msg, "\n");
            strcat(g_Msg, msg);
        }
        else
            strcpy(g_Msg, msg);
    }
    PaintMessage(g_hwnd); 
    InvalidateRect(g_hwnd, NULL, TRUE);
    UpdateWindow(g_hwnd);
    return 0;
}

void openWeb()
{
    HKEY hkRoot,hSubKey;
    char ValueName[256];
    unsigned char DataValue[256];
    unsigned long cbValueName=256;
    unsigned long cbDataValue=256;
    char ShellChar[256]; // shell command
    DWORD dwType;

    // open register root key
    if(RegOpenKey(HKEY_CLASSES_ROOT,NULL,&hkRoot)==ERROR_SUCCESS)
        {
            // open subkey
            if(RegOpenKeyEx(hkRoot,
                            "htmlfile\\shell\\open\\command",
                            0,
                            KEY_ALL_ACCESS,
                            &hSubKey)==ERROR_SUCCESS)
                {
                    // get the browers
                    RegEnumValue(hSubKey,
                                 0,
                                 ValueName,
                                 &cbValueName,
                                 NULL,
                                 &dwType,
                                 DataValue,
                                 &cbDataValue);
                    // set main page value
                    strcpy(ShellChar,(char *)DataValue);
                    strcat(ShellChar,"http://wiki.openmoko.org");
                    // run the browers
                    WinExec(ShellChar,SW_SHOW);
                }
            else
                Message("WEB browers open error.");
        }
    else
        Message("WEB browers open error.");

    RegCloseKey(hSubKey);
    RegCloseKey(hkRoot);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
    case WM_PAINT:
        PaintMessage(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_DEVICECHANGE:
        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lparam;

        switch (wparam) { 
        case DBT_DEVICEARRIVAL:
            Message("Debug: A device has been inserted.");
            if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;

                UINT   IsRAMDISK;    
                char Driver[4] = {'A', ':', '\\', '\0'};
                Driver[0] = FirstDriveFromMask(lpdbv->dbcv_unitmask);
                IsRAMDISK=GetDriveType(Driver);    
                if (IsRAMDISK == DRIVE_RAMDISK || 
                    IsRAMDISK == DRIVE_FIXED || /* DRIVE_FIXED means harddisk */
                    IsRAMDISK == DRIVE_REMOVABLE) {
                    wsprintf(g_Msg, "Drive %s Media has arrived.\n", Driver);
                    Message(NULL);
					/* check_wikireader(Driver); */
                    openWeb();  /*FIXME:  there should check the wikireader the openWeb  */ 
                }
            }
            break;
 
        case DBT_DEVICEREMOVECOMPLETE:
            Message("Debug:  A device has been removed");
            break;
        }
        break;

    case WM_USER:
        switch(lparam)
            {
            case WM_LBUTTONDBLCLK:
                if (g_bInSysTray)
                    ShowWindow(g_hwnd,SW_SHOW);
                else
                    MinimizeToSysTray();
                g_bInSysTray = !g_bInSysTray;
                break;
            case WM_LBUTTONUP:
                break;
            }
        break;
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}
/*------------------------------------------------------------------
FirstDriveFromMask (unitmask)

Description
Finds the first valid drive letter from a mask of drive letters.
The mask must be in the format bit 0 = A, bit 1 = B, bit 3 = C,
etc. A valid drive letter is defined when the corresponding bit
is set to 1.

Returns the first drive letter that was found.
--------------------------------------------------------------------*/
char FirstDriveFromMask(ULONG unitmask) 
{ 
    char i; 
    for (i = 0; i < 26; ++i) 
        { 
            if (unitmask & 0x1) 
                break; 
            unitmask = unitmask >> 1; 
        } 
    return (i + 'A'); 
} 

void MinimizeToSysTray() {
	// Hide the window
	ShowWindow(g_hwnd,SW_HIDE);

	// Show the notification icon
	NOTIFYICONDATA nid;
	ZeroMemory(&nid,sizeof(nid));
	nid.cbSize =	sizeof(nid);
	nid.hWnd	=	g_hwnd;
	nid.uID	=	0;
	nid.uFlags =	NIF_ICON | NIF_MESSAGE | NIF_TIP ;
	nid.uCallbackMessage	=	WM_USER;
	nid.hIcon	=	LoadIcon(NULL, MAKEINTRESOURCE(NULL));
	lstrcpy(nid.szTip,"WikiReader Detect (doucle click to maximize)\n Openmoko, Inc.");

	Shell_NotifyIcon(NIM_ADD,&nid);
}
