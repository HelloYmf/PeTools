#include "SectionView.h"

//FileBuffer指针
extern PIMAGE_DOS_HEADER DosHeader;								//DOS头指针
extern PIMAGE_NT_HEADERS NtHeader;								//NT头指针
extern PIMAGE_FILE_HEADER FileHeader;							//标准PE头指针
extern PIMAGE_OPTIONAL_HEADER OptionalHeader;					//可选PE头指针 
extern PIMAGE_SECTION_HEADER pSectionHeader;					//节表指针

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
				//初始化表头
				IintSectionListView(hwndDlg);
				//填充数据
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

	//初始化
	memset(&lv,0,sizeof(LV_COLUMN));
	//获取IDC_LIST_Section句柄
	hListSection = GetDlgItem(hDlg,IDC_LIST_SECTIONTABLE);
	//设置整行选中
	SendMessage(hListSection,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//设置样式
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	//设置第一列
	lv.pszText = TEXT("名称");
	lv.cx = 96;
	lv.iSubItem = 0;
	SendMessage(hListSection,LVM_INSERTCOLUMN,0,(DWORD)&lv);

	//设置第二列
	lv.pszText = TEXT("文件偏移");
	lv.cx = 110;
	lv.iSubItem = 1;
	SendMessage(hListSection,LVM_INSERTCOLUMN,1,(DWORD)&lv);

	//设置第三列
	lv.pszText = TEXT("文件大小");
	lv.cx = 110;
	lv.iSubItem = 2;
	SendMessage(hListSection,LVM_INSERTCOLUMN,2,(DWORD)&lv);

	//设置第四列
	lv.pszText = TEXT("内存偏移");
	lv.cx = 110;
	lv.iSubItem = 3;
	SendMessage(hListSection,LVM_INSERTCOLUMN,3,(DWORD)&lv);

	//设置第五列
	lv.pszText = TEXT("内存大小");
	lv.cx = 110;
	lv.iSubItem = 4;
	SendMessage(hListSection,LVM_INSERTCOLUMN,4,(DWORD)&lv);

	//设置第六列
	lv.pszText = TEXT("属性");
	lv.cx = 110;
	lv.iSubItem = 5;
	SendMessage(hListSection,LVM_INSERTCOLUMN,5,(DWORD)&lv);
}

VOID FillSectionData(HWND hDlg)
{
	PIMAGE_SECTION_HEADER pTemp = pSectionHeader;
	LV_ITEM vitem;				//List的数据项
	HWND hListSection;			//List句柄
						
	//初始化						
	memset(&vitem,0,sizeof(LV_ITEM));	
	//获取IDC_LIST_PROCESS句柄
	hListSection = GetDlgItem(hDlg,IDC_LIST_SECTIONTABLE);
	
	//设置样式
	vitem.mask = LVIF_TEXT;		

	for(DWORD i = 0; i < FileHeader->NumberOfSections; i++)
	{
		wchar_t *WTempStr = (wchar_t*)malloc(sizeof(wchar_t) * 30);
		//设置节的名字
		mbstowcs(WTempStr,(char*)pTemp->Name,30);
		vitem.pszText = WTempStr;				
		vitem.iItem = i;						
		vitem.iSubItem = 0;						
		ListView_InsertItem(hListSection, &vitem);	

		//设置文件中偏移
		wsprintf(WTempStr,TEXT("%08X"),pTemp->PointerToRawData);
		vitem.pszText = WTempStr;										
		vitem.iSubItem = 1;						
		ListView_SetItem(hListSection, &vitem);	
		//设置文件中大小
		wsprintf(WTempStr,TEXT("%08X"),pTemp->SizeOfRawData);
		vitem.pszText = WTempStr;										
		vitem.iSubItem = 2;						
		ListView_SetItem(hListSection, &vitem);
		//设置内存中偏移
		wsprintf(WTempStr,TEXT("%08X"),pTemp->VirtualAddress);
		vitem.pszText = WTempStr;										
		vitem.iSubItem = 3;						
		ListView_SetItem(hListSection, &vitem);
		//设置内存中大小
		wsprintf(WTempStr,TEXT("%08X"),pTemp->Misc.VirtualSize);
		vitem.pszText = WTempStr;										
		vitem.iSubItem = 4;						
		ListView_SetItem(hListSection, &vitem);
		//设置属性
		wsprintf(WTempStr,TEXT("%08X"),pTemp->Characteristics);
		vitem.pszText = WTempStr;										
		vitem.iSubItem = 5;						
		ListView_SetItem(hListSection, &vitem);

		pTemp++;
	}
					
}