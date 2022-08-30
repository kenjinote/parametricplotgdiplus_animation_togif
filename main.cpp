#define UNICODE
#pragma comment(lib,"gdiplus")
#pragma comment(lib,"comctl32")
#include <vector>
#include <windows.h>
#include <commctrl.h>
#define GDIPVER     0x0110
#include <gdiplus.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include"GifEncoder.h"

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.GdiPlus' version='1.1.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace Gdiplus;
TCHAR szClassName[] = TEXT("Window");
static int penwidth = 4;
static int step = 100;
static double a = 3;
static double b = 5;
static double x = 0;
static double y = 0;

COLORREF hsv2rgb(double h, double s, double v)
{
	double _r, _g, _b;
	while (h >= 360)h -= 360;
	const int i = (int)(h / 60.0);
	const double f = (h / 60.0) - i;
	const double p1 = v * (1 - s);
	const double p2 = v * (1 - s*f);
	const double p3 = v * (1 - s*(1 - f));
	switch (i)
	{
	case 0: _r = v; _g = p3; _b = p1; break;
	case 1: _r = p2; _g = v; _b = p1; break;
	case 2: _r = p1; _g = v; _b = p3; break;
	case 3: _r = p1; _g = p2; _b = v; break;
	case 4: _r = p3; _g = p1; _b = v; break;
	case 5: _r = v; _g = p1; _b = p2; break;
	}
	return RGB((BYTE)(_r * 255), (BYTE)(_g * 255), (BYTE)(_b * 255));
}

void Paint(HDC hdc, int width, int height, DWORD dwTime, DWORD dwTimeTotal)
{
	if (width <= 0 || height <= 0 || dwTime > dwTimeTotal || dwTimeTotal == 0) return;
	POINT point;
	SetViewportOrgEx(hdc, width / 2, height / 2, &point);
	const double r = (min(width, height) - penwidth) / 2.0;
	double newx, newy;
	Graphics graphics(hdc);
	graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias8x8);
	x = r;
	y = 0;
	for (int i = 0; i <= dwTime; i++)
	{
		const COLORREF color = hsv2rgb(360.0 * i / dwTimeTotal, 0.9, 1.0);
		Pen p(Color(GetRValue(color), GetGValue(color), GetBValue(color)), (Gdiplus::REAL)penwidth);
		p.SetStartCap(Gdiplus::LineCapRound);
		p.SetStartCap(Gdiplus::LineCapRound);
		newx = r * cos(a * 2.0 * M_PI * (double)i / (double)dwTimeTotal);
		newy = r * sin(b * 2.0 * M_PI * (double)i / (double)dwTimeTotal);
		graphics.DrawLine(&p, (int)x, (int)y, (int)newx, (int)newy);
		x = newx;
		y = newy;
	}
	SetViewportOrgEx(hdc, point.x, point.y, 0);
}

