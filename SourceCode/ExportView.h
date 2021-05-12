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

//��ʼ����ͷ
VOID InitExportHeader(HWND hwndDlg);
//��ӡ������
VOID PrintExport(HWND hwndDlg);