/***********************************************************************************************
			created: 		2019-05-01

			author:			chensong

			purpose:		ccfg

			输赢不重要，答案对你们有什么意义才重要。

			光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


			我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
			然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
			3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
			然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
			于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
			我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
			从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
			我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
			沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
			安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/


#include <iostream>
#include "crender_browser.h"
#include <Windows.h>
#include <thread>
#include <windows.h>
#include <iostream>

bool IsWindows11OrLater() {
	DWORD dwMajorVersion = 0;
	DWORD dwMinorVersion = 0;
	DWORD dwBuildNumber = 0;

	// 使用 RtlGetSystemVersion 函数获取系统版本信息
	// 注意：这个函数不是公开的 API，但在 ntdll.dll 中存在，并且被广泛用于检测 Windows 版本
	typedef LONG(WINAPI* RtlGetSystemVersion_Proc)(PDWORD, PDWORD, PDWORD);
	RtlGetSystemVersion_Proc RtlGetSystemVersion = (RtlGetSystemVersion_Proc)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetSystemVersion");
	if (RtlGetSystemVersion != nullptr && RtlGetSystemVersion(&dwMajorVersion, &dwMinorVersion, &dwBuildNumber) == ERROR_SUCCESS) {
		// 检查版本号是否为 Windows 11 或更高版本
		if (dwMajorVersion == 10 && dwMinorVersion == 0 && dwBuildNumber >= 22000) {
			return true;
		}
	}
	return false;
}

int test_main() {
	if (IsWindows11OrLater()) {
		std::cout << "当前操作系统是 Windows 11 或更高版本。" << std::endl;
	}
	else {
		std::cout << "当前操作系统不是 Windows 11。" << std::endl;
	}
	return 0;
}



static void _fuf(void* f)
{

}


LRESULT CALLBACK  WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	printf("[%s][%d][hWnd = %p][message = %u]\n", __FUNCTION__, __LINE__, hWnd, Message);
	switch (Message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	default:
	{
		return DefWindowProc(hWnd, Message, wParam, lParam);
		//break;
	}
	}
	return 0;

}


int test_window()
{
	MSG msg;
	HWND hWnd;
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = ::GetModuleHandle(nullptr);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "WindowClass";

	// 
	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, "Window resiter failed !!!", "Error !!!", MB_ICONEXCLAMATION | MB_OK);
	}


	hWnd = CreateWindow("WindowClass", "first window", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, ::GetModuleHandle(nullptr), NULL);

	printf("[%s][%d][hWnd = %p] \n", __FUNCTION__, __LINE__, hWnd);

	::ShowWindow(hWnd, SW_SHOW);
	::UpdateWindow(hWnd);
	 
	   while (GetMessage(&msg, NULL, 0, 0))
	   {
		   TranslateMessage(&msg);
		   DispatchMessage(&msg);
	   } 




	return EXIT_SUCCESS;
}

#if 0
int main(int argc, char* argv[])
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, LPSTR lpCmdLine, int nCmdShow)
{
	
	//test_main();
	char czFileName[1024 * 4] = { 0 };
	uint32_t length = GetModuleFileName(NULL, czFileName, _countof(czFileName) - 1);
	strrchr(czFileName, '\\')[0] = 0;//ä¹Ÿæ˜¯å¾—åˆ°C:\Users\Jovan Yang\Desktop\è®ºæ–‡ä»£ç \UnAPK&Extract all API\Debug
	for (uint32_t i = 0; i < length; ++i)
	{
		if (czFileName[i] == '/')
		{
			czFileName[i] = '\\';
		}
	}
	//	debug TODO@chensong 202412-22  cef 必须是 \\目录
		//NORMAL_EX_LOG("[work_dir = %s]", czFileName);
	std::string app_work = std::string(czFileName) + "\\cbrowser_render";
	printf("app_work = %s\n", app_work.c_str());
	std::string browser_config = std::string(czFileName) + "\\browser";
	chen::browser_init(app_work.c_str(), browser_config.c_str());
	chen::browser_startup("http://map.baidu.com", 1920, 1080, 30, &_fuf, true);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	while (true)
	{
		Sleep(3);
	}
	return 0;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}
#endif // MSG msg;
