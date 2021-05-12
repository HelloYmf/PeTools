#include "ImportView.h"
#include "PeView.h"

//FileBufferָ��
extern PBYTE FileBuffer;
extern PIMAGE_DOS_HEADER DosHeader;								//DOSͷָ��
extern PIMAGE_NT_HEADERS NtHeader;								//NTͷָ��
extern PIMAGE_FILE_HEADER FileHeader;							//��׼PEͷָ��
extern PIMAGE_OPTIONAL_HEADER OptionalHeader;					//��ѡPEͷָ�� 
extern PIMAGE_SECTION_HEADER pSectionHeader;					//�ڱ�ָ��
extern PIMAGE_EXPORT_DIRECTORY pExportDirectory;				//������ָ��
extern PIMAGE_IMPORT_DESCRIPTOR pImportDesriptor;				//�����ָ��

BOOL CALLBACK DialogPrintImportProc(									
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
				InitImportHeader(hwndDlg);
				//��ӡ��������Ϣ
				PrintImport(hwndDlg);
				return TRUE;
			}	
		case WM_CLOSE:
			{
				EndDialog(hwndDlg, 0);
				return TRUE;
			}	
	}
	return FALSE;
}

VOID InitImportHeader(HWND hwndDlg)
{
	LV_COLUMN lv;
	HWND hListImport;

	//��ʼ��
	memset(&lv,0,sizeof(LV_COLUMN));
	//��ȡIDC_LIST_IMPORT_UP���
	hListImport = GetDlgItem(hwndDlg,IDC_LIST_IMPORT_UP);
	//��������ѡ��
	SendMessage(hListImport,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//������ʽ
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	//���õ�һ��
	lv.pszText = TEXT("DLL����");
	lv.cx = 160;
	lv.iSubItem = 0;
	SendMessage(hListImport,LVM_INSERTCOLUMN,0,(DWORD)&lv);

	//���õڶ���
	lv.pszText = TEXT("OriginalFirstThunk");
	lv.cx = 100;
	lv.iSubItem = 1;
	SendMessage(hListImport,LVM_INSERTCOLUMN,1,(DWORD)&lv);

	//���õ�����
	lv.pszText = TEXT("ʱ���");
	lv.cx = 100;
	lv.iSubItem = 2;
	SendMessage(hListImport,LVM_INSERTCOLUMN,2,(DWORD)&lv);

	//���õ�����
	lv.pszText = TEXT("ForwarderChain");
	lv.cx = 100;
	lv.iSubItem = 3;
	SendMessage(hListImport,LVM_INSERTCOLUMN,3,(DWORD)&lv);

	//���õ�����
	lv.pszText = TEXT("����RVA");
	lv.cx = 100;
	lv.iSubItem = 4;
	SendMessage(hListImport,LVM_INSERTCOLUMN,4,(DWORD)&lv);

	//���õ�����
	lv.pszText = TEXT("FirstThunk");
	lv.cx = 100;
	lv.iSubItem = 5;
	SendMessage(hListImport,LVM_INSERTCOLUMN,5,(DWORD)&lv);

	//��ȡIDC_LIST_IMPORT_DWON���
	hListImport = GetDlgItem(hwndDlg,IDC_LIST_IMPORT_DOWN);

	//��������ѡ��
	SendMessage(hListImport,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//���õ�һ��
	lv.pszText = TEXT("����DLL");
	lv.cx = 160;
	lv.iSubItem = 0;
	SendMessage(hListImport,LVM_INSERTCOLUMN,0,(DWORD)&lv);

	//���õڶ���
	lv.pszText = TEXT("Hint");
	lv.cx = 100;
	lv.iSubItem = 1;
	SendMessage(hListImport,LVM_INSERTCOLUMN,1,(DWORD)&lv);

	//���õ�����
	lv.pszText = TEXT("ApiInfo");
	lv.cx = 260;
	lv.iSubItem = 2;
	SendMessage(hListImport,LVM_INSERTCOLUMN,2,(DWORD)&lv);

	//���õ�����
	lv.pszText = TEXT("��������");
	lv.cx = 100;
	lv.iSubItem = 3;
	SendMessage(hListImport,LVM_INSERTCOLUMN,3,(DWORD)&lv);
}

VOID PrintImport(HWND hwndDlg)
{
	int flag = 1;	//������е�������
	PIMAGE_IMPORT_DESCRIPTOR pTemp = pImportDesriptor;		//��ʱָ��

	TCHAR szFileBuffer[120];
	LV_ITEM vitem;					//List��������
	HWND hListImport_UP;			//List���
	HWND hListImport_DWON;			//List���
	//��ʼ��						
	memset(&vitem,0,sizeof(LV_ITEM));	
	//��ȡIDC_LIST_EXPORT���
	hListImport_UP = GetDlgItem(hwndDlg,IDC_LIST_IMPORT_UP);
	
	//������ʽ
	vitem.mask = LVIF_TEXT;		

	int i = 0;
	//���ѭ��������ȫ�������
	while(flag)
	{
		flag = 0;
		//�жϵ�������
		if( pTemp->Name == NULL)
		{
			break;
		}
		for(int j = 0; j < sizeof(IMAGE_IMPORT_DESCRIPTOR); j++)
		{
			if(  ((PBYTE)pTemp)[j] != 0)
			{
				flag = 1;
				break;
			}
		}
		//����DLL����
		mbstowcs(szFileBuffer,(char*)(RvaToFoa(pTemp->Name) + (DWORD)FileBuffer),20);
		vitem.pszText = szFileBuffer;					
		vitem.iItem = i;						
		vitem.iSubItem = 0;						
		ListView_InsertItem(hListImport_UP, &vitem);

		//����OriginalFirstThunk
		wsprintf(szFileBuffer,TEXT("%08X\n"),pTemp->OriginalFirstThunk);
		vitem.pszText = szFileBuffer;
		vitem.iItem = i;
		vitem.iSubItem = 1;						
		ListView_SetItem(hListImport_UP, &vitem);

		//����ʱ���
		wsprintf(szFileBuffer,TEXT("%08X\n"),pTemp->TimeDateStamp);
		vitem.pszText = szFileBuffer;
		vitem.iItem = i;
		vitem.iSubItem = 2;						
		ListView_SetItem(hListImport_UP, &vitem);

		//����ForwarderChain
		wsprintf(szFileBuffer,TEXT("%08X\n"),pTemp->ForwarderChain);
		vitem.pszText = szFileBuffer;
		vitem.iItem = i;
		vitem.iSubItem = 3;						
		ListView_SetItem(hListImport_UP, &vitem);

		//��������RVA
		wsprintf(szFileBuffer,TEXT("%08X\n"),pTemp->Name);
		vitem.pszText = szFileBuffer;
		vitem.iItem = i;
		vitem.iSubItem = 4;						
		ListView_SetItem(hListImport_UP, &vitem);

		//����FirstThunk
		wsprintf(szFileBuffer,TEXT("%08X\n"),pTemp->FirstThunk);
		vitem.pszText = szFileBuffer;
		vitem.iItem = i;
		vitem.iSubItem = 5;						
		ListView_SetItem(hListImport_UP, &vitem);

		hListImport_DWON = GetDlgItem(hwndDlg,IDC_LIST_IMPORT_DOWN);

		for(int k = 0;;k++)
		{
			if( ((PDWORD)(  RvaToFoa(pTemp->FirstThunk) + FileBuffer    ))[k] != 0)
			{
				if(!pTemp->TimeDateStamp)	//���ʱ�����0��˵��û�а󶨵����
				{
					//��������DLL
					mbstowcs(szFileBuffer,(char*)(RvaToFoa(pTemp->Name) + (DWORD)FileBuffer),100);
					vitem.pszText = szFileBuffer;
					vitem.iItem = k;
					vitem.iSubItem = 0;						
					ListView_InsertItem(hListImport_DWON, &vitem);
					if( ((PDWORD)(  RvaToFoa(pTemp->FirstThunk) + FileBuffer  ))[k] >> 0x1F  == 1)
					{
						//����Hint
						wsprintf(szFileBuffer,TEXT("-"));
						vitem.pszText = szFileBuffer;
						vitem.iItem = k;
						vitem.iSubItem = 1;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//����ApiInfo
						wsprintf(szFileBuffer,TEXT("%08X\n"),((PDWORD)(  RvaToFoa(pTemp->FirstThunk) + FileBuffer  ))[k] & 0x7FFFFFFF);
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 2;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//���õ�������
						wsprintf(szFileBuffer,TEXT("���"));
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 3;						
						ListView_SetItem(hListImport_DWON, &vitem);
					}
					else
					{
						//����Hint
						wsprintf(szFileBuffer,TEXT("%08X\n"),((PIMAGE_IMPORT_BY_NAME)(RvaToFoa(((PDWORD)(  RvaToFoa(pTemp->FirstThunk) + FileBuffer  ))[k]) + FileBuffer))->Hint);
						vitem.pszText = szFileBuffer;
						vitem.iItem = k;
						vitem.iSubItem = 1;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//����ApiInfo
						mbstowcs(szFileBuffer,(char*)(((PIMAGE_IMPORT_BY_NAME)(RvaToFoa(((PDWORD)(  RvaToFoa(pTemp->FirstThunk) + FileBuffer  ))[k]) + FileBuffer))->Name),100);
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 2;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//���õ�������
						wsprintf(szFileBuffer,TEXT("����"));
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 3;						
						ListView_SetItem(hListImport_DWON, &vitem);
					}
				}
				else
				{
					//��������DLL
					mbstowcs(szFileBuffer,(char*)(RvaToFoa(pTemp->Name) + (DWORD)FileBuffer),100);
					vitem.pszText = szFileBuffer;
					vitem.iItem = k;
					vitem.iSubItem = 0;						
					ListView_InsertItem(hListImport_DWON, &vitem);
					if( ((PDWORD)(  RvaToFoa(pTemp->OriginalFirstThunk) + FileBuffer  ))[k] >> 0x1F  == 1)
					{
						//����Hint
						wsprintf(szFileBuffer,TEXT("-"));
						vitem.pszText = szFileBuffer;
						vitem.iItem = k;
						vitem.iSubItem = 1;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//����ApiInfo
						wsprintf(szFileBuffer,TEXT("%08X\n"),((PDWORD)(  RvaToFoa(pTemp->OriginalFirstThunk) + FileBuffer  ))[k] & 0x7FFFFFFF);
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 2;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//���õ�������
						wsprintf(szFileBuffer,TEXT("���"));
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 3;						
						ListView_SetItem(hListImport_DWON, &vitem);
					}
					else
					{
						//����Hint
						wsprintf(szFileBuffer,TEXT("%08X\n"),((PIMAGE_IMPORT_BY_NAME)(RvaToFoa(((PDWORD)(  RvaToFoa(pTemp->OriginalFirstThunk) + FileBuffer  ))[k]) + FileBuffer))->Hint);
						vitem.pszText = szFileBuffer;
						vitem.iItem = k;
						vitem.iSubItem = 1;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//����ApiInfo
						mbstowcs(szFileBuffer,(char*)(((PIMAGE_IMPORT_BY_NAME)(RvaToFoa(((PDWORD)(  RvaToFoa(pTemp->OriginalFirstThunk) + FileBuffer  ))[k]) + FileBuffer))->Name),100);
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 2;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//���õ�������
						wsprintf(szFileBuffer,TEXT("����"));
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 3;						
						ListView_SetItem(hListImport_DWON, &vitem);
					}
				}
			}
			else
			{
				break;
			}
		}
		i++;
		pTemp++;
	}
}