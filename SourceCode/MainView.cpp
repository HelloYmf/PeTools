#include <Windows.h>
#include "PeView.h"
#include "resource.h"
#include <CommCtrl.h>
#include <Tlhelp32.h>
#include <stdio.h>
#include "ShellView.h"
#pragma comment(lib,"comctl32.lib")

//ImageBase
HINSTANCE hAppInstance;

//打印函数
void __cdecl OutputDebugStringF(const char *format, ...)  
{  
    va_list vlArgs;  
    char    *strBuffer = (char*)GlobalAlloc(GPTR, 4096);  

    va_start(vlArgs, format);  
    _vsnprintf(strBuffer, 4096 - 1, format, vlArgs);  
    va_end(vlArgs);  
    strcat(strBuffer, "\n");  
    OutputDebugStringA(strBuffer);  
    GlobalFree(strBuffer);  
    return;  
}  

//十进制数字->十六进制宽字符串
VOID DWORDToWStr(wchar_t** wstr,DWORD value,size_t size)
{
	char str[20];
	sprintf(str,"%08X" ,value);
	mbstowcs(*wstr,str,size);
}

// 十六进制字符串->整型(int)数值
int CharToInt(char ch)
{
    if(isdigit(ch))
            return ch - 48;
    if( ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'z' )
            return -1;
    if(isalpha(ch))
            return isupper(ch) ? ch - 55 : ch - 87;
    return -1;
}
int HexToDec(char *hex)
{
    int len;
    int num = 0;
    int temp;
    int bits;
    int i; 
    len = strlen(hex);
 
    for (i=0, temp=0; i<len; i++, temp=0)
    {
        temp = CharToInt( *(hex + i) );
        bits = (len - i - 1) * 4;
        temp = temp << bits;
        num = num | temp;
    }
    return num;
}

