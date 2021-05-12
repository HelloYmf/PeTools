#include "ResourceView.h"

extern PIMAGE_RESOURCE_DIRECTORY pResourceFirst;				//��Դ��-��ԴĿ¼ָ��-��һ��
extern PIMAGE_RESOURCE_DIRECTORY pResourceSecond;				//��Դ��-��ԴĿ¼ָ��-�ڶ���
extern PIMAGE_RESOURCE_DIRECTORY pResourceThird;				//��Դ��-��ԴĿ¼ָ��-������
extern PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceEntryFirst;		//��Դ��-��ԴĿ¼��ָ��-��һ��
extern PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceEntrySecond;	//��Դ��-��ԴĿ¼��ָ��-�ڶ���
extern PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceEntryThird;		//��Դ��-��ԴĿ¼��ָ��-������

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
	int length = 0;								//�ַ�������
	string str = "";							//��Ҫ�ַ���
	char temp[0x100] = {0};						//��ʱ�ַ���

	//��ӡ��Դ��
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
						printf("��%d�����->����\n",j+1);
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
							printf("��%d������ҳ->����\n",k+1);
							for(DWORD ii = 0; ii < ((PIMAGE_RESOURCE_DIR_STRING_U)pResourceEntryThird->NameOffset)->Length; ii++)
							{
								printf("%c",((PIMAGE_RESOURCE_DIR_STRING_U)pResourceEntryThird->NameOffset)->NameString + ii);
							}
							printf("\n");
						}
						else
						{
							//printf("��%d������ҳ->ID->��%d\n",k+1,pResourceEntryThird->Id);
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