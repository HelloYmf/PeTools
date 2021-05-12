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

//��ӡ����
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

//ʮ��������->ʮ�����ƿ��ַ���
VOID DWORDToWStr(wchar_t** wstr,DWORD value,size_t size)
{
	char str[20];
	sprintf(str,"%08X" ,value);
	mbstowcs(*wstr,str,size);
}

// ʮ�������ַ���->����(int)��ֵ
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

//��ʼ����������
VOID IintProcessListView(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;

	//��ʼ��
	memset(&lv,0,sizeof(LV_COLUMN));
	//��ȡIDC_LIST_PROCESS���
	hListProcess = GetDlgItem(hDlg,IDC_LIST_PROCESS);
	//��������ѡ��
	SendMessage(hListProcess,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//������ʽ
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	//���õ�һ��
	lv.pszText = TEXT("����");
	lv.cx = 260;
	lv.iSubItem = 0;
	SendMessage(hListProcess,LVM_INSERTCOLUMN,0,(DWORD)&lv);

	//���õڶ���
	lv.pszText = TEXT("PID");
	lv.cx = 100;
	lv.iSubItem = 1;
	SendMessage(hListProcess,LVM_INSERTCOLUMN,1,(DWORD)&lv);

	//���õ�����
	lv.pszText = TEXT("�����ַ");
	lv.cx = 115;
	lv.iSubItem = 2;
	SendMessage(hListProcess,LVM_INSERTCOLUMN,2,(DWORD)&lv);

	//���õ�����
	lv.pszText = TEXT("�����С");
	lv.cx = 100;
	lv.iSubItem = 3;
	SendMessage(hListProcess,LVM_INSERTCOLUMN,3,(DWORD)&lv);
}

//��ʼ��ģ������
VOID IintMoudlesListView(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;

	//��ʼ��
	memset(&lv,0,sizeof(LV_COLUMN));
	//��ȡIDC_LIST_PROCESS���
	hListProcess = GetDlgItem(hDlg,IDC_LIST_MOUDLE);
	//��������ѡ��
	SendMessage(hListProcess,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//������ʽ
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	//���õ�һ��
	lv.pszText = TEXT("ģ������");
	lv.cx = 275;
	lv.iSubItem = 0;
	SendMessage(hListProcess,LVM_INSERTCOLUMN,0,(DWORD)&lv);

	//���õڶ���
	lv.pszText = TEXT("ģ��λ��");
	lv.cx = 320;
	lv.iSubItem = 1;
	SendMessage(hListProcess,LVM_INSERTCOLUMN,1,(DWORD)&lv);
}

//��������
VOID EnumProcess(HWND hDlg)
{
	LV_ITEM vitem;				//List��������
	HWND hListProcess;			//List���
	PROCESSENTRY32 pe32;		//������Ϣ�ṹ
	MODULEENTRY32 me32;			//ģ����Ϣ�ṹ
	HANDLE hSnapshot_proc;		//���̿��վ��
	HANDLE hSnapshot_modl;		//ģ����վ��
						
	//��ʼ��						
	memset(&vitem,0,sizeof(LV_ITEM));	
	memset(&pe32,0,sizeof(PROCESSENTRY32));
	memset(&me32,0,sizeof(MODULEENTRY32));
	//��ȡIDC_LIST_PROCESS���
	hListProcess = GetDlgItem(hDlg,IDC_LIST_PROCESS);
	
	//������ʽ
	vitem.mask = LVIF_TEXT;		

	//����ϵͳ����
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
			//���ý�������
			vitem.pszText = pe32.szExeFile;					
			vitem.iItem = i;						
			vitem.iSubItem = 0;						
			ListView_InsertItem(hListProcess, &vitem);												
			
			//���ý���PID
			wchar_t* wstrPid;
			wstrPid = (wchar_t*)malloc(sizeof(wchar_t) * 20);
			DWORDToWStr(&wstrPid,pe32.th32ProcessID,20);
			vitem.pszText = wstrPid;
			vitem.iSubItem = 1;						
			ListView_SetItem(hListProcess, &vitem);						
				
			//���þ����ַ�;����С
			hSnapshot_modl = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
			if( hSnapshot_modl != INVALID_HANDLE_VALUE )
			{
				Module32First(hSnapshot_modl, &me32);
				//�����ַ
				wchar_t* wBaseAddr;
				wBaseAddr = (wchar_t*)malloc(sizeof(wchar_t) * 20);
				DWORDToWStr(&wBaseAddr,(DWORD)me32.modBaseAddr,20);
				vitem.pszText = wBaseAddr;
				vitem.iSubItem = 2;						
				ListView_SetItem(hListProcess, &vitem);	
				//�����С
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

//����ģ��
VOID EnumMoudle(HWND hDlg)
{
	LV_ITEM vitem;				//List��������
	DWORD dwRowId;				//ѡ�е���
	TCHAR szPid[0x80] = {0};	//���PID�Ļ�����(���ַ�)
	char strPid[0x80] = {0};	//���PID�Ļ�����(խ�ַ�)
	MODULEENTRY32 me32;			//ģ����Ϣ�ṹ
	HANDLE hSnapshot_modl;		//ģ����վ��
	HWND hListProcess;			//ProcessList���
	HWND hListMoudle;			//MoudleList���

	//��ʼ��
	memset(&vitem,0,sizeof(LV_ITEM));
	memset(&me32,0,sizeof(MODULEENTRY32));

	//��ȡIDC_LIST���
	hListProcess = GetDlgItem(hDlg,IDC_LIST_PROCESS);
	hListMoudle = GetDlgItem(hDlg,IDC_LIST_MOUDLE);

	//�������������
	ListView_DeleteAllItems(hListMoudle);
	
	//��ȡѡ����
	dwRowId = SendMessage(hListProcess,LVM_GETNEXTITEM,-1,LVNI_SELECTED);
	if(dwRowId == -1)
	{
		MessageBox(NULL,TEXT("��ѡ����̣�����"),TEXT("��ʾ"),MB_OK);
		return;
	}
	//��ȡPid
	vitem.iSubItem = 1;			//Ҫ��ȡ����
	vitem.pszText = szPid;		//ָ���洢��ѯ����Ļ�����
	vitem.cchTextMax = 0x80;	//Ҫ��ȡ�ĳߴ�
	SendMessage(hListProcess,LVM_GETITEMTEXT,dwRowId,(DWORD)&vitem);
	//����ģ�����ƺ�ģ��λ��
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
			//ģ������
			vitem.iItem = 0;
			vitem.pszText = me32.szModule;						
			vitem.iSubItem = 0;						
			ListView_InsertItem(hListMoudle, &vitem);			
			//vitem.iSubItem = 1;						
			//ListView_SetItem(hListMoudle, &vitem);
			//ģ��λ��
			vitem.pszText = me32.szExePath;	
			vitem.iSubItem = 1;						
			ListView_SetItem(hListMoudle, &vitem);	
			
		}while(Module32Next(hSnapshot_modl, &me32));
	}
}

//������ص�����
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
			//����ͼ��
			HICON hIcon = LoadIcon (hAppInstance, (LPCWSTR)IDI_ICON1);		
			//����ͼ��		
			SendMessage(hwndDlg,WM_SETICON,ICON_BIG,(DWORD)hIcon);		
			SendMessage(hwndDlg,WM_SETICON,ICON_SMALL,(DWORD)hIcon);	
			//��ʼ��������
			IintProcessListView(hwndDlg);
			//��ʼ��ģ����
			IintMoudlesListView(hwndDlg);
			//��������
			EnumProcess(hwndDlg);

			return TRUE;
		}					
	case WM_COMMAND:								
		{							
			switch (LOWORD (wParam))							
			{							
				case IDC_BUTTON_LOADPE:		
					{
						//���µĶԻ���
						DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_PE),hwndDlg,DialogPEProc);

						return TRUE;
					}
				case IDC_BUTTON_SHELL:
					{
						//���µĶԻ���
						DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_SHELL),hwndDlg,DialogShellProc);

						return TRUE;
					}
				case IDC_BUTTON_ABOUT:							
					{
						MessageBox(NULL,TEXT("����"),TEXT("����"),MB_OK);																	
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
				//����ģ��
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
 			
//������
int CALLBACK WinMain(  								
	_In_  HINSTANCE hInstance, 	
	_In_  HINSTANCE hPrevInstance,  								
	_In_  LPSTR lpCmdLine,  								
	_In_  int nCmdShow  								
	)  			
{  		
	hAppInstance = hInstance;
	//����ͨ�ÿؼ�
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);

	//����������Dialog
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_MAIN),NULL,DialogMainProc);

	//��ӡ�������
	DWORD errorCode = GetLastError();
	return 0;  								
} 