//初始化进程列名
VOID IintProcessListView(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;

	//初始化
	memset(&lv,0,sizeof(LV_COLUMN));
	//获取IDC_LIST_PROCESS句柄
	hListProcess = GetDlgItem(hDlg,IDC_LIST_PROCESS);
	//设置整行选中
	SendMessage(hListProcess,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//设置样式
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	//设置第一列
	lv.pszText = TEXT("进程");
	lv.cx = 260;
	lv.iSubItem = 0;
	SendMessage(hListProcess,LVM_INSERTCOLUMN,0,(DWORD)&lv);

	//设置第二列
	lv.pszText = TEXT("PID");
	lv.cx = 100;
	lv.iSubItem = 1;
	SendMessage(hListProcess,LVM_INSERTCOLUMN,1,(DWORD)&lv);

	//设置第三列
	lv.pszText = TEXT("镜像基址");
	lv.cx = 115;
	lv.iSubItem = 2;
	SendMessage(hListProcess,LVM_INSERTCOLUMN,2,(DWORD)&lv);

	//设置第四列
	lv.pszText = TEXT("镜像大小");
	lv.cx = 100;
	lv.iSubItem = 3;
	SendMessage(hListProcess,LVM_INSERTCOLUMN,3,(DWORD)&lv);
}

//初始化模块列名
VOID IintMoudlesListView(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;

	//初始化
	memset(&lv,0,sizeof(LV_COLUMN));
	//获取IDC_LIST_PROCESS句柄
	hListProcess = GetDlgItem(hDlg,IDC_LIST_MOUDLE);
	//设置整行选中
	SendMessage(hListProcess,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//设置样式
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	//设置第一列
	lv.pszText = TEXT("模块名称");
	lv.cx = 275;
	lv.iSubItem = 0;
	SendMessage(hListProcess,LVM_INSERTCOLUMN,0,(DWORD)&lv);

	//设置第二列
	lv.pszText = TEXT("模块位置");
	lv.cx = 320;
	lv.iSubItem = 1;
	SendMessage(hListProcess,LVM_INSERTCOLUMN,1,(DWORD)&lv);
}

//遍历进程
VOID EnumProcess(HWND hDlg)
{
	LV_ITEM vitem;				//List的数据项
	HWND hListProcess;			//List句柄
	PROCESSENTRY32 pe32;		//进程信息结构
	MODULEENTRY32 me32;			//模块信息结构
	HANDLE hSnapshot_proc;		//进程快照句柄
	HANDLE hSnapshot_modl;		//模块快照句柄
						
	//初始化						
	memset(&vitem,0,sizeof(LV_ITEM));	
	memset(&pe32,0,sizeof(PROCESSENTRY32));
	memset(&me32,0,sizeof(MODULEENTRY32));
	//获取IDC_LIST_PROCESS句柄
	hListProcess = GetDlgItem(hDlg,IDC_LIST_PROCESS);
	
	//设置样式
	vitem.mask = LVIF_TEXT;		

	//遍历系统进程
	me32.dwSize = sizeof(me32);
    pe32.dwSize = sizeof(pe32);
    hSnapshot_proc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if ( hSnapshot_proc != INVALID_HANDLE_VALUE )
    {
        BOOL check1 = Process32First(hSnapshot_proc, &pe32);
		int i = 0;
        while ( check1 )
		{
			if(pe32.th32ProcessID == 0)
			{
				check1 = Process32Next(hSnapshot_proc, &pe32);
				continue;
			}
			//设置进程名字
			vitem.pszText = pe32.szExeFile;					
			vitem.iItem = i;						
			vitem.iSubItem = 0;						
			ListView_InsertItem(hListProcess, &vitem);												
			
			//设置进程PID
			wchar_t* wstrPid;
			wstrPid = (wchar_t*)malloc(sizeof(wchar_t) * 20);
			DWORDToWStr(&wstrPid,pe32.th32ProcessID,20);
			vitem.pszText = wstrPid;
			vitem.iSubItem = 1;						
			ListView_SetItem(hListProcess, &vitem);						
				
			//设置镜像基址和镜像大小
			hSnapshot_modl = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
			if( hSnapshot_modl != INVALID_HANDLE_VALUE )
			{
				Module32First(hSnapshot_modl, &me32);
				//镜像基址
				wchar_t* wBaseAddr;
				wBaseAddr = (wchar_t*)malloc(sizeof(wchar_t) * 20);
				DWORDToWStr(&wBaseAddr,(DWORD)me32.modBaseAddr,20);
				vitem.pszText = wBaseAddr;
				vitem.iSubItem = 2;						
				ListView_SetItem(hListProcess, &vitem);	
				//镜像大小
				wchar_t* wBaseSize;
				wBaseSize = (wchar_t*)malloc(sizeof(wchar_t) * 20);
				DWORDToWStr(&wBaseSize,me32.modBaseSize,20);
				vitem.pszText = wBaseSize;
				vitem.iSubItem = 3;						
				ListView_SetItem(hListProcess, &vitem);	
			}
			else
			{
				vitem.pszText = TEXT("0");
				vitem.iSubItem = 2;						
				ListView_SetItem(hListProcess, &vitem);	
				vitem.iSubItem = 3;						
				ListView_SetItem(hListProcess, &vitem);	
			}

			check1 = Process32Next(hSnapshot_proc, &pe32);
			i++;
        }
    }
    CloseHandle(hSnapshot_proc);
}

//遍历模块
VOID EnumMoudle(HWND hDlg)
{
	LV_ITEM vitem;				//List的数据项
	DWORD dwRowId;				//选中的行
	TCHAR szPid[0x80] = {0};	//存放PID的缓冲区(宽字符)
	char strPid[0x80] = {0};	//存放PID的缓冲区(窄字符)
	MODULEENTRY32 me32;			//模块信息结构
	HANDLE hSnapshot_modl;		//模块快照句柄
	HWND hListProcess;			//ProcessList句柄
	HWND hListMoudle;			//MoudleList句柄

	//初始化
	memset(&vitem,0,sizeof(LV_ITEM));
	memset(&me32,0,sizeof(MODULEENTRY32));

	//获取IDC_LIST句柄
	hListProcess = GetDlgItem(hDlg,IDC_LIST_PROCESS);
	hListMoudle = GetDlgItem(hDlg,IDC_LIST_MOUDLE);

	//清空所有数据项
	ListView_DeleteAllItems(hListMoudle);
	
	//获取选中行
	dwRowId = SendMessage(hListProcess,LVM_GETNEXTITEM,-1,LVNI_SELECTED);
	if(dwRowId == -1)
	{
		MessageBox(NULL,TEXT("请选择进程！！！"),TEXT("提示"),MB_OK);
		return;
	}
	//获取Pid
	vitem.iSubItem = 1;			//要提取的列
	vitem.pszText = szPid;		//指定存储查询结果的缓冲区
	vitem.cchTextMax = 0x80;	//要提取的尺寸
	SendMessage(hListProcess,LVM_GETITEMTEXT,dwRowId,(DWORD)&vitem);
	//设置模块名称和模块位置
	me32.dwSize = sizeof(MODULEENTRY32);
	wcstombs(strPid,szPid,0x80);
	hSnapshot_modl = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, HexToDec(strPid));
	if( hSnapshot_modl )
	{
		if( !Module32First(hSnapshot_modl, &me32) )
		{
			CloseHandle(hSnapshot_modl);
			return;
		}
		do {
			//模块名称
			vitem.iItem = 0;
			vitem.pszText = me32.szModule;						
			vitem.iSubItem = 0;						
			ListView_InsertItem(hListMoudle, &vitem);			
			//vitem.iSubItem = 1;						
			//ListView_SetItem(hListMoudle, &vitem);
			//模块位置
			vitem.pszText = me32.szExePath;	
			vitem.iSubItem = 1;						
			ListView_SetItem(hListMoudle, &vitem);	
			
		}while(Module32Next(hSnapshot_modl, &me32));
	}
}

//主界面回调函数
BOOL CALLBACK DialogMainProc(									
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
			//加载图标
			HICON hIcon = LoadIcon (hAppInstance, (LPCWSTR)IDI_ICON1);		
			//设置图标		
			SendMessage(hwndDlg,WM_SETICON,ICON_BIG,(DWORD)hIcon);		
			SendMessage(hwndDlg,WM_SETICON,ICON_SMALL,(DWORD)hIcon);	
			//初始化进程列
			IintProcessListView(hwndDlg);
			//初始化模块列
			IintMoudlesListView(hwndDlg);
			//遍历进程
			EnumProcess(hwndDlg);

			return TRUE;
		}					
	case WM_COMMAND:								
		{							
			switch (LOWORD (wParam))							
			{							
				case IDC_BUTTON_LOADPE:		
					{
						//打开新的对话框
						DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_PE),hwndDlg,DialogPEProc);

						return TRUE;
					}
				case IDC_BUTTON_SHELL:
					{
						//打开新的对话框
						DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_SHELL),hwndDlg,DialogShellProc);

						return TRUE;
					}
				case IDC_BUTTON_ABOUT:							
					{
						MessageBox(NULL,TEXT("关于"),TEXT("关于"),MB_OK);																	
						return TRUE;	
					}
				case IDC_BUTTON_EXIT:
					{
						EndDialog(hwndDlg, 0);									
						return TRUE;	
					}
			}	
		}
	case WM_NOTIFY:
		{
			NMHDR* pNMHDR = (NMHDR*)lParam;
			if(wParam == IDC_LIST_PROCESS && pNMHDR->code == NM_CLICK)
			{
				//遍历模块
				EnumMoudle(hwndDlg);
			}
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
 			
//主函数
int CALLBACK WinMain(  								
	_In_  HINSTANCE hInstance, 	
	_In_  HINSTANCE hPrevInstance,  								
	_In_  LPSTR lpCmdLine,  								
	_In_  int nCmdShow  								
	)  			
{  		
	hAppInstance = hInstance;
	//加载通用控件
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);

	//加载主界面Dialog
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_MAIN),NULL,DialogMainProc);

	//打印错误代码
	DWORD errorCode = GetLastError();
	return 0;  								
} 