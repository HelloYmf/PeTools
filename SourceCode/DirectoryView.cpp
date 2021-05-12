#include "DirectoryView.h"

int index = 0;
extern HINSTANCE hAppInstance; 
//FileBuffer指针
extern PIMAGE_DOS_HEADER DosHeader;								//DOS头指针
extern PIMAGE_NT_HEADERS NtHeader;								//NT头指针
extern PIMAGE_FILE_HEADER FileHeader;							//标准PE头指针
extern PIMAGE_OPTIONAL_HEADER OptionalHeader;					//可选PE头指针 
extern PIMAGE_SECTION_HEADER pSectionHeader;					//节表指针

BOOL CALLBACK DialogDirectoryProc(									
						 HWND hwndDlg,  // handle to dialog box			
						 UINT uMsg,     // message			
						 WPARAM wParam, // first message parameter			
						 LPARAM lParam  // second message parameter			
						 )
{
	switch(uMsg)								
	{
		case WM_INITDIALOG:								
			{
				//填充目录项数据
				FillDirectoryData(hwndDlg);

				return TRUE;
			}	
		case WM_CLOSE:
			{
				EndDialog(hwndDlg, 0);
				return TRUE;
			}	
		case WM_COMMAND:
			{
				switch (LOWORD (wParam))							
				{			
					case IDC_BUTTON_EXPORT:
						{
							//打开导出表对话框
							DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_EXPORTINFO),hwndDlg,DialogPrintExportProc);
							return TRUE;
						}
					case IDC_BUTTON_IMPORT:
						{
							//打开导出表对话框
							DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_IMPORTINFO),hwndDlg,DialogPrintImportProc);
							return TRUE;
						}
					case IDC_BUTTON_BOUNDIMPORT:
						{
							//打开绑定导入表对话框
							DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_BOUNDIMPORTINFO),hwndDlg,DialogBoundImportProc);
							return TRUE;
						}
					case IDC_BUTTON_RELATION:
						{
							//打开重定位表对话框
							DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_RELOCATIONINFO),hwndDlg,DialogRelocationProc);
							return TRUE;
						}
					case IDC_BUTTON_RESOURCE:
						{
							//打开资源表对话框
							DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_RESOURCEINFO),hwndDlg,DialogResourceProc);
							return TRUE;
						}
					case IDC_BUTTON_EXIT3:
						{
							EndDialog(hwndDlg, 0);									
							return TRUE;	
						}
				}	
				return TRUE;
			}
	}
	return FALSE;
}

VOID FillDirectoryData(HWND hwndDlg)
{
	//写入数据
	TCHAR szFileBuffer[120];
	//设置导出表RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[0].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_EXPORTRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置导出表SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[0].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_EXPORTSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置导入表RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[1].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_IMPORTRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置导入表SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[1].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_IMPORTSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置资源表RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[2].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_RESOURCERVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置资源表SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[2].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_RESOURCESIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置异常RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[3].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_EXCEPTIONRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置异常SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[3].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_EXCEPTIONSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置证书RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[4].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CERTRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置证书SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[4].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CERTSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置重定位表RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[5].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_RELARVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置重定位表SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[5].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_RELASIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置调试RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[6].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_DEBUGRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置调试SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[6].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_DEBUGSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置版权RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[7].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_ARCHRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置版权SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[7].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_ARCHSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置全局指针RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[8].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_GPRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置全局指针SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[8].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_GPSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置TLSRVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[9].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_TLSRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置TLSSIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[9].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_TLSSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置载入配置RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[10].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_LOADCONRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置载入配置SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[10].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_LOADCONSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置绑定导入表RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[11].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_BOUNDIMPORTRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置绑定导入表SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[11].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_BOUNDIMPORTSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置IATRVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[12].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_IATRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置IATSIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[12].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_IATSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置延迟输入RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[13].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CLRRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置延迟输入SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[13].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CLRSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置COMRVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[14].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_COMRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置COMSIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[14].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_COMSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置保留RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[15].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_KEEPRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置保留SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[15].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_KEEPSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
}