#pragma once

#include <windows.h>
#include <gdiplus.h>

Gdiplus::Image* LoadPNGFromResource(HINSTANCE hInst, int resourceID);
void LoadAllImages(HWND hWnd);
void DeleteAllImages();
