#include <windows.h>
#include <Windowsx.h>

//enable visual styles:
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

static HWND Top, Text, Select, Quit;

LRESULT CALLBACK mainwindowMessageHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
	static HWND target = 0;
	static bool istop;
	switch (message) {
	case WM_CREATE:
		Select = CreateWindow(L"Button", L"Select", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, 0, 0, 0);
		Top = CreateWindow(L"Button", L"Top", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, 0, 0, 0);
		Text = CreateWindow(L"Edit", L"<none>", WS_CHILD | WS_VISIBLE | ES_READONLY, 0, 0, 0, 0, hwnd, 0, 0, 0);
		Quit = CreateWindow(L"Button", L"Quit", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, 0, 0, 0);
		return 0;
	case WM_SIZE:
	{
		int xs = LOWORD(lparam), ys = HIWORD(lparam);
		int ysize = ys;
		int count = 4;
		MoveWindow(Select, 0, ys - ysize, xs, ysize / count, 1);
		ysize -= ysize / count--;
		MoveWindow(Text, 0, ys - ysize, xs, ysize / count, 1);
		ysize -= ysize / count--;
		MoveWindow(Top, 0, ys - ysize, xs, ysize / count, 1);
		ysize -= ysize / count--;
		MoveWindow(Quit, 0, ys - ysize, xs, ysize / count, 1);
	}
	return 0;
	case WM_COMMAND:
		if ((HWND)lparam == Select)
			SetCapture(hwnd);
		else if ((HWND)lparam == Top) {
			istop = !istop;
			SendMessage(Top, WM_SETTEXT, 0, (LPARAM)(istop ? L"Bottom" : L"Top"));
			if (!SetWindowPos(target, istop ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE)) {
				wchar_t text[1024];
				wsprintf(text, L"error %d target %p", GetLastError(), target);
			}
		}
		else if ((HWND)lparam == Quit)
			PostQuitMessage(0);
		return 0;
	case WM_LBUTTONUP:
		if (GetCapture() == hwnd) {
			WINDOWINFO WI = { sizeof WI };
			GetWindowInfo(hwnd, &WI);
			POINT point = { GET_X_LPARAM(lparam) + WI.rcClient.left, GET_Y_LPARAM(lparam) + WI.rcClient.top };
			target = WindowFromPoint(point);
			for (HWND parent = GetParent(target); parent; target = parent, parent = GetParent(target));
			wchar_t text[1024];
			GetWindowText(target, text, sizeof text - 1);
			SendMessage(Text, WM_SETTEXT, 0, (LPARAM)text);
			istop = GetWindowLong(target, GWL_EXSTYLE) & WS_EX_TOPMOST;
			SendMessage(Top, WM_SETTEXT, 0, (LPARAM)(istop ? L"Bottom" : L"Top"));
			ReleaseCapture();
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		HBRUSH dcbrush = (HBRUSH)GetStockObject(DC_BRUSH);
		SelectObject(hdc, dcbrush);
		SetDCBrushColor(hdc, 0xFF0000);
		FillRect(hdc, &ps.rcPaint, dcbrush);
		EndPaint(hwnd, &ps);
	}
	return 0;
	}
	return DefWindowProc(hwnd, message, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	wc.cbSize = sizeof wc;
	wc.style = 0;
	wc.lpfnWndProc = mainwindowMessageHandler;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"mainwindow";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wc)) {
		MessageBox(0, L"Window registration failed.", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	HWND mainwindow = CreateWindowEx(0, wc.lpszClassName, L"WinTop", WS_OVERLAPPEDWINDOW, 0, 0, 240, 200, 0, 0, hInstance, 0);
	ShowWindow(mainwindow, SW_SHOW);
	MSG message;
	while (GetMessage(&message, 0, 0, 0) > 0) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return 0;
}
