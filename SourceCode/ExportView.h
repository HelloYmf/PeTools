#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include "resource.h"

BOOL CALLBACK DialogPrintExportProc(									
						 HWND hwndDlg,  // handle to dialog box			
						 UINT uMsg,     // message			
						 WPARAM wParam, // first message parameter			
						 LPARAM lParam  // second message parameter			
						 );

//初始化表头
VOID InitExportHeader(HWND hwndDlg);
//打印导出表
VOID PrintExport(HWND hwndDlg);