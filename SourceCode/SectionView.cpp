#include "SectionView.h"

//FileBufferָ��
extern PIMAGE_DOS_HEADER DosHeader;								//DOSͷָ��
extern PIMAGE_NT_HEADERS NtHeader;								//NTͷָ��
extern PIMAGE_FILE_HEADER FileHeader;							//��׼PEͷָ��
extern PIMAGE_OPTIONAL_HEADER OptionalHeader;					//��ѡPEͷָ�� 
extern PIMAGE_SECTION_HEADER pSectionHeader;					//�ڱ�ָ��

BOOL CALLBACK DialogSectionProc(									
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
				IintSectionListView(hwndDlg);
				//�������
				FillSectionData(hwndDlg);


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

VOID IintSectionListView(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListSection;

	//��ʼ��
	memset(&lv,0,sizeof(LV_COLUMN));
	//��ȡIDC_LIST_Section���
	hListSection = GetDlgItem(hDlg,IDC_LIST_SECTIONTABLE);
	//��������ѡ��
	SendMessage(hListSection,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//������ʽ
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	//���õ�һ��
	lv.pszText = TEXT("����");
	lv.cx = 96;
	lv.iSubItem = 0;
	SendMessage(hListSection,LVM_INSERTCOLUMN,0,(DWORD)&lv);

	//���õڶ���
	lv.pszText = TEXT("�ļ�ƫ��");
	lv.cx = 110;
	lv.iSubItem = 1;
	SendMessage(hListSection,LVM_INSERTCOLUMN,1,(DWORD)&lv);

	//���õ�����
	lv.pszText = TEXT("�ļ���С");
	lv.cx = 110;
	lv.iSubItem = 2;
	SendMessage(hListSection,LVM_INSERTCOLUMN,2,(DWORD)&lv);

	//���õ�����
	lv.pszText = TEXT("�ڴ�ƫ��");
	lv.cx = 110;
	lv.iSubItem = 3;
	SendMessage(hListSection,LVM_INSERTCOLUMN,3,(DWORD)&lv);

	//���õ�����
	lv.pszText = TEXT("�ڴ��С");
	lv.cx = 110;
	lv.iSubItem = 4;
	SendMessage(hListSection,LVM_INSERTCOLUMN,4,(DWORD)&lv);

	//���õ�����
	lv.pszText = TEXT("����");
	lv.cx = 110;
	lv.iSubItem = 5;
	SendMessage(hListSection,LVM_INSERTCOLUMN,5,(DWORD)&lv);
}

VOID FillSectionData(HWND hDlg)
{
	PIMAGE_SECTION_HEADER pTemp = pSectionHeader;
	LV_ITEM vitem;				//List��������
	HWND hListSection;			//List���
						
	//��ʼ��						
	memset(&vitem,0,sizeof(LV_ITEM));	
	//��ȡIDC_LIST_PROCESS���
	hListSection = GetDlgItem(hDlg,IDC_LIST_SECTIONTABLE);
	
	//������ʽ
	vitem.mask = LVIF_TEXT;		

	for(DWORD i = 0; i < FileHeader->NumberOfSections; i++)
	{
		wchar_t *WTempStr = (wchar_t*)malloc(sizeof(wchar_t) * 30);
		//���ýڵ�����
		mbstowcs(WTempStr,(char*)pTemp->Name,30);
		vitem.pszText = WTempStr;				
		vitem.iItem = i;						
		vitem.iSubItem = 0;						
		ListView_InsertItem(hListSection, &vitem);	

		//�����ļ���ƫ��
		wsprintf(WTempStr,TEXT("%08X"),pTemp->PointerToRawData);
		vitem.pszText = WTempStr;										
		vitem.iSubItem = 1;						
		ListView_SetItem(hListSection, &vitem);	
		//�����ļ��д�С
		wsprintf(WTempStr,TEXT("%08X"),pTemp->SizeOfRawData);
		vitem.pszText = WTempStr;										
		vitem.iSubItem = 2;						
		ListView_SetItem(hListSection, &vitem);
		//�����ڴ���ƫ��
		wsprintf(WTempStr,TEXT("%08X"),pTemp->VirtualAddress);
		vitem.pszText = WTempStr;										
		vitem.iSubItem = 3;						
		ListView_SetItem(hListSection, &vitem);
		//�����ڴ��д�С
		wsprintf(WTempStr,TEXT("%08X"),pTemp->Misc.VirtualSize);
		vitem.pszText = WTempStr;										
		vitem.iSubItem = 4;						
		ListView_SetItem(hListSection, &vitem);
		//��������
		wsprintf(WTempStr,TEXT("%08X"),pTemp->Characteristics);
		vitem.pszText = WTempStr;										
		vitem.iSubItem = 5;						
		ListView_SetItem(hListSection, &vitem);

		pTemp++;
	}
					
}