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
	int length = 0;													//�ַ�������
	string str = "->BoundImport Info \r\n\r\n";						//��Ҫ�ַ���
	char temp[0x100] = {0};											//��ʱ�ַ���
	int flag;														//����������־
	PBYTE pTemp = (PBYTE)pBoundImport;								//���������ƶ���ָ��
	WORD NumberOfModule = 0;										//�ӱ����

	//�ж��Ƿ���ڰ󶨵����
	if(!pImportDesriptor->TimeDateStamp)
	{
		MessageBox(hwndDlg,TEXT("�ް󶨵����"),TEXT("��ʾ"),MB_OK);
		return;
	}

	//���ѭ��������ȫ���󶨵����
	for(int i = 0; ; i++)
	{
		flag = 0;
		//�жϰ󶨵�������
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
		//��ӡ���������
		sprintf(temp,"\t%X",i + 1);
		str += temp;
		str += ".BoundImport Info :\r\n";
		
		//��ӡʱ���
		str += "\t\tTimeStamp:\t";
		sprintf(temp,"%X\r\n",((PIMAGE_BOUND_IMPORT_DESCRIPTOR)pTemp)->TimeDateStamp);
		str += temp;

		//��ӡDLL����
		str += "\t\tDLLName:\t";
		str += (char*)(((PIMAGE_BOUND_IMPORT_DESCRIPTOR)pTemp)->OffsetModuleName + (DWORD)pBoundImport);
		str += "\r\n";

		//��ӡ�ֱ����
		str += "\t\tNumberOfModuleForwarderRefs:\t";
		sprintf(temp,"%X\r\n",((PIMAGE_BOUND_IMPORT_DESCRIPTOR)pTemp)->NumberOfModuleForwarderRefs);
		str += temp;

		NumberOfModule = ((PIMAGE_BOUND_IMPORT_DESCRIPTOR)pTemp)->NumberOfModuleForwarderRefs;
		pTemp = pTemp + sizeof(_IMAGE_BOUND_IMPORT_DESCRIPTOR);	//ƫ��
		for(int j = 0; j < NumberOfModule; j++)
		{
			//��ӡ�Ӱ󶨵������Ϣ
			str += "\r\n\t\t\t";
			sprintf(temp,"%X",j + 1);
			str += temp;
			str += ".Child BoundImport Info :\r\n";

			//��ӡʱ���
			str += "\t\t\tTimeStamp:\t";
			sprintf(temp,"%X\r\n",((PIMAGE_BOUND_FORWARDER_REF)pTemp)->TimeDateStamp);
			str += temp;

			//��ӡDLL����
			str += "\t\t\tDLLName:\t";
			str += (char*)(((PIMAGE_BOUND_FORWARDER_REF)pTemp)->OffsetModuleName + (DWORD)pBoundImport);
			str += "\r\n";

			//��ӡReserved
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