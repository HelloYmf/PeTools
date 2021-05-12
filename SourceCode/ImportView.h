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

//��ʼ����ͷ
VOID InitImportHeader(HWND hwndDlg);
//��ӡ������
VOID PrintImport(HWND hwndDlg);