#pragma once
#include <Windows.h>
#include "resource.h"
#include "SectionView.h"
#include "DirectoryView.h"
#include <stdio.h>

DWORD RvaToFoa(DWORD dwRva);

//��ȡ���ļ�·��
TCHAR* GetOpenFileName(HWND hwndDlg);

//����PE�����ص�FileBuffer
size_t LoadPE(TCHAR* wPath , PBYTE* Buffer);

//���PE��Ϣ
VOID FillPEData(TCHAR* OpenName,HWND hwndDlg);

BOOL CALLBACK DialogPEProc(									
						 HWND hwndDlg,  // handle to dialog box			
						 UINT uMsg,     // message			
						 WPARAM wParam, // first message parameter			
						 LPARAM lParam  // second message parameter			
						 );