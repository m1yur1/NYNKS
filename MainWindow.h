#pragma once

#include "resource.h"
#include "WmUser.h"
extern CAppModule _Module;


class CMainWindow:
	public CWindowImpl<CMainWindow>
{
public:
	DECLARE_WND_CLASS(TEXT("CMainWindow"))

private:
	NOTIFYICONDATA notify_icon_;
	UINT reinitialize_tasktray_message_;
	CMenu popup_menu_;

	TIMECAPS time_caps_;

	std::vector<Microsoft::WRL::ComPtr<IMMDevice> > mmdevices_;
	std::vector<Microsoft::WRL::ComPtr<IAudioClient> > audio_clients_;
	const REFERENCE_TIME kOneMilliSecond = 10 * 1000; //100[ns] * 10 * 1000
	const REFERENCE_TIME kBufferDuration = kOneMilliSecond * 100; //100[ms]

public:
	CMainWindow()
	{
	}

	~CMainWindow()
	{
	}

	BEGIN_MSG_MAP_EX(CMainWindow)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER_EX(WM_USER_TASKTRAY, OnUserTasktray)
		MESSAGE_HANDLER_EX(reinitialize_tasktray_message_, OnReInitializeTasktray)
		COMMAND_ID_HANDLER_EX(IDM_QUIT, OnUserTasktrayQuit)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT pCreate)
	{
		UNREFERENCED_PARAMETER(pCreate);

		::timeGetDevCaps(&time_caps_, sizeof (time_caps_));
		::timeBeginPeriod(time_caps_.wPeriodMin);

		popup_menu_.CreatePopupMenu();
		popup_menu_.AppendMenu(MF_STRING, IDM_QUIT, TEXT("Quit"));

		reinitialize_tasktray_message_ = ::RegisterWindowMessage(TEXT("TaskbarCreated"));
		InitializeTasktray();

		InitializeWASAPI();

		return 0;
	}

	void OnDestroy()
	{
		::timeEndPeriod(time_caps_.wPeriodMin);
		FinalizeTasktray();
		popup_menu_.DestroyMenu();
		::PostQuitMessage(0);
	}

	LRESULT OnUserTasktray(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		UNREFERENCED_PARAMETER(uMsg);
		UNREFERENCED_PARAMETER(wParam);

		if (WM_RBUTTONDOWN == lParam)
		{
			POINT point;

			GetCursorPos(&point);
			SetForegroundWindow(m_hWnd);
			popup_menu_.TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, m_hWnd);
			PostMessage(WM_NULL);
		}

		return 0;
	}

	LRESULT OnReInitializeTasktray(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		UNREFERENCED_PARAMETER(uMsg);
		UNREFERENCED_PARAMETER(wParam);
		UNREFERENCED_PARAMETER(lParam);

		InitializeTasktray();

		return 0;
	}

	void OnUserTasktrayQuit(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		UNREFERENCED_PARAMETER(uNotifyCode);
		UNREFERENCED_PARAMETER(nID);
		UNREFERENCED_PARAMETER(wndCtl);

		DestroyWindow();
	}

private:
	bool InitializeTasktray()
	{
		BOOL result;

		::ZeroMemory(&notify_icon_, sizeof (notify_icon_));
		notify_icon_.cbSize = sizeof (notify_icon_);
		notify_icon_.hIcon = ::LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_NYNKS));
		notify_icon_.hWnd = m_hWnd;
		::lstrcpyn(notify_icon_.szTip, TEXT("NYNKS"), sizeof(notify_icon_.szTip));
		notify_icon_.uCallbackMessage = WM_USER_TASKTRAY;
		notify_icon_.uFlags = NIF_ICON| NIF_MESSAGE| NIF_TIP;
		notify_icon_.uID = 0;
		result = ::Shell_NotifyIcon(NIM_ADD, &notify_icon_);

		return true;
	}

	bool FinalizeTasktray()
	{
		BOOL result;

		result = ::Shell_NotifyIcon(NIM_DELETE, &notify_icon_);

		return true;
	}

	bool InitializeWASAPI()
	{
		HRESULT result;
		Microsoft::WRL::ComPtr<IMMDeviceEnumerator> device_enumerator;
		Microsoft::WRL::ComPtr<IMMDeviceCollection> device_collection;
		UINT devices = 0;

		result = ::CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (void **)(&device_enumerator));
		result = device_enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &device_collection);
		result = device_collection->GetCount(&devices);
		mmdevices_.resize(devices);
		audio_clients_.resize(devices);

		for (UINT i = 0; i < devices; ++i)
		{
			WAVEFORMATEX *format_ptr = nullptr;

			result = device_collection->Item(i, &mmdevices_[i]);
			result = mmdevices_[i]->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, &audio_clients_[i]);
			result = audio_clients_[i]->GetMixFormat(&format_ptr);
			result = audio_clients_[i]->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, kBufferDuration, 0, format_ptr, NULL);
			::CoTaskMemFree(format_ptr);
			result = audio_clients_[i]->Start();
		}

		return true;
	}
};
