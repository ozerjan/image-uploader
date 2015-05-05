/*

    Image Uploader -  free application for uploading images/files to the Internet

    Copyright 2007-2015 Sergey Svistunov (zenden2k@gmail.com)

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

 */
#ifndef SIZEEXCEED_H
#define SIZEEXCEED_H

#pragma once

#include "atlheaders.h"
#include "resource.h"       // main symbols
#include "Gui/Controls/myimage.h"

struct FullUploadProfile;
struct ImageConvertingParams;
class CMyEngineList;

class CSizeExceed :
	public CDialogImpl<CSizeExceed>
{
	public:
		CSizeExceed(LPCTSTR szFileName, FullUploadProfile& iss, CMyEngineList* EngineList);
		~CSizeExceed();
		enum { IDD = IDD_SIZEEXCEED };

		BEGIN_MSG_MAP(CSizeExceed)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
			COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
			COMMAND_HANDLER(IDC_FORALL, BN_CLICKED, OnBnClickedForall)
			COMMAND_HANDLER(IDC_KEEPASIS, BN_CLICKED, OnBnClickedKeepasis)
		END_MSG_MAP()
		// Handler prototypes:
		//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
		LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		LRESULT OnBnClickedKeepasis(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
		LRESULT OnBnClickedForall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
		void Translate();

	private:
		CMyImage img;
		CString m_szFileName;
		FullUploadProfile& m_UploadProfile;
		ImageConvertingParams& m_ImageSettings;
		void DisplayParams(void);
		void GetParams();
		CMyEngineList* m_EngineList;
};

#endif // SIZEEXCEED_H