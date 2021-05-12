#include "ShellView.h"

TCHAR* ShellName = NULL;
TCHAR* ShellSrcName = NULL;

BOOL CALLBACK DialogShellProc(									
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message			
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
	)			
{

	switch(uMsg)								
	{	
	case	WM_COMMAND:
		{
			switch (LOWORD (wParam))							
			{						
				case IDC_BUTTON_SHELL:
					{
						//获取壳源文件完整路径
						ShellName = GetOpenFileName(hwndDlg);
						//写进对话框
						SendDlgItemMessage(hwndDlg,IDC_EDIT_SHELL,WM_SETTEXT,0,(DWORD)ShellName);

						return TRUE;	
					}
				case IDC_BUTTON_SHELLSRC:
					{
						//获取目标文件完整路径
						ShellSrcName = GetOpenFileName(hwndDlg);
						//写进对话框
						SendDlgItemMessage(hwndDlg,IDC_EDIT_SHELLSRC,WM_SETTEXT,0,(DWORD)ShellSrcName);

						return TRUE;
					}
				case IDC_BUTTON_SHELLSTART:
					{
						//把壳源文件装进缓冲区

						//壳源PE新增一个节

						//把目标文件追加进缓冲区中新增节中

						//对目标文件部分进行加密

						//从缓冲区写回文件

						return TRUE;
					}
				return TRUE;
			}
		}
	case WM_INITDIALOG:								
		{
			return TRUE;
		}	
	case WM_CLOSE:
		{
			EndDialog(hwndDlg, 0);
			return TRUE;
		}
	}																
	return FALSE ;								
}	