#include "ResourceView.h"

extern PIMAGE_RESOURCE_DIRECTORY pResourceFirst;				//资源表-资源目录指针-第一层
extern PIMAGE_RESOURCE_DIRECTORY pResourceSecond;				//资源表-资源目录指针-第二层
extern PIMAGE_RESOURCE_DIRECTORY pResourceThird;				//资源表-资源目录指针-第三层
extern PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceEntryFirst;		//资源表-资源目录项指针-第一层
extern PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceEntrySecond;	//资源表-资源目录项指针-第二层
extern PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceEntryThird;		//资源表-资源目录项指针-第三层

BOOL CALLBACK DialogResourceProc(									
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
				PrintResource(hwndDlg);
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

VOID PrintResource(HWND hwndDlg)
{
	int length = 0;								//字符串长度
	string str = "";							//重要字符串
	char temp[0x100] = {0};						//临时字符串

	//打印资源表
	str += "->Resource Info (ICON)\r\n";
	for(WORD i = 0; i < pResourceFirst->NumberOfIdEntries + pResourceFirst->NumberOfNamedEntries; i++)
	{
		if( pResourceEntryFirst->NameIsString == 1 )
		{
			str += "\tType->Name \r\n";
			for(DWORD ii = 0; ii < ((PIMAGE_RESOURCE_DIR_STRING_U)pResourceEntryFirst->NameOffset)->Length; ii++)
			{
				printf("%c",((PIMAGE_RESOURCE_DIR_STRING_U)pResourceEntryFirst->NameOffset)->NameString + ii);
				
			}
			printf("\n");
		}
		else
		{
			if( pResourceEntryFirst->Id == 3 )
			{
				pResourceSecond = (PIMAGE_RESOURCE_DIRECTORY)((PBYTE)pResourceFirst + pResourceEntryFirst->OffsetToDirectory);
				pResourceEntrySecond = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((PBYTE)pResourceSecond + sizeof(_IMAGE_RESOURCE_DIRECTORY));
				for(WORD j = 0; j < pResourceSecond->NumberOfIdEntries + pResourceSecond->NumberOfNamedEntries; j++)
				{
					if( pResourceEntrySecond->NameIsString == 1 )
					{
						printf("第%d个编号->名字\n",j+1);
						sprintf(temp,"\t%d",j + 1);
						str += temp;
						str += ". number -> Name\r\n";
						for(DWORD ii = 0; ii < ((PIMAGE_RESOURCE_DIR_STRING_U)pResourceEntrySecond->NameOffset)->Length; ii++)
						{
							printf("%c",((PIMAGE_RESOURCE_DIR_STRING_U)pResourceEntrySecond->NameOffset)->NameString + ii);
						}
						printf("\n");
					}
					else
					{
						str += "\tNO.";
						sprintf(temp,"%d",j + 1);
						str += temp;
						str += "->ID ->";
						sprintf(temp,"%d\r\n",pResourceEntrySecond->NameOffset);
						str += temp;
					}
					pResourceThird = (PIMAGE_RESOURCE_DIRECTORY)((PBYTE)pResourceFirst + pResourceEntrySecond->OffsetToDirectory);
					pResourceEntryThird = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((PBYTE)pResourceThird + sizeof(_IMAGE_RESOURCE_DIRECTORY));
					for(WORD k = 0; k < pResourceThird->NumberOfIdEntries + pResourceThird->NumberOfNamedEntries; k++)
					{
						if( pResourceEntryThird->NameIsString == 1 )
						{
							printf("第%d个代码页->名字\n",k+1);
							for(DWORD ii = 0; ii < ((PIMAGE_RESOURCE_DIR_STRING_U)pResourceEntryThird->NameOffset)->Length; ii++)
							{
								printf("%c",((PIMAGE_RESOURCE_DIR_STRING_U)pResourceEntryThird->NameOffset)->NameString + ii);
							}
							printf("\n");
						}
						else
						{
							//printf("第%d个代码页->ID->：%d\n",k+1,pResourceEntryThird->Id);
							str += "\t\tNO.";
							sprintf(temp,"%d",k + 1);
							str += temp;
							str += "->CodePage->ID ->";
							sprintf(temp,"%d\r\n",pResourceEntryThird->Id);
							str += temp;
							//printf("		RVA->%x\n", ((PIMAGE_DATA_DIRECTORY)((PBYTE)pResourceFirst + pResourceEntryThird->OffsetToDirectory))->VirtualAddress );
							str += "\t\t\tRVA->";
							sprintf(temp,"0x%08X\r\n", ((PIMAGE_DATA_DIRECTORY)((PBYTE)pResourceFirst + pResourceEntryThird->OffsetToDirectory))->VirtualAddress);
							str += temp;
						}
						pResourceEntryThird++;
					}
					pResourceEntrySecond++;
				}
			}
		}
		pResourceEntryFirst++;
	}

	length = str.length();
	TCHAR* szBuffer = (TCHAR*)malloc(sizeof(TCHAR) * length);
	mbstowcs(szBuffer,str.c_str(),length * 2);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_RESOURCE,WM_SETTEXT,0,(DWORD)szBuffer);
}