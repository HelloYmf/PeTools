#include "PeView.h"

PBYTE FileBuffer = NULL;

//FileBuffer指针
PIMAGE_DOS_HEADER DosHeader = NULL;								//DOS头指针
PIMAGE_NT_HEADERS NtHeader = NULL;								//NT头指针
PIMAGE_FILE_HEADER FileHeader = NULL;							//标准PE头指针
PIMAGE_OPTIONAL_HEADER OptionalHeader = NULL;					//可选PE头指针 
PIMAGE_SECTION_HEADER pSectionHeader = NULL;					//节表指针
PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;				//导出表指针
PIMAGE_IMPORT_DESCRIPTOR pImportDesriptor = NULL;				//导入表指针
PIMAGE_BOUND_FORWARDER_REF pBoundImport = NULL;					//绑定导入表指针
PIMAGE_BASE_RELOCATION pBaseRelocation = NULL;					//重定位表指针
PIMAGE_RESOURCE_DIRECTORY pResourceFirst = NULL;				//资源表-资源目录指针-第一层
PIMAGE_RESOURCE_DIRECTORY pResourceSecond = NULL;				//资源表-资源目录指针-第二层
PIMAGE_RESOURCE_DIRECTORY pResourceThird = NULL;				//资源表-资源目录指针-第三层
PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceEntryFirst = NULL;		//资源表-资源目录项指针-第一层
PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceEntrySecond = NULL;	//资源表-资源目录项指针-第二层
PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceEntryThird = NULL;		//资源表-资源目录项指针-第三层

extern HINSTANCE hAppInstance;
TCHAR* OpenName = NULL;

//RVA->FOA
DWORD RvaToFoa(DWORD dwRva)
{
	DWORD dwFoa = 0;

	//判断是否在头+节表中
	if(dwRva <= OptionalHeader->SizeOfHeaders)
	{
		dwFoa = dwRva;
		return dwFoa;
	}
	
	//判断在是否在节中
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
						//打开节表对话框
						DialogBox(hAppInstance,MAKEINTRESOURCE(IDD_DIALOG_SECTIONSTABLE),hwndDlg,DialogSectionProc);
		
						return TRUE;	
					}
				case IDC_BUTTON_DIRECTORY:
					{
						//打开目录对话框
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
			//获取打开文件完整信息
			OpenName = GetOpenFileName(hwndDlg);
			//填充数据
			FillPEData(OpenName,hwndDlg);
			//释放内存
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
	//设置过滤器信息
	TCHAR szPeFileExt[30] = TEXT("*.exe;*.dll;*.scr;*.drv;*.sys");
	//保存文件名字缓冲区
	TCHAR* szFileName = (TCHAR*)malloc(sizeof(TCHAR) * 256);

	//初始化
	memset(szFileName,0,256);
	memset(&stOpenFile,0,sizeof(OPENFILENAME));

	//设置参数
	stOpenFile.lStructSize = sizeof(OPENFILENAME);
	stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	stOpenFile.hwndOwner = hwndDlg;
	stOpenFile.lpstrFilter = szPeFileExt;
	stOpenFile.lpstrFile = szFileName;
	stOpenFile.nMaxFile = MAX_PATH;	

	//获取文件完整路径
	GetOpenFileName(&stOpenFile);

	return szFileName;
}

