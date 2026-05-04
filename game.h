#pragma once

#include <windows.h>
#include <tchar.h>
#include <gdiplus.h>

extern HINSTANCE g_hInst;
extern LPCTSTR lpszClass;
extern LPCTSTR lpszWindowName;
extern ULONG_PTR g_gdiplusToken;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
void DeleteAllImages();
