#include "FindWindow.h"

struct WindowData {
    HWND handle;//窗口句柄
    char title[256];//窗口标题
};

std::vector<WindowData> windowDatas;

// 声明回调函数  
BOOL CALLBACK WindowEnumerationCallback(HWND hwnd, LPARAM lParam) {
    // 通过IsWindow函数检查窗口是否有效  
    if (IsWindow(hwnd)) {
        // 通过IsWindowEnabled函数检查窗口是否启用  
        if (IsWindowEnabled(hwnd)) {
            // 通过IsWindowVisible函数检查窗口是否可见  
            if (IsWindowVisible(hwnd)) {
                // 获取窗口的文本，并打印  
                char windowText[256];
                GetWindowTextA(hwnd, windowText, sizeof(windowText));
                WindowData windowData;
                windowData.handle = hwnd;
                memcpy(windowData.title, windowText, 256);
                windowDatas.push_back(windowData);
            }
        }
    }
    // 继续枚举其他窗口  
    return TRUE;
}

//返回包含titleSection的桌面窗口句柄
HWND getWindowHWND(std::string titleSection)
{
    HWND handle = NULL;
    //每次都要清空容器
    windowDatas.clear();
    // 使用EnumWindows函数枚举所有窗口，并传递给回调函数处理  
    EnumWindows(WindowEnumerationCallback, NULL);
    //一个个找包含指定字符串的
    for (auto it = windowDatas.begin(); it != windowDatas.end(); it++)
    {
        char title[256];
        memcpy(title, it->title, 256);
        std::string windowTitle(title);
        if (windowTitle.find(titleSection) != std::string::npos)
        {
            handle = it->handle;
        }
    }
    return handle;
}


//根据窗口句柄和桌面缩放获得窗口尺寸和位置
RECT getWindowLoc(HWND hwnd)
{
    RECT frame;
    DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(RECT));
    //std::cout << "窗口位置：(" << frame.left << ", " << frame.top << ")" << std::endl;
    //std::cout << "窗口大小：(" << frame.right-frame.left << ", " << frame.bottom-frame.top << ")" << std::endl;
    return frame;
}
