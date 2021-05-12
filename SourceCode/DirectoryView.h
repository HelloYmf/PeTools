#pragma once
#include <Windows.h>
#include "ExportView.h"
#include "ImportView.h"
#include "BoundImportView.h"
#include "RelocationView.h"
#include "ResourceView.h"

BOOL CALLBACK DialogDirectoryProc(									
						 HWND hwndDlg,  // handle to dialog box			
						 UINT uMsg,     // message			
						 WPARAM wParam, // first message parameter			
						 LPARAM lParam  // second message parameter			
						 );

VOID FillDirectoryData(HWND hwndDlg);