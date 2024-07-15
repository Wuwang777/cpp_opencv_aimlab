#include "WindowShot.h"

//初始化变量
WindowShot::WindowShot() : shotData(nullptr), windowUchar(nullptr) {
    zoom = getZoom(); // 缩放率 比如1.25
}

//根据窗口标题是否包含该字符串，获得窗口截图
cv::Mat WindowShot::getWindowMat(std::string titleSection) {
    hwnd = getWindowHWND(titleSection);
    //如果窗口小化 就将其展示 
    if (IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
    }
    SetForegroundWindow(hwnd); // 将窗口置顶  
    rect = getWindowLoc(hwnd); // 窗口位置
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    windowRect.x = rect.left;
    windowRect.y = rect.top;
    windowRect.width = width;
    windowRect.height = height;

    delete[] shotData; // 释放之前分配的内存
    shotData = new char[width * height * 4];

    screenDC = GetDC(NULL); // 获取屏幕 DC
    compatibleDC = CreateCompatibleDC(screenDC); // 兼容新DC
    // 创建位图
    hBitmap = CreateCompatibleBitmap(screenDC, width, height);
    SelectObject(compatibleDC, hBitmap);
    // 得到位图的数据
    BitBlt(compatibleDC, 0, 0, width, height, screenDC, rect.left, rect.top, SRCCOPY);
    GetBitmapBits(hBitmap, width * height * 4, shotData);
    // 创建图像
    cv::Mat windowMat(height, width, CV_8UC4, shotData);

    // 释放 DC
    ReleaseDC(NULL, screenDC);
    DeleteDC(compatibleDC);
    DeleteObject(hBitmap);

    return windowMat;
}

//根据窗口标题是否包含该字符串，获得窗口截图 将截图转为uchar* 供python使用
uchar* WindowShot::getWindowUchar(std::string titleSection) {
    cv::Mat windowMat = this->getWindowMat(titleSection);
    int size = width * height * 4;

    // 释放之前分配的内存
    if (windowUchar) {
        free(windowUchar);
    }

    windowUchar = (uchar*)malloc(sizeof(uchar) * size);
    memcpy(windowUchar, windowMat.data, size);
    return windowUchar;
}

cv::Mat WindowShot::getDesktopMat() {
    width = GetSystemMetrics(SM_CXSCREEN) * zoom;
    height = GetSystemMetrics(SM_CYSCREEN) * zoom;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

    delete[] shotData; // 释放之前分配的内存
    shotData = new char[width * height * 4];

    screenDC = GetDC(NULL); // 获取屏幕 DC
    compatibleDC = CreateCompatibleDC(screenDC); // 兼容新DC
    // 创建位图
    hBitmap = CreateCompatibleBitmap(screenDC, width, height);
    SelectObject(compatibleDC, hBitmap);
    // 得到位图的数据
    BitBlt(compatibleDC, 0, 0, width, height, screenDC, rect.left, rect.top, SRCCOPY);
    GetBitmapBits(hBitmap, width * height * 4, shotData);
    // 创建图像
    cv::Mat desktopMat(height, width, CV_8UC4, shotData);

    // 释放 DC
    ReleaseDC(NULL, screenDC);
    DeleteDC(compatibleDC);
    DeleteObject(hBitmap);

    return desktopMat;
}

/* 获取屏幕缩放值 */
double WindowShot::getZoom() {
    // 获取窗口当前显示的监视器
    HWND hWnd = GetDesktopWindow();
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    // 获取监视器逻辑宽度
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(hMonitor, &monitorInfo);
    int cxLogical = (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);

    // 获取监视器物理宽度
    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &dm);
    int cxPhysical = dm.dmPelsWidth;

    return cxPhysical * 1.0 / cxLogical;
}

WindowShot::~WindowShot() {
    delete[] shotData; // 释放内存
    if (windowUchar) {
        free(windowUchar);
    }
    // 不需要在这里释放 hBitmap, compatibleDC, 和 screenDC
    // 因为它们在 getWindowMat 和 getDesktopMat 中已经被释放
}
