#include <iostream>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include "windowshot.h"
#include "findwindow.h"

const std::string AIMLAB_WINDOW_NAME = "aimlab_tb";
const double DPI_ZOOM = 1.5; //DPI缩放

cv::Point get_center_point(const cv::Rect& rect) {
	int x = rect.x, y = rect.y, width = rect.width, height = rect.height;
	int centerx = x + width / 2, centery = y + height / 2;
	return cv::Point(centerx, centery);
}

int main() {
	WindowShot ws;
	cv::Scalar lower_cyan(82, 205, 144);  // 青色下界
	cv::Scalar upper_cyan(255, 255, 255);  // 青色上界

	cv::namedWindow("this", cv::WINDOW_FREERATIO);
	cv::moveWindow("this", 0, 0);
	cv::resizeWindow("this", cv::Size(1280/ DPI_ZOOM, 720 / DPI_ZOOM));

	while (1) {
		auto start = std::chrono::high_resolution_clock::now();
		cv::Mat src = ws.getWindowMat(AIMLAB_WINDOW_NAME);

		cv::UMat frame, hsv, binary;
		src.copyTo(frame);
		if (src.empty()) {
			break;
		}

		cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
		cv::inRange(hsv, lower_cyan, upper_cyan, binary);

		//轮廓检测
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(binary, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

		cv::UMat result = frame;
		std::vector<cv::Rect> rects;
		for (const auto& contour : contours)
			rects.push_back(cv::boundingRect(contour));

		int center_window_x = (ws.windowRect.x * 2 + ws.windowRect.width) / 2;
		int center_window_y = (ws.windowRect.y * 2 + ws.windowRect.height) / 2;

		int target_dx = INT_MAX, target_dy = INT_MAX, min_dis = INT_MAX;
		for (int i = 0; i < rects.size(); i++) {
			if (rects[i].width < 30 || rects[i].height < 30) continue;
			cv::Point center = get_center_point(rects[i]);
			cv::rectangle(result, rects[i], cv::Scalar(0, 0, 255), 1, 8);
			cv::circle(result, center, 8, cv::Scalar(0, 0, 255), -1, 8);

			int dx = center.x + ws.windowRect.x - center_window_x;
			int dy = center.y + ws.windowRect.y - center_window_y;
			dy -= 20; //窗口条误差修正

			if (dx * dx + dy * dy < min_dis) {
				min_dis = dx * dx + dy * dy;
				target_dx = dx, target_dy = dy;
			}

			std::string info = std::to_string(dx) + " " + std::to_string(dy);
			cv::putText(result, info, center, 1, 1, cv::Scalar(255, 0, 0), 2, 8);
		}

		if (min_dis <= INT_MAX / 2) {
			mouse_event(MOUSEEVENTF_MOVE, target_dx * DPI_ZOOM, target_dy * DPI_ZOOM, 0, 0);

			if (min_dis < 900) {
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			}

			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = end - start;

			std::string info = std::to_string(elapsed.count() * 1000) + "ms";
			cv::putText(result, info, cv::Point(100, 100), 1, 3, cv::Scalar(0, 0, 255), 2, 8);
			cv::imshow("this", result);
			cv::waitKey(1);
		}
		
		src.release();
		hsv.release();
		frame.release();
		binary.release();
		result.release();
	}
}