#include "PeView.h"

PBYTE FileBuffer = NULL;

//FileBufferָ��
PIMAGE_DOS_HEADER DosHeader = NULL;								//DOSͷָ��
PIMAGE_NT_HEADERS NtHeader = NULL;								//NTͷָ��
PIMAGE_FILE_HEADER FileHeader = NULL;							//��׼PEͷָ��
PIMAGE_OPTIONAL_HEADER OptionalHeader = NULL;					//��ѡPEͷָ�� 
PIMAGE_SECTION_HEADER pSectionHeader = NULL;					//�ڱ�ָ��
PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;				//������ָ��
PIMAGE_IMPORT_DESCRIPTOR pImportDesriptor = NULL;				//�����ָ��
PIMAGE_BOUND_FORWARDER_REF pBoundImport = NULL;					//�󶨵����ָ��
PIMAGE_BASE_RELOCATION pBaseRelocation = NULL;					//�ض�λ��ָ��
PIMAGE_RESOURCE_DIRECTORY pResourceFirst = NULL;				//��Դ��-��ԴĿ¼ָ��-��һ��
PIMAGE_RESOURCE_DIRECTORY pResourceSecond = NULL;				//��Դ��-��ԴĿ¼ָ��-�ڶ���
PIMAGE_RESOURCE_DIRECTORY pResourceThird = NULL;				//��Դ��-��ԴĿ¼ָ��-������
PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceEntryFirst = NULL;		//��Դ��-��ԴĿ¼��ָ��-��һ��
PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceEntrySecond = NULL;	//��Դ��-��ԴĿ¼��ָ��-�ڶ���
PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceEntryThird = NULL;		//��Դ��-��ԴĿ¼��ָ��-������

extern HINSTANCE hAppInstance;
TCHAR* OpenName = NULL;

//RVA->FOA
DWORD RvaToFoa(DWORD dwRva)
{
	DWORD dwFoa = 0;

	//�ж��Ƿ���ͷ+�ڱ���
	if(dwRva <= OptionalHeader->SizeOfHeaders)
	{
		dwFoa = dwRva;
		return dwFoa;
	}
	
	//�ж����Ƿ��ڽ���
	int i;
	for(i = 0; i < FileHeader->NumberOfSections; i++)
	{
		if(dwRva >= pSectionHeader[i].VirtualAddress && dwRva <= (pSectionHeader[i].VirtualAddress + pSectionHeader[i].SizeOfRawData))
		{
			dwFoa = dwRva - pSectionHeader[i].VirtualAddress;
			return dwFoa + pSectionHeader[i].PointerToRawData;
		}
	}
}

