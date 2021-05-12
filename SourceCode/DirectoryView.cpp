#include "DirectoryView.h"

int index = 0;
extern HINSTANCE hAppInstance; 
//FileBufferָ��
extern PIMAGE_DOS_HEADER DosHeader;								//DOSͷָ��
extern PIMAGE_NT_HEADERS NtHeader;								//NTͷָ��
extern PIMAGE_FILE_HEADER FileHeader;							//��׼PEͷָ��
extern PIMAGE_OPTIONAL_HEADER OptionalHeader;					//��ѡPEͷָ�� 
extern PIMAGE_SECTION_HEADER pSectionHeader;					//�ڱ�ָ��

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
				//���Ŀ¼������
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
							//�򿪵�����Ի���
							DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_EXPORTINFO),hwndDlg,DialogPrintExportProc);
							return TRUE;
						}
					case IDC_BUTTON_IMPORT:
						{
							//�򿪵�����Ի���
							DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_IMPORTINFO),hwndDlg,DialogPrintImportProc);
							return TRUE;
						}
					case IDC_BUTTON_BOUNDIMPORT:
						{
							//�򿪰󶨵����Ի���
							DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_BOUNDIMPORTINFO),hwndDlg,DialogBoundImportProc);
							return TRUE;
						}
					case IDC_BUTTON_RELATION:
						{
							//���ض�λ��Ի���
							DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_RELOCATIONINFO),hwndDlg,DialogRelocationProc);
							return TRUE;
						}
					case IDC_BUTTON_RESOURCE:
						{
							//����Դ��Ի���
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
	//д������
	TCHAR szFileBuffer[120];
	//���õ�����RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[0].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_EXPORTRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���õ�����SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[0].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_EXPORTSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���õ����RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[1].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_IMPORTRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���õ����SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[1].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_IMPORTSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//������Դ��RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[2].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_RESOURCERVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//������Դ��SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[2].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_RESOURCESIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//�����쳣RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[3].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_EXCEPTIONRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//�����쳣SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[3].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_EXCEPTIONSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����֤��RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[4].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CERTRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����֤��SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[4].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CERTSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//�����ض�λ��RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[5].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_RELARVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//�����ض�λ��SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[5].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_RELASIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���õ���RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[6].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_DEBUGRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���õ���SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[6].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_DEBUGSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���ð�ȨRVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[7].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_ARCHRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���ð�ȨSIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[7].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_ARCHSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����ȫ��ָ��RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[8].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_GPRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����ȫ��ָ��SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[8].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_GPSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����TLSRVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[9].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_TLSRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����TLSSIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[9].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_TLSSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//������������RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[10].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_LOADCONRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//������������SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[10].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_LOADCONSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���ð󶨵����RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[11].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_BOUNDIMPORTRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���ð󶨵����SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[11].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_BOUNDIMPORTSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����IATRVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[12].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_IATRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����IATSIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[12].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_IATSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//�����ӳ�����RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[13].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CLRRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//�����ӳ�����SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[13].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CLRSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����COMRVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[14].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_COMRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����COMSIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[14].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_COMSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���ñ���RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[15].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_KEEPRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���ñ���SIZE
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[15].Size);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_KEEPSIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
}