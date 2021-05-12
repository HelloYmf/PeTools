#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include "resource.h"

BOOL CALLBACK DialogPrintImportProc(									
						 HWND hwndDlg,  // handle to dialog box			
						 UINT uMsg,     // message			
						 WPARAM wParam, // first message parameter			
						 LPARAM lParam  // second message parameter			
						 );

//初始化表头
VOID InitImportHeader(HWND hwndDlg);
//打印导入表表
VOID PrintImport(HWND hwndDlg);