//加载PE，加载到FileBuffer
size_t LoadPE(TCHAR* wPath , PBYTE* Buffer)
{
	//打开文件
	char* Path = (char*)malloc(sizeof(char) * 256);
	wcstombs(Path,wPath,256);
	FILE* fp = fopen(Path,"rb");
	if(!fp)
	{
		//OutputDebugStringF("文件打开失败\n");
		return 0;
	}

	//获取文件尺寸
	fseek(fp,0,SEEK_END);
	size_t FileSize = ftell(fp);
	fseek(fp,0,SEEK_SET);

	//申请文件缓冲区
	BYTE* pFileBuffer = (BYTE*)malloc(FileSize);
	if( !pFileBuffer )
	{
		//OutputDebugStringF("内存申请失败\n");
		fclose(fp);
		return 0;
	}
	//内存拷贝
	memset(pFileBuffer,0,FileSize);

	if(!fread(pFileBuffer,FileSize,1,fp))
	{
		//OutputDebugStringF("写入缓冲区失败\n");
		free(pFileBuffer);
		fclose(fp);
		return 0;
	}
	*Buffer = pFileBuffer;

	//判断是否是PE文件
	if(*((WORD*)pFileBuffer) != 0x5A4D)
	{
		//OutputDebugStringF("写不是有效的MZ标志\n");
		free(pFileBuffer);
		fclose(fp);
		return 0;
	}
	DosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pFileBuffer = pFileBuffer + ((PIMAGE_DOS_HEADER)pFileBuffer)->e_lfanew;	//偏移到NT头
	if(((PIMAGE_NT_HEADERS)pFileBuffer)->Signature != 0x4550)
	{
		//OutputDebugStringF("不是有效的PE标志\n");
		free(*Buffer);
		fclose(fp);
		return 0;
	}
	NtHeader = (PIMAGE_NT_HEADERS)pFileBuffer;							//获取NT头
	pFileBuffer = pFileBuffer + 0x4;									//偏移到标准PE头
	FileHeader = (PIMAGE_FILE_HEADER)pFileBuffer;						//获取标准PE头
	pFileBuffer = pFileBuffer + 0x14;									//偏移到可选PE头
	OptionalHeader = (PIMAGE_OPTIONAL_HEADER)pFileBuffer;				//获取可选PE头
	pFileBuffer = pFileBuffer + FileHeader->SizeOfOptionalHeader;		//偏移到节表
	pSectionHeader = (PIMAGE_SECTION_HEADER)pFileBuffer;				//获取节表
	//获取导出表指针
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(RvaToFoa(OptionalHeader->DataDirectory[0].VirtualAddress) + (DWORD)(*Buffer));
	//获取导出表指针
	pImportDesriptor = (PIMAGE_IMPORT_DESCRIPTOR)(RvaToFoa(OptionalHeader->DataDirectory[1].VirtualAddress) + (DWORD)FileBuffer);
	//获取绑定导入表
	pBoundImport = (PIMAGE_BOUND_FORWARDER_REF)(RvaToFoa(OptionalHeader->DataDirectory[11].VirtualAddress) + (DWORD)FileBuffer);
	//获取重定位表指针
	pBaseRelocation = (PIMAGE_BASE_RELOCATION)(RvaToFoa(OptionalHeader->DataDirectory[5].VirtualAddress) + (DWORD)FileBuffer);
	//获取资源表指针
	pResourceFirst = (PIMAGE_RESOURCE_DIRECTORY)(RvaToFoa(OptionalHeader->DataDirectory[2].VirtualAddress) + (DWORD)FileBuffer);
	pResourceEntryFirst = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((PBYTE)pResourceFirst + sizeof(_IMAGE_RESOURCE_DIRECTORY));

	fclose(fp);
	return FileSize;
}

VOID FillPEData(TCHAR* OpenName,HWND hwndDlg)
{
	//加载PE文件
	size_t FileSize = LoadPE(OpenName,&FileBuffer); 
	if( !FileSize )
	{
		return;
	}
	//写入数据
	TCHAR szFileBuffer[120];
	//设置EntryPoint
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->AddressOfEntryPoint);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_OEP,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置ImageBase
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->ImageBase);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_IMAGEBASE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置ImageSize
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->SizeOfImage);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_IMAGESIZE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置代码基址
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->BaseOfCode);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CODEBASE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置数据基址
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->BaseOfData);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_DATABASE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置内存对齐
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->SectionAlignment);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_MEMORYALIGN,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置文件对齐
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->FileAlignment);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_FILEALIGN,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置标志字
	wsprintf(szFileBuffer,TEXT("%04X\n"),OptionalHeader->Magic);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_SIGNBYTE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置子系统
	wsprintf(szFileBuffer,TEXT("%04X\n"),OptionalHeader->Subsystem);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CHILDSYSTEM,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置节的数目
	wsprintf(szFileBuffer,TEXT("%04X\n"),FileHeader->NumberOfSections);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_SIZEOFSECTIONS,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置时间戳
	wsprintf(szFileBuffer,TEXT("%08X\n"),FileHeader->TimeDateStamp);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_TIMESTAMP,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置PE头大小
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->SizeOfHeaders);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_SIZEOFHEADERS,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置特征值
	wsprintf(szFileBuffer,TEXT("%04X\n"),FileHeader->Characteristics);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CHARACTERISTICVALUE,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置校验和
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->CheckSum);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_CHECKSUMS,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置可选头大小
	wsprintf(szFileBuffer,TEXT("%04X\n"),FileHeader->SizeOfOptionalHeader);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_SIZEOFOPTIONALHEADER,WM_SETTEXT,0,(DWORD)szFileBuffer);
	//设置目录项数目
	wsprintf(szFileBuffer,TEXT("%08X\n"),OptionalHeader->NumberOfRvaAndSizes);
	SendDlgItemMessage(hwndDlg,IDC_EDIT_SIZEOFDIRECTORIES,WM_SETTEXT,0,(DWORD)szFileBuffer);
}