BOOL CALLBACK DialogPEProc(									
						 HWND hwndDlg,  // handle to dialog box			
						 UINT uMsg,     // message			
						 WPARAM wParam, // first message parameter			
						 LPARAM lParam  // second message parameter			
						 )			
{
	
	switch(uMsg)								
	{	
	case	WM_COMMAND:
		{
			switch (LOWORD (wParam))							
			{						
				case IDC_BUTTON_Section:
					{
						//�򿪽ڱ�Ի���
						DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_SECTIONSTABLE),hwndDlg,DialogSectionProc);
		
						return TRUE;	
					}
				case IDC_BUTTON_DIRECTORY:
					{
						//��Ŀ¼�Ի���
						DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY),hwndDlg,DialogDirectoryProc);

						return TRUE;
					}
				case IDC_BUTTON_EXIT2:
					{
						EndDialog(hwndDlg, 0);									
						return TRUE;	
					}
			}	
			return TRUE;
		}
	case WM_INITDIALOG:								
		{
			//��ȡ���ļ�������Ϣ
			OpenName = GetOpenFileName(hwndDlg);
			//�������
			FillPEData(OpenName,hwndDlg);
			//�ͷ��ڴ�
			free(OpenName);
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

TCHAR* GetOpenFileName(HWND hwndDlg)
{
	OPENFILENAME stOpenFile;
	//���ù�������Ϣ
	TCHAR szPeFileExt[30] = TEXT("*.exe;*.dll;*.scr;*.drv;*.sys");
	//�����ļ����ֻ�����
	TCHAR* szFileName = (TCHAR*)malloc(sizeof(TCHAR) * 256);

	//��ʼ��
	memset(szFileName,0,256);
	memset(&stOpenFile,0,sizeof(OPENFILENAME));

	//���ò���
	stOpenFile.lStructSize = sizeof(OPENFILENAME);
	stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	stOpenFile.hwndOwner = hwndDlg;
	stOpenFile.lpstrFilter = szPeFileExt;
	stOpenFile.lpstrFile = szFileName;
	stOpenFile.nMaxFile = MAX_PATH;	

	//��ȡ�ļ�����·��
	GetOpenFileName(&stOpenFile);

	return szFileName;
}

//����PE�����ص�FileBuffer
size_t LoadPE(TCHAR* wPath , PBYTE* Buffer)
{
	//���ļ�
	char* Path = (char*)malloc(sizeof(char) * 256);
	wcstombs(Path,wPath,256);
	FILE* fp = fopen(Path,"rb");
	if(!fp)
	{
		//OutputDebugStringF("�ļ���ʧ��\n");
		return 0;
	}

	//��ȡ�ļ��ߴ�
	fseek(fp,0,SEEK_END);
	size_t FileSize = ftell(fp);
	fseek(fp,0,SEEK_SET);

	//�����ļ�������
	BYTE* pFileBuffer = (BYTE*)malloc(FileSize);
	if( !pFileBuffer )
	{
		//OutputDebugStringF("�ڴ�����ʧ��\n");
		fclose(fp);
		return 0;
	}
	//�ڴ濽��
	memset(pFileBuffer,0,FileSize);

	if(!fread(pFileBuffer,FileSize,1,fp))
	{
		//OutputDebugStringF("д�뻺����ʧ��\n");
		free(pFileBuffer);
		fclose(fp);
		return 0;
	}
	*Buffer = pFileBuffer;

	//�ж��Ƿ���PE�ļ�
	if(*((WORD*)pFileBuffer) != 0x5A4D)
	{
		//OutputDebugStringF("д������Ч��MZ��־\n");
		free(pFileBuffer);
		fclose(fp);
		return 0;
	}
	DosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pFileBuffer = pFileBuffer + ((PIMAGE_DOS_HEADER)pFileBuffer)->e_lfanew;	//ƫ�Ƶ�NTͷ
	if(((PIMAGE_NT_HEADERS)pFileBuffer)->Signature != 0x4550)
	{
		//OutputDebugStringF("������Ч��PE��־\n");
		free(*Buffer);
		fclose(fp);
		return 0;
	}
	NtHeader = (PIMAGE_NT_HEADERS)pFileBuffer;							//��ȡNTͷ
	pFileBuffer = pFileBuffer + 0x4;									//ƫ�Ƶ���׼PEͷ
	FileHeader = (PIMAGE_FILE_HEADER)pFileBuffer;						//��ȡ��׼PEͷ
	pFileBuffer = pFileBuffer + 0x14;									//ƫ�Ƶ���ѡPEͷ
	OptionalHeader = (PIMAGE_OPTIONAL_HEADER)pFileBuffer;				//��ȡ��ѡPEͷ
	pFileBuffer = pFileBuffer + FileHeader->SizeOfOptionalHeader;		//ƫ�Ƶ��ڱ�
	pSectionHeader = (PIMAGE_SECTION_HEADER)pFileBuffer;				//��ȡ�ڱ�
	//��ȡ������ָ��
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(RvaToFoa(OptionalHeader->DataDirectory[0].VirtualAddress) + (DWORD)(*Buffer));
	//��ȡ������ָ��
	pImportDesriptor = (PIMAGE_IMPORT_DESCRIPTOR)(RvaToFoa(OptionalHeader->DataDirectory[1].VirtualAddress) + (DWORD)FileBuffer);
	//��ȡ�󶨵����
	pBoundImport = (PIMAGE_BOUND_FORWARDER_REF)(RvaToFoa(OptionalHeader->DataDirectory[11].VirtualAddress) + (DWORD)FileBuffer);
	//��ȡ�ض�λ��ָ��
	pBaseRelocation = (PIMAGE_BASE_RELOCATION)(RvaToFoa(OptionalHeader->DataDirectory[5].VirtualAddress) + (DWORD)FileBuffer);
	//��ȡ��Դ��ָ��
	pResourceFirst = (PIMAGE_RESOURCE_DIRECTORY)(RvaToFoa(OptionalHeader->DataDirectory[2].VirtualAddress) + (DWORD)FileBuffer);
	pResourceEntryFirst = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((PBYTE)pResourceFirst + sizeof(_IMAGE_RESOURCE_DIRECTORY));

	fclose(fp);
	return FileSize;
}

VOID FillPEData(TCHAR* OpenName,HWND hwndDlg)
{
	//����PE�ļ�
	size_t FileSize = LoadPE(OpenName,&FileBuffer); 
	if( !FileSize )
	{
		return;
	}
	//д������
	TCHAR szFileBuffer[120];
	//����EntryPoint
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->AddressOfEntryPoint);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_OEP,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����ImageBase
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->ImageBase);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_IMAGEBASE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����ImageSize
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->SizeOfImage);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_IMAGESIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���ô����ַ
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->BaseOfCode);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CODEBASE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//�������ݻ�ַ
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->BaseOfData);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_DATABASE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//�����ڴ����
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->SectionAlignment);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_MEMORYALIGN,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//�����ļ�����
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->FileAlignment);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_FILEALIGN,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���ñ�־��
	wsprintf(szFileBuffer,TEXT("%04X\n"),OptionalHeader->Magic);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_SIGNBYTE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//������ϵͳ
	wsprintf(szFileBuffer,TEXT("%04X\n"),OptionalHeader->Subsystem);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CHILDSYSTEM,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���ýڵ���Ŀ
	wsprintf(szFileBuffer,TEXT("%04X\n"),FileHeader->NumberOfSections);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_SIZEOFSECTIONS,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����ʱ���
	wsprintf(szFileBuffer,TEXT("%08X\n"),FileHeader->TimeDateStamp);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_TIMESTAMP,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����PEͷ��С
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->SizeOfHeaders);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_SIZEOFHEADERS,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//��������ֵ
	wsprintf(szFileBuffer,TEXT("%04X\n"),FileHeader->Characteristics);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CHARACTERISTICVALUE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����У���
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->CheckSum);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CHECKSUMS,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//���ÿ�ѡͷ��С
	wsprintf(szFileBuffer,TEXT("%04X\n"),FileHeader->SizeOfOptionalHeader);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_SIZEOFOPTIONALHEADER,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//����Ŀ¼����Ŀ
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->NumberOfRvaAndSizes);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_SIZEOFDIRECTORIES,WM_SETTEXT,0,(DWORD)szFileBuffer);
}