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

//��ʼ����ͷ
VOID IintSectionListView(HWND hDlg);
//�������
VOID FillSectionData(HWND hDlg);