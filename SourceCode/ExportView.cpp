#include "ExportView.h"
#include "PeView.h"

//FileBufferָ��
extern PBYTE FileBuffer;
extern PIMAGE_DOS_HEADER DosHeader;								//DOSͷָ��
extern PIMAGE_NT_HEADERS NtHeader;								//NTͷָ��
extern PIMAGE_FILE_HEADER FileHeader;							//��׼PEͷָ��
extern PIMAGE_OPTIONAL_HEADER OptionalHeader;					//��ѡPEͷָ�� 
extern PIMAGE_SECTION_HEADER pSectionHeader;					//�ڱ�ָ��
extern PIMAGE_EXPORT_DIRECTORY pExportDirectory;				//������ָ��

BOOL CALLBACK DialogPrintExportProc(									
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
				//��ʼ����ͷ
				InitExportHeader(hwndDlg);
				//��ӡ��������Ϣ
				PrintExport(hwndDlg);
				return TRUE;
			}	
		case WM_CLOSE:
			{
				EndDialog(hwndDlg, 0);
				return TRUE;
			}	
		case WM_COMMAND:
			{
				switch(LOWORD (wParam))
				{
					case IDC_BUTTON_EXIT4:
						{
							EndDialog(hwndDlg, 0);
						}
				}
			}
	}
	return FALSE;
}


VOID InitExportHeader(HWND hwndDlg)
{
	LV_COLUMN lv;
	HWND hListExport;

	//��ʼ��
	memset(&lv,0,sizeof(LV_COLUMN));
	//��ȡIDC_LIST_EXPORT���
	hListExport = GetDlgItem(hwndDlg,IDC_LIST_EXPORT);
	//��������ѡ��
	SendMessage(hListExport,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//������ʽ
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	//���õ�һ��
	lv.pszText = TEXT("����������ַ");
	lv.cx = 160;
	lv.iSubItem = 0;
	SendMessage(hListExport,LVM_INSERTCOLUMN,0,(DWORD)&lv);

	//���õڶ���
	lv.pszText = TEXT("�����������");
	lv.cx = 160;
	lv.iSubItem = 1;
	SendMessage(hListExport,LVM_INSERTCOLUMN,1,(DWORD)&lv);

	//���õ�����
	lv.pszText = TEXT("������������");
	lv.cx = 335;
	lv.iSubItem = 2;
	SendMessage(hListExport,LVM_INSERTCOLUMN,2,(DWORD)&lv);
}

VOID PrintExport(HWND hwndDlg)
{
	if( pExportDirectory->NumberOfFunctions == 0 )
	{
		MessageBox(hwndDlg,TEXT("�޵�����"),TEXT("��ʾ"),MB_OK);
		return;
	}	
	//д������
	TCHAR szFileBuffer[120];
	//���õ�����RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[0].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_EXPORTRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//��������ֵ
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->Characteristics);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CHARACTER,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//������ʼ���
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->Base);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_START,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//��������RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->Name);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_NAMERVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���������ִ�
	mbstowcs(szFileBuffer,(char*)(RvaToFoa( (DWORD)pExportDirectory->Name ) + (DWORD)FileBuffer ),20);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_NAMESTR,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���ú�������
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->NumberOfFunctions);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_NUMBEROFFUN,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//�������ֵ�����������
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->NumberOfNames);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_NUMBEROFORD,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���õ���������ַ��RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->AddressOfFunctions);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_FUNADDRRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���õ����������Ʊ�RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->AddressOfNames);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_FUNNAMERVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���õ���������ű�RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->AddressOfNameOrdinals);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_ORDRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);

	LV_ITEM vitem;				//List��������
	HWND hListExport;			//List���
	//��ʼ��						
	memset(&vitem,0,sizeof(LV_ITEM));	
	//��ȡIDC_LIST_EXPORT���
	hListExport = GetDlgItem(hwndDlg,IDC_LIST_EXPORT);
	
	//������ʽ
	vitem.mask = LVIF_TEXT;		
	for(int i = 0; i < pExportDirectory->NumberOfFunctions; i++)
	{
		wsprintf(szFileBuffer,TEXT("%08X\n"),((PDWORD)(RvaToFoa(pExportDirectory->AddressOfFunctions) + (DWORD)FileBuffer))[i]);
		vitem.pszText = szFileBuffer;					
		vitem.iItem = i;						
		vitem.iSubItem = 0;						
		ListView_InsertItem(hListExport, &vitem);	
	}

	for(int j = 0; j < pExportDirectory->NumberOfNames; j++)
	{
		wsprintf(szFileBuffer,TEXT("%08X\n"),((PWORD)(RvaToFoa(pExportDirectory->AddressOfNameOrdinals) + (DWORD)FileBuffer))[j] + pExportDirectory->Base);
		vitem.pszText = szFileBuffer;					
		vitem.iItem = j;						
		vitem.iSubItem = 1;						
		ListView_SetItem(hListExport, &vitem);	
	}

	for(int k = 0; k < pExportDirectory->NumberOfNames; k++)
	{
		mbstowcs(szFileBuffer,(char*)( RvaToFoa(((PDWORD)(RvaToFoa(pExportDirectory->AddressOfNames) + (DWORD)FileBuffer))[k]) + (DWORD)FileBuffer ),20);
		vitem.pszText = szFileBuffer;					
		vitem.iItem = k;						
		vitem.iSubItem = 2;						
		ListView_SetItem(hListExport, &vitem);		
	}
}
