#pragma once
#include <Windows.h>
#include "resource.h"
#include "SectionView.h"
#include "DirectoryView.h"
#include <stdio.h>

DWORD RvaToFoa(DWORD dwRva);

//获取打开文件路径
TCHAR* GetOpenFileName(HWND hwndDlg);

//加载PE，加载到FileBuffer
size_t LoadPE(TCHAR* wPath , PBYTE* Buffer);

//填充PE信息
VOID FillPEData(TCHAR* OpenName,HWND hwndDlg);

BOOL CALLBACK DialogPEProc(									
						 HWND hwndDlg,  // handle to dialog box			
						 UINT uMsg,     // message			
						 WPARAM wParam, // first message parameter			
						 LPARAM lParam  // second message parameter			
						 );