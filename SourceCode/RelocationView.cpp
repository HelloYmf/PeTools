#include "RelocationView.h"

extern PIMAGE_BASE_RELOCATION pBaseRelocation;
extern PIMAGE_OPTIONAL_HEADER OptionalHeader;

BOOL CALLBACK DialogRelocationProc(									
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
				PrintRelocation(hwndDlg);
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

VOID PrintRelocation(HWND hwndDlg)
{
	if( OptionalHeader->DataDirectory[5].VirtualAddress == NULL && OptionalHeader->DataDirectory[5].Size == NULL )
	{
		MessageBox(hwndDlg,TEXT("无重定位表"),TEXT("提示"),MB_OK);
		return;
	}

	int length = 0;								//字符串长度
	string str = "";							//重要字符串
	char temp[0x100] = {0};						//临时字符串
	int flag;									//导入表结束标志
	PBYTE Point = (PBYTE)pBaseRelocation;		//可以随意移动的指针
	//打印重定位表
	str += "->BaseRelocation Info \r\n";
	for(int i = 0 ;((PIMAGE_BASE_RELOCATION)Point)->SizeOfBlock !=0 && ((PIMAGE_BASE_RELOCATION)Point)->VirtualAddress != 0; i++)
	{
		sprintf(temp,"\t%d",i + 1);
		str += temp;
		str += " .Relocation Block:\r\n";
		for(   int j = 0; j < (((PIMAGE_BASE_RELOCATION)Point)->SizeOfBlock - 8)/2; j++     )
		{
			if((((PWORD)(Point + 0x8))[j] >> 0xC) == 0x3)
			{
				str += "\t\tRVA:\t";
				sprintf(temp,"0x%08X\r\n", (((PWORD)(Point + 0x8))[j] & 0xFFF) + ((PIMAGE_BASE_RELOCATION)Point)->VirtualAddress);
				str += temp;
			}
		}
		Point = Point + ((PIMAGE_BASE_RELOCATION)Point)->SizeOfBlock;
	}

	length = str.length();
	TCHAR* szBuffer = (TCHAR*)malloc(sizeof(TCHAR) * length);
	mbstowcs(szBuffer,str.c_str(),length * 2);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_RELOCATION,WM_SETTEXT,0,(DWORD)szBuffer);
}