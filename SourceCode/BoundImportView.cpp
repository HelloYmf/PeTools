#include "BoundImportView.h"

extern PIMAGE_IMPORT_DESCRIPTOR pImportDesriptor;
extern PIMAGE_BOUND_FORWARDER_REF pBoundImport;

BOOL CALLBACK DialogBoundImportProc(									
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
				PrintBoundImport(hwndDlg);
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

VOID PrintBoundImport(HWND hwndDlg)
{
	int length = 0;													//字符串长度
	string str = "->BoundImport Info \r\n\r\n";						//重要字符串
	char temp[0x100] = {0};											//临时字符串
	int flag;														//导入表结束标志
	PBYTE pTemp = (PBYTE)pBoundImport;								//可以随意移动的指针
	WORD NumberOfModule = 0;										//子表个数

	//判断是否存在绑定导入表
	if(!pImportDesriptor->TimeDateStamp)
	{
		MessageBox(hwndDlg,TEXT("无绑定导入表"),TEXT("提示"),MB_OK);
		return;
	}

	//外层循环，遍历全部绑定导入表
	for(int i = 0; ; i++)
	{
		flag = 0;
		//判断绑定导入表结束
		for(int k = 0; k < sizeof(_IMAGE_BOUND_IMPORT_DESCRIPTOR); k++)
		{
			if(  ((PBYTE)pTemp)[k] != 0 )
			{
				flag = 1;
				break;
			}
		}
		if( !flag )
		{
			break;
		}
		//打印导入表索引
		sprintf(temp,"\t%X",i + 1);
		str += temp;
		str += ".BoundImport Info :\r\n";
		
		//打印时间戳
		str += "\t\tTimeStamp:\t";
		sprintf(temp,"%X\r\n",((PIMAGE_BOUND_IMPORT_DESCRIPTOR)pTemp)->TimeDateStamp);
		str += temp;

		//打印DLL名字
		str += "\t\tDLLName:\t";
		str += (char*)(((PIMAGE_BOUND_IMPORT_DESCRIPTOR)pTemp)->OffsetModuleName + (DWORD)pBoundImport);
		str += "\r\n";

		//打印字表个数
		str += "\t\tNumberOfModuleForwarderRefs:\t";
		sprintf(temp,"%X\r\n",((PIMAGE_BOUND_IMPORT_DESCRIPTOR)pTemp)->NumberOfModuleForwarderRefs);
		str += temp;

		NumberOfModule = ((PIMAGE_BOUND_IMPORT_DESCRIPTOR)pTemp)->NumberOfModuleForwarderRefs;
		pTemp = pTemp + sizeof(_IMAGE_BOUND_IMPORT_DESCRIPTOR);	//偏移
		for(int j = 0; j < NumberOfModule; j++)
		{
			//打印子绑定导入表信息
			str += "\r\n\t\t\t";
			sprintf(temp,"%X",j + 1);
			str += temp;
			str += ".Child BoundImport Info :\r\n";

			//打印时间戳
			str += "\t\t\tTimeStamp:\t";
			sprintf(temp,"%X\r\n",((PIMAGE_BOUND_FORWARDER_REF)pTemp)->TimeDateStamp);
			str += temp;

			//打印DLL名字
			str += "\t\t\tDLLName:\t";
			str += (char*)(((PIMAGE_BOUND_FORWARDER_REF)pTemp)->OffsetModuleName + (DWORD)pBoundImport);
			str += "\r\n";

			//打印Reserved
			str += "\t\t\tReserved:\t";
			sprintf(temp,"%X\r\n",((PIMAGE_BOUND_FORWARDER_REF)pTemp)->Reserved);
			str += temp;

			pTemp = pTemp + sizeof(_IMAGE_BOUND_FORWARDER_REF);
		}
		str += "\r\n";
	}
	length = str.length();
	TCHAR* szBuffer = (TCHAR*)malloc(sizeof(TCHAR) * length);
	mbstowcs(szBuffer,str.c_str(),length * 2);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_BOUNDIMPORT,WM_SETTEXT,0,(DWORD)szBuffer);
}