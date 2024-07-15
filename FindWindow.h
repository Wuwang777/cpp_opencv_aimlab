#pragma once
#include <dwmapi.h>
#include <windows.h> 
#include <vector> 
#include <string>  
#include <iostream>

struct WindowData;

BOOL CALLBACK WindowEnumerationCallback(HWND hwnd, LPARAM lParam);

HWND getWindowHWND(std::string titleSection);

RECT getWindowLoc(HWND hwnd);