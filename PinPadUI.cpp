/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#include "precompiled.h"
#include <Windows.h>
#include <Shellapi.h>
#include "PinPadUI.h"
#include <Commctrl.h>
#include <TCHAR.H>

HRESULT CALLBACK TaskDialogCallbackProcPinEntry(HWND hwnd, UINT uNotification, WPARAM wParam, LPARAM lParam, LONG_PTR dwRefData) 
{

	if(uNotification == TDN_CREATED)
	{
		SendMessage(hwnd, TDM_SET_PROGRESS_BAR_STATE, 0x0003, 0);
		SendMessage( hwnd, TDM_SET_PROGRESS_BAR_POS, 100, 0L );
		SendMessage(hwnd, TDM_SET_PROGRESS_BAR_STATE, 0x0001, 0);
	}
	else if(uNotification == TDN_TIMER)
	{
		int ddd = 100 - (int)wParam / 300;
		SendMessage( hwnd, TDM_SET_PROGRESS_BAR_POS, ddd, 0L );
	}
	else if(uNotification == TDN_HYPERLINK_CLICKED)
	{
		ShellExecute( 0, L"open", (LPCTSTR) lParam, 0, 0, SW_SHOW );
	}
	else if(uNotification == TDN_BUTTON_CLICKED)
	{
		SendMessage(hwnd, WM_NCDESTROY, 0, 0);
		return S_FALSE;
	}
	return S_OK;
}


DWORD WINAPI DialogThreadEntry(LPVOID lpParam)
{
	int buttonPressed = 0;
	EXTERNAL_INFO *externalInfo = (PEXTERNAL_INFO)lpParam;
	TASKDIALOGCONFIG config = {0};

	config.cbSize = sizeof(config);
	config.hInstance = GetModuleHandle(NULL);
	config.dwCommonButtons = TDCBF_CANCEL_BUTTON;
	config.pszMainIcon = TD_INFORMATION_ICON;
	
	if(externalInfo->langId == 0x0419)
		config.pszMainInstruction = L"PIN Pad считыватель";
	else if(externalInfo->langId == 0x0425)
		//config.pszMainInstruction = L"Autentige PINPADˇiga";
		config.pszMainInstruction = L"PIN Pad kaardilugeja";
	else
		config.pszMainInstruction = L"PIN Pad Reader";

	if(externalInfo->pinType == 1)
	{
		if(externalInfo->langId == 0x0419)
			config.pszContent = L"Введите код PIN для идентификации (PIN 1)";
		else if(externalInfo->langId == 0x0425)
			config.pszContent = L"Palun sisestage autoriseerimise PIN (PIN1)";
		else
			config.pszContent = L"Enter PIN for authentication (PIN 1)";
	}
	else if(externalInfo->pinType == 3)
	{
		if(externalInfo->langId == 0x0419)
			config.pszContent = L"Введите код PIN для подписи (PIN 2)";
		else if(externalInfo->langId == 0x0425)
			config.pszContent = L"Palun sisestage digiallkirjastamise PIN (PIN2)";
		else
			config.pszContent = L"Enter PIN for digital signature (PIN 2)";
	}
	else if(externalInfo->pinType == 5)
	{
		if(externalInfo->langId == 0x0419)
			config.pszContent = L"Введите PUK код (PUK)";
		else if(externalInfo->langId == 0x0425)
			config.pszContent = L"Palun sisestage PUK kood (PUK)";
		else
			config.pszContent = L"Enter PUK code (PUK)";
	}
	else
	{
		config.pszContent = L"Enter PIN";
		if(externalInfo->langId == 0x0419)
			config.pszContent = L"Введите PIN код";
		else if(externalInfo->langId == 0x0425)
			config.pszContent = L"Sisestage PIN";
		else
			config.pszContent = L"Enter PIN code";
	}

	config.dwFlags = TDF_EXPAND_FOOTER_AREA  | TDF_SHOW_PROGRESS_BAR | TDF_CALLBACK_TIMER | TDF_ENABLE_HYPERLINKS;
	if(externalInfo->langId == 0x0419)
	{
		if(externalInfo->pinType == 1)
		{
			config.pszExpandedInformation = L"Данная операция требует сертификат идентификации. Для использования сертификата идентификации введите PIN1 с клавиатуры считывателя.";
		}
		else if(externalInfo->pinType == 3)
		{
			config.pszExpandedInformation = L"Для данной операцин необходим сертификат подписи. Для использования сертификата подписи введите PIN2 с клавиатуры считывателя.";
		}
		else if(externalInfo->pinType == 5)
		{
			config.pszExpandedInformation = L"Для данной операцин необходим PUK код. Наберите его на клавиатуре считывателя.";
		}
	}
	else if(externalInfo->langId == 0x0425)
	{
		if(externalInfo->pinType == 1)
		{
			config.pszExpandedInformation = L"Valitud tegevuse jaoks on vaja kasutada isikutuvastuse sertifikaati. Sertifikaadi kasutamiseks sisesta PIN1 kaardilugeja sõrmistikult.";
		}
		else if(externalInfo->pinType == 3)
		{
			config.pszExpandedInformation = L"Valitud tegevuse jaoks on vaja kasutada allkirjastamise sertifikaati. Sertifikaadi kasutamiseks sisesta PIN2 kaardilugeja sõrmistikult.";
		}
		else if(externalInfo->pinType == 5)
		{
			config.pszExpandedInformation = L"Valitud tegevuse jaoks PUK kood on vajalik. Sisesta PUK kood kaardilugeja sõrmistikult.";
		}
	}
	else
	{
		if(externalInfo->pinType == 1)
		{
			config.pszExpandedInformation = L"Selected action requires authentication certificate. For using authentication certificate enter PIN1 at the reader.";
		}
		else if(externalInfo->pinType == 3)
		{
			config.pszExpandedInformation = L"Selected action requires digital signature certificate. For using signature certificate enter PIN2 at the reader.";
		}
		else if(externalInfo->pinType == 5)
		{
			config.pszExpandedInformation = L"Selected action requires PUK code. Enter PUK code at the reader.";
		}
	}
	config.pfCallback = TaskDialogCallbackProcPinEntry;
	BOOL bVerification = false;
	HRESULT res = TaskDialogIndirect(&config, &buttonPressed, NULL, NULL);
	
	return 0;
}