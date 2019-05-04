#include "Control.h"
#include "Class.h"

LRESULT _HandleMsg(void* self, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	account(self);

	switch (msg)
	{
	case WM_KILLFOCUS:
		SetFocus(hWnd);
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000))
		{
			this->kbd->method->OnKeyPressed(this->kbd,(Byte)(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		this->kbd->method->OnKeyReleased(this->kbd, (Byte)(wParam));
		break;
	case WM_INPUT:
	{
		UINT dwSize = 48;
		RAWINPUT raw;

		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize,
			sizeof(RAWINPUTHEADER)) != dwSize)
			OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

		if (raw.header.dwType == RIM_TYPEMOUSE)
		{
			this->mouse->method->OnMouseMoved(this->mouse, &raw.data.mouse);
		}

		break;
	}

	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// retrieve ptr to win class
	struct MessageWindow* const pWnd = (struct MessageWindow*)(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward msg to class handler
	return pWnd->method->HandleMsg(pWnd, hWnd, msg, wParam, lParam);
}
LRESULT HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		// Extract data from creation of window
		const CREATESTRUCTW* const pCreate = (CREATESTRUCTW*)(lParam);
		struct MessageWindow* const pWnd = (pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to win class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(pWnd));
		// set msgproc to to non setup handle
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)(&HandleMsgThunk));

		return pWnd->method->HandleMsg(pWnd, hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
HINSTANCE _GetInstance(void* self)
{
	return ((struct c_class*)self)->hInst;
}
bool _CreateControl(void* self, HWND parent)
{
	account(self);

	WNDCLASSEX wx = {0};
	wx.cbSize = sizeof(WNDCLASSEX);
	wx.lpfnWndProc = HandleMessageSetup;        // function which will handle messages
	wx.hInstance = _GetInstance(self);
	wx.lpszClassName = this->wndClassName;

	if (RegisterClassEx(&wx)) 
	{
		this->Window = CreateWindowExW(0,
			this->wndClassName,
			L"dummy_name",
			WS_CHILD, 0, 0, 0, 0,
			parent,
			NULL, _GetInstance(this), this);
		
		SetFocus(this->Window);

		return true;
	}
	return false;
}

constructMethodTable(
	.HandleMsg = _HandleMsg
);

Constructor(void* self, va_list *ap)
{
	struct c_class* this = self;
	assignMethodTable(this);

	this->wndClassName = L"DUMMY_CLASS";
	this->hInst = GetModuleHandle(NULL);

	_CreateControl(this,va_arg(*ap, HWND));

	// Create controls and bind mouse to the window
	this->kbd = new(Keyboard);
	this->mouse = new(Mouse);
	this->mouse->method->InitializeMouse(this->mouse, this->Window);

	return this;
}
Destructor(void* self)
{
	struct c_class* this = self;
	UnregisterClass(this->wndClassName, _GetInstance(self));
	delete(this->kbd);
	delete(this->mouse);
	return this;
}
ENDCLASSDESC