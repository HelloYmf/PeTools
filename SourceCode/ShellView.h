#pragma once
#include <Windows.h>
#include "resource.h"
#include "PeView.h"

BOOL CALLBACK DialogShellProc(									
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message			
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
	);

//����PE�ļ�
BOOL LordPe();
//PE�ļ�->FileBuffer

//������
BOOL AddSection();