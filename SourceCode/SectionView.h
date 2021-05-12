#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <Tlhelp32.h>
#include "resource.h"

BOOL CALLBACK DialogSectionProc(									
						 HWND hwndDlg,  // handle to dialog box			
						 UINT uMsg,     // message			
						 WPARAM wParam, // first message parameter			
						 LPARAM lParam  // second message parameter			
						 );

//初始化表头
VOID IintSectionListView(HWND hDlg);
//填充数据
VOID FillSectionData(HWND hDlg);