void CreateAnimationGif(LPCTSTR lpszFilePath, int nDelay, int width, int height, int nTotalTime)
{
	CGifEncoder gifEncoder;
	gifEncoder.SetFrameSize(width, height);
	gifEncoder.SetDelayTime(nDelay);
	gifEncoder.StartEncoder(std::wstring(lpszFilePath));
	Gdiplus::Bitmap *bmp = new Gdiplus::Bitmap(width, height);
	for (int i = 0; i <= nTotalTime; i++)
	{
		Gdiplus::Graphics g(bmp);
		const HDC hdc = g.GetHDC();
		PatBlt(hdc, 0, 0, width, height, WHITENESS);
		Paint(hdc, width, height, i, nTotalTime);
		g.ReleaseHDC(hdc);
		gifEncoder.AddFrame(bmp);
	}
	delete bmp;
	gifEncoder.FinishEncoder();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit1, hEdit2, hEdit3, hEdit4;
	static HWND hUpdown1, hUpdown2, hUpdown3, hUpdown4;
	static int i;
	switch (msg)
	{
	case WM_CREATE:
		InitCommonControls();
		hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("3"), WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 0, 0, 64, 32, hWnd, (HMENU)100, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hUpdown1 = CreateUpDownControl(WS_CHILD | WS_VISIBLE | WS_BORDER | UDS_ALIGNRIGHT, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, hEdit1, 0x7FFF, 0, (int)a);
		hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("5"), WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 0, 32, 64, 32, hWnd, (HMENU)101, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hUpdown2 = CreateUpDownControl(WS_CHILD | WS_VISIBLE | WS_BORDER | UDS_ALIGNRIGHT, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, hEdit2, 0x7FFF, 0, (int)b);
		hEdit3 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("100"), WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 0, 64, 64, 32, hWnd, (HMENU)102, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hUpdown3 = CreateUpDownControl(WS_CHILD | WS_VISIBLE | WS_BORDER | UDS_ALIGNRIGHT, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, hEdit3, 0x7FFF, 0, step);
		hEdit4 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("4"), WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 0, 96, 64, 32, hWnd, (HMENU)103, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hUpdown4 = CreateUpDownControl(WS_CHILD | WS_VISIBLE | WS_BORDER | UDS_ALIGNRIGHT, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, hEdit4, 0x7FFF, 0, penwidth);
		CreateWindow(TEXT("BUTTON"), TEXT("ì¬"), WS_VISIBLE | WS_CHILD, 0, 96+32, 64, 32, hWnd, (HMENU)104, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SetTimer(hWnd, 0x1234, 10, 0);
		break;
	case WM_NOTIFY:
		if (hUpdown1 == ((LPNMHDR)lParam)->hwndFrom)
		{
			if (((LPNMUPDOWN)lParam)->hdr.code == UDN_DELTAPOS)
			{
				SetDlgItemInt(hWnd, 100, ((LPNMUPDOWN)lParam)->iPos, 0);
			}
		}
		else if (hUpdown2 == ((LPNMHDR)lParam)->hwndFrom)
		{
			if (((LPNMUPDOWN)lParam)->hdr.code == UDN_DELTAPOS)
			{
				SetDlgItemInt(hWnd, 101, ((LPNMUPDOWN)lParam)->iPos, 0);
			}
		}
		else if (hUpdown3 == ((LPNMHDR)lParam)->hwndFrom)
		{
			if (((LPNMUPDOWN)lParam)->hdr.code == UDN_DELTAPOS)
			{
				SetDlgItemInt(hWnd, 102, ((LPNMUPDOWN)lParam)->iPos, 0);
			}
		}
		else if (hUpdown4 == ((LPNMHDR)lParam)->hwndFrom)
		{
			if (((LPNMUPDOWN)lParam)->hdr.code == UDN_DELTAPOS)
			{
				SetDlgItemInt(hWnd, 103, ((LPNMUPDOWN)lParam)->iPos, 0);
			}
		}
		break;
	case WM_TIMER:
		i++;
		if (i >= step)
		{
			KillTimer(hWnd, 0x1234);
		}
		InvalidateRect(hWnd, 0, 0);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == 104)
		{
			CreateAnimationGif(TEXT("output.gif"), 100, 250, 250, step);
		}
		else if ((LOWORD(wParam) == 100 || LOWORD(wParam) == 101 || LOWORD(wParam) == 102 || LOWORD(wParam) == 103) && HIWORD(wParam) == EN_CHANGE)
		{
			KillTimer(hWnd, 0x1234);
			a = GetDlgItemInt(hWnd, 100, 0, 0);
			b = GetDlgItemInt(hWnd, 101, 0, 0);
			step = GetDlgItemInt(hWnd, 102, 0, 0);
			penwidth = GetDlgItemInt(hWnd, 103, 0, 0);
			SendMessage(hUpdown1, UDM_SETPOS, 0, (LPARAM)a);
			SendMessage(hUpdown2, UDM_SETPOS, 0, (LPARAM)b);
			SendMessage(hUpdown3, UDM_SETPOS, 0, (LPARAM)step);
			SendMessage(hUpdown4, UDM_SETPOS, 0, (LPARAM)penwidth);
			i = 0;
			RECT rect;
			GetClientRect(hWnd, &rect);
			x = (min(rect.right, rect.bottom) - penwidth) / 2.0;
			y = 0;
			InvalidateRect(hWnd, 0, 1);
			SetTimer(hWnd, 0x1234, 10, 0);
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			const HDC hdc = BeginPaint(hWnd, &ps);
			RECT rect;
			GetClientRect(hWnd, &rect);
			Paint(hdc, rect.right, rect.bottom, i, step);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 0x1234);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	ULONG_PTR gdiToken;
	GdiplusStartupInput gdiSI;
	GdiplusStartup(&gdiToken, &gdiSI, NULL);
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Window"),
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
		);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	GdiplusShutdown(gdiToken);
	return msg.wParam;
}
