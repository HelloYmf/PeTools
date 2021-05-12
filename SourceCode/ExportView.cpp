#include "ExportView.h"
#include "PeView.h"

//FileBuffer指针
extern PBYTE FileBuffer;
extern PIMAGE_DOS_HEADER DosHeader;								//DOS头指针
extern PIMAGE_NT_HEADERS NtHeader;								//NT头指针
extern PIMAGE_FILE_HEADER FileHeader;							//标准PE头指针
extern PIMAGE_OPTIONAL_HEADER OptionalHeader;					//可选PE头指针 
extern PIMAGE_SECTION_HEADER pSectionHeader;					//节表指针
extern PIMAGE_EXPORT_DIRECTORY pExportDirectory;				//导出表指针

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
				//初始化表头
				InitExportHeader(hwndDlg);
				//打印导出表信息
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

	//初始化
	memset(&lv,0,sizeof(LV_COLUMN));
	//获取IDC_LIST_EXPORT句柄
	hListExport = GetDlgItem(hwndDlg,IDC_LIST_EXPORT);
	//设置整行选中
	SendMessage(hListExport,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//设置样式
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	//设置第一列
	lv.pszText = TEXT("导出函数地址");
	lv.cx = 160;
	lv.iSubItem = 0;
	SendMessage(hListExport,LVM_INSERTCOLUMN,0,(DWORD)&lv);

	//设置第二列
	lv.pszText = TEXT("导出函数序号");
	lv.cx = 160;
	lv.iSubItem = 1;
	SendMessage(hListExport,LVM_INSERTCOLUMN,1,(DWORD)&lv);

	//设置第三列
	lv.pszText = TEXT("导出函数名字");
	lv.cx = 335;
	lv.iSubItem = 2;
	SendMessage(hListExport,LVM_INSERTCOLUMN,2,(DWORD)&lv);
}

VOID PrintExport(HWND hwndDlg)
{
	if( pExportDirectory->NumberOfFunctions == 0 )
	{
		MessageBox(hwndDlg,TEXT("无导出表"),TEXT("提示"),MB_OK);
		return;
	}	
	//写入数据
	TCHAR szFileBuffer[120];
	//设置导出表RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->DataDirectory[0].VirtualAddress);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_EXPORTRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置特征值
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->Characteristics);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CHARACTER,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置起始序号
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->Base);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_START,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置名称RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->Name);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_NAMERVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置名称字串
	mbstowcs(szFileBuffer,(char*)(RvaToFoa( (DWORD)pExportDirectory->Name ) + (DWORD)FileBuffer ),20);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_NAMESTR,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置函数数量
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->NumberOfFunctions);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_NUMBEROFFUN,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置名字导出函数数量
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->NumberOfNames);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_NUMBEROFORD,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置导出函数地址表RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->AddressOfFunctions);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_FUNADDRRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置导出函数名称表RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->AddressOfNames);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_FUNNAMERVA,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置导出函数序号表RVA
	wsprintf(szFileBuffer,TEXT("%08X\n"),pExportDirectory->AddressOfNameOrdinals);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_ORDRVA,WM_SETTEXT,0,(DWORD)szFileBuffer);

	LV_ITEM vitem;				//List的数据项
	HWND hListExport;			//List句柄
	//初始化						
	memset(&vitem,0,sizeof(LV_ITEM));	
	//获取IDC_LIST_EXPORT句柄
	hListExport = GetDlgItem(hwndDlg,IDC_LIST_EXPORT);
	
	//设置样式
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
