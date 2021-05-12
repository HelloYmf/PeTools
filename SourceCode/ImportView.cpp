#include "ImportView.h"
#include "PeView.h"

//FileBuffer指针
extern PBYTE FileBuffer;
extern PIMAGE_DOS_HEADER DosHeader;								//DOS头指针
extern PIMAGE_NT_HEADERS NtHeader;								//NT头指针
extern PIMAGE_FILE_HEADER FileHeader;							//标准PE头指针
extern PIMAGE_OPTIONAL_HEADER OptionalHeader;					//可选PE头指针 
extern PIMAGE_SECTION_HEADER pSectionHeader;					//节表指针
extern PIMAGE_EXPORT_DIRECTORY pExportDirectory;				//导出表指针
extern PIMAGE_IMPORT_DESCRIPTOR pImportDesriptor;				//导入表指针

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
				//初始化表头
				InitImportHeader(hwndDlg);
				//打印导出表信息
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

	//初始化
	memset(&lv,0,sizeof(LV_COLUMN));
	//获取IDC_LIST_IMPORT_UP句柄
	hListImport = GetDlgItem(hwndDlg,IDC_LIST_IMPORT_UP);
	//设置整行选中
	SendMessage(hListImport,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//设置样式
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	//设置第一列
	lv.pszText = TEXT("DLL名称");
	lv.cx = 160;
	lv.iSubItem = 0;
	SendMessage(hListImport,LVM_INSERTCOLUMN,0,(DWORD)&lv);

	//设置第二列
	lv.pszText = TEXT("OriginalFirstThunk");
	lv.cx = 100;
	lv.iSubItem = 1;
	SendMessage(hListImport,LVM_INSERTCOLUMN,1,(DWORD)&lv);

	//设置第三列
	lv.pszText = TEXT("时间戳");
	lv.cx = 100;
	lv.iSubItem = 2;
	SendMessage(hListImport,LVM_INSERTCOLUMN,2,(DWORD)&lv);

	//设置第四列
	lv.pszText = TEXT("ForwarderChain");
	lv.cx = 100;
	lv.iSubItem = 3;
	SendMessage(hListImport,LVM_INSERTCOLUMN,3,(DWORD)&lv);

	//设置第五列
	lv.pszText = TEXT("名称RVA");
	lv.cx = 100;
	lv.iSubItem = 4;
	SendMessage(hListImport,LVM_INSERTCOLUMN,4,(DWORD)&lv);

	//设置第六列
	lv.pszText = TEXT("FirstThunk");
	lv.cx = 100;
	lv.iSubItem = 5;
	SendMessage(hListImport,LVM_INSERTCOLUMN,5,(DWORD)&lv);

	//获取IDC_LIST_IMPORT_DWON句柄
	hListImport = GetDlgItem(hwndDlg,IDC_LIST_IMPORT_DOWN);

	//设置整行选中
	SendMessage(hListImport,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//设置第一列
	lv.pszText = TEXT("所属DLL");
	lv.cx = 160;
	lv.iSubItem = 0;
	SendMessage(hListImport,LVM_INSERTCOLUMN,0,(DWORD)&lv);

	//设置第二列
	lv.pszText = TEXT("Hint");
	lv.cx = 100;
	lv.iSubItem = 1;
	SendMessage(hListImport,LVM_INSERTCOLUMN,1,(DWORD)&lv);

	//设置第三列
	lv.pszText = TEXT("ApiInfo");
	lv.cx = 260;
	lv.iSubItem = 2;
	SendMessage(hListImport,LVM_INSERTCOLUMN,2,(DWORD)&lv);

	//设置第四列
	lv.pszText = TEXT("导入类型");
	lv.cx = 100;
	lv.iSubItem = 3;
	SendMessage(hListImport,LVM_INSERTCOLUMN,3,(DWORD)&lv);
}

VOID PrintImport(HWND hwndDlg)
{
	int flag = 1;	//标记所有导入表结束
	PIMAGE_IMPORT_DESCRIPTOR pTemp = pImportDesriptor;		//临时指针

	TCHAR szFileBuffer[120];
	LV_ITEM vitem;					//List的数据项
	HWND hListImport_UP;			//List句柄
	HWND hListImport_DWON;			//List句柄
	//初始化						
	memset(&vitem,0,sizeof(LV_ITEM));	
	//获取IDC_LIST_EXPORT句柄
	hListImport_UP = GetDlgItem(hwndDlg,IDC_LIST_IMPORT_UP);
	
	//设置样式
	vitem.mask = LVIF_TEXT;		

	int i = 0;
	//外层循环，遍历全部导入表
	while(flag)
	{
		flag = 0;
		//判断导入表结束
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
		//设置DLL名称
		mbstowcs(szFileBuffer,(char*)(RvaToFoa(pTemp->Name) + (DWORD)FileBuffer),20);
		vitem.pszText = szFileBuffer;					
		vitem.iItem = i;						
		vitem.iSubItem = 0;						
		ListView_InsertItem(hListImport_UP, &vitem);

		//设置OriginalFirstThunk
		wsprintf(szFileBuffer,TEXT("%08X\n"),pTemp->OriginalFirstThunk);
		vitem.pszText = szFileBuffer;
		vitem.iItem = i;
		vitem.iSubItem = 1;						
		ListView_SetItem(hListImport_UP, &vitem);

		//设置时间戳
		wsprintf(szFileBuffer,TEXT("%08X\n"),pTemp->TimeDateStamp);
		vitem.pszText = szFileBuffer;
		vitem.iItem = i;
		vitem.iSubItem = 2;						
		ListView_SetItem(hListImport_UP, &vitem);

		//设置ForwarderChain
		wsprintf(szFileBuffer,TEXT("%08X\n"),pTemp->ForwarderChain);
		vitem.pszText = szFileBuffer;
		vitem.iItem = i;
		vitem.iSubItem = 3;						
		ListView_SetItem(hListImport_UP, &vitem);

		//设置名称RVA
		wsprintf(szFileBuffer,TEXT("%08X\n"),pTemp->Name);
		vitem.pszText = szFileBuffer;
		vitem.iItem = i;
		vitem.iSubItem = 4;						
		ListView_SetItem(hListImport_UP, &vitem);

		//设置FirstThunk
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
				if(!pTemp->TimeDateStamp)	//如果时间戳是0，说明没有绑定导入表
				{
					//设置所属DLL
					mbstowcs(szFileBuffer,(char*)(RvaToFoa(pTemp->Name) + (DWORD)FileBuffer),100);
					vitem.pszText = szFileBuffer;
					vitem.iItem = k;
					vitem.iSubItem = 0;						
					ListView_InsertItem(hListImport_DWON, &vitem);
					if( ((PDWORD)(  RvaToFoa(pTemp->FirstThunk) + FileBuffer  ))[k] >> 0x1F  == 1)
					{
						//设置Hint
						wsprintf(szFileBuffer,TEXT("-"));
						vitem.pszText = szFileBuffer;
						vitem.iItem = k;
						vitem.iSubItem = 1;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//设置ApiInfo
						wsprintf(szFileBuffer,TEXT("%08X\n"),((PDWORD)(  RvaToFoa(pTemp->FirstThunk) + FileBuffer  ))[k] & 0x7FFFFFFF);
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 2;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//设置导入类型
						wsprintf(szFileBuffer,TEXT("序号"));
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 3;						
						ListView_SetItem(hListImport_DWON, &vitem);
					}
					else
					{
						//设置Hint
						wsprintf(szFileBuffer,TEXT("%08X\n"),((PIMAGE_IMPORT_BY_NAME)(RvaToFoa(((PDWORD)(  RvaToFoa(pTemp->FirstThunk) + FileBuffer  ))[k]) + FileBuffer))->Hint);
						vitem.pszText = szFileBuffer;
						vitem.iItem = k;
						vitem.iSubItem = 1;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//设置ApiInfo
						mbstowcs(szFileBuffer,(char*)(((PIMAGE_IMPORT_BY_NAME)(RvaToFoa(((PDWORD)(  RvaToFoa(pTemp->FirstThunk) + FileBuffer  ))[k]) + FileBuffer))->Name),100);
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 2;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//设置导入类型
						wsprintf(szFileBuffer,TEXT("名字"));
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 3;						
						ListView_SetItem(hListImport_DWON, &vitem);
					}
				}
				else
				{
					//设置所属DLL
					mbstowcs(szFileBuffer,(char*)(RvaToFoa(pTemp->Name) + (DWORD)FileBuffer),100);
					vitem.pszText = szFileBuffer;
					vitem.iItem = k;
					vitem.iSubItem = 0;						
					ListView_InsertItem(hListImport_DWON, &vitem);
					if( ((PDWORD)(  RvaToFoa(pTemp->OriginalFirstThunk) + FileBuffer  ))[k] >> 0x1F  == 1)
					{
						//设置Hint
						wsprintf(szFileBuffer,TEXT("-"));
						vitem.pszText = szFileBuffer;
						vitem.iItem = k;
						vitem.iSubItem = 1;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//设置ApiInfo
						wsprintf(szFileBuffer,TEXT("%08X\n"),((PDWORD)(  RvaToFoa(pTemp->OriginalFirstThunk) + FileBuffer  ))[k] & 0x7FFFFFFF);
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 2;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//设置导入类型
						wsprintf(szFileBuffer,TEXT("序号"));
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 3;						
						ListView_SetItem(hListImport_DWON, &vitem);
					}
					else
					{
						//设置Hint
						wsprintf(szFileBuffer,TEXT("%08X\n"),((PIMAGE_IMPORT_BY_NAME)(RvaToFoa(((PDWORD)(  RvaToFoa(pTemp->OriginalFirstThunk) + FileBuffer  ))[k]) + FileBuffer))->Hint);
						vitem.pszText = szFileBuffer;
						vitem.iItem = k;
						vitem.iSubItem = 1;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//设置ApiInfo
						mbstowcs(szFileBuffer,(char*)(((PIMAGE_IMPORT_BY_NAME)(RvaToFoa(((PDWORD)(  RvaToFoa(pTemp->OriginalFirstThunk) + FileBuffer  ))[k]) + FileBuffer))->Name),100);
						vitem.pszText = szFileBuffer;					
						vitem.iItem = k;						
						vitem.iSubItem = 2;						
						ListView_SetItem(hListImport_DWON, &vitem);
						//设置导入类型
						wsprintf(szFileBuffer,TEXT("名字"));
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