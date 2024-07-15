#pragma once
#include "FindWindow.h"
#include <opencv2/opencv.hpp>

struct WindowRect
{
    int x;
    int y;
    int width;
    int height;
};

class WindowShot
{
public:
    WindowShot();
    ~WindowShot();
    double static getZoom();
    cv::Mat getWindowMat(std::string titleSection);
    uchar* getWindowUchar(std::string titleSection);
    WindowRect windowRect;
    cv::Mat getDesktopMat();

private:
    int width;
    int height;
    double zoom;
    uchar* windowUchar;
    RECT rect;
    HDC screenDC;
    HDC compatibleDC;
    HBITMAP hBitmap;
    LPVOID shotData;
    HWND hwnd;
};
