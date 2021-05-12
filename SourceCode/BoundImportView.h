#pragma once
#include <Windows.h>
#include "commctrl.h"
#include "resource.h"
#include <string>
using namespace std;

BOOL CALLBACK DialogBoundImportProc(									
						 HWND hwndDlg,  // handle to dialog box			
						 UINT uMsg,     // message			
						 WPARAM wParam, // first message parameter			
						 LPARAM lParam  // second message parameter			
						 );

VOID PrintBoundImport(HWND hwndDlg);