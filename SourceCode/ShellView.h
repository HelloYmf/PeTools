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

//加载PE文件
BOOL LordPe();
//PE文件->FileBuffer

//新增节
BOOL AddSection();