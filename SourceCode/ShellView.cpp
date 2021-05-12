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
						//��ȡ��Դ�ļ�����·��
						ShellName = GetOpenFileName(hwndDlg);
						//д���Ի���
						SendDlgItemMessage(hwndDlg,IDC_EDIT_SHELL,WM_SETTEXT,0,(DWORD)ShellName);

						return TRUE;	
					}
				case IDC_BUTTON_SHELLSRC:
					{
						//��ȡĿ���ļ�����·��
						ShellSrcName = GetOpenFileName(hwndDlg);
						//д���Ի���
						SendDlgItemMessage(hwndDlg,IDC_EDIT_SHELLSRC,WM_SETTEXT,0,(DWORD)ShellSrcName);

						return TRUE;
					}
				case IDC_BUTTON_SHELLSTART:
					{
						//�ѿ�Դ�ļ�װ��������

						//��ԴPE����һ����

						//��Ŀ���ļ�׷�ӽ�����������������

						//��Ŀ���ļ����ֽ��м���

						//�ӻ�����д���ļ�

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