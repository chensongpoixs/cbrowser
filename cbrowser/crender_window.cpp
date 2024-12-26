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
#include "crender_window.h"
 
#include "Windows.h"
#include <windowsx.h>
#include <Windows.h>
#include <Windows.h>
#include "crender_browser.h"
#include "cutil_win.h"
#include "cbrowser-source.hpp"
#include <shellscalingapi.h>
namespace chen {
	//cmediasoup_mgr g_rtc_mgr;;
	set_gpu_addresses_callback     g_gpu_addresses_callback_ptr = NULL;;
	crender_window * render_window_ptr = NULL;
	const char kD3DClassName[] = "browser_render";



	// Returns true if the process is per monitor DPI aware.
	bool IsProcessPerMonitorDpiAware() {
		enum class PerMonitorDpiAware {
			UNKNOWN = 0,
			PER_MONITOR_DPI_UNAWARE,
			PER_MONITOR_DPI_AWARE,
		};
		static PerMonitorDpiAware per_monitor_dpi_aware = PerMonitorDpiAware::UNKNOWN;
		if (per_monitor_dpi_aware == PerMonitorDpiAware::UNKNOWN) {
			per_monitor_dpi_aware = PerMonitorDpiAware::PER_MONITOR_DPI_UNAWARE;
			HMODULE shcore_dll = ::LoadLibrary("shcore.dll");
			if (shcore_dll) {
				typedef HRESULT(WINAPI* GetProcessDpiAwarenessPtr)(
					HANDLE, PROCESS_DPI_AWARENESS*);
				GetProcessDpiAwarenessPtr func_ptr =
					reinterpret_cast<GetProcessDpiAwarenessPtr>(
						::GetProcAddress(shcore_dll, "GetProcessDpiAwareness"));
				if (func_ptr) {
					PROCESS_DPI_AWARENESS awareness;
					if (SUCCEEDED(func_ptr(nullptr, &awareness)) &&
						awareness == PROCESS_PER_MONITOR_DPI_AWARE) {
						per_monitor_dpi_aware = PerMonitorDpiAware::PER_MONITOR_DPI_AWARE;
					}
				}
			}
		}
		return per_monitor_dpi_aware == PerMonitorDpiAware::PER_MONITOR_DPI_AWARE;
	}
	// DPI value for 1x scale factor.
#define DPI_1X 96.0f

	float GetWindowScaleFactor(HWND hwnd) {
		if (hwnd && IsProcessPerMonitorDpiAware()) {
			typedef UINT(WINAPI* GetDpiForWindowPtr)(HWND);
			static GetDpiForWindowPtr func_ptr = reinterpret_cast<GetDpiForWindowPtr>(
				GetProcAddress(GetModuleHandle("user32.dll"), "GetDpiForWindow"));
			if (func_ptr) {
				return static_cast<float>(func_ptr(hwnd)) / DPI_1X;
			}
		}

		return GetDeviceScaleFactor();
	}

	BrowserLayer::BrowserLayer(const std::shared_ptr<d3d11::Device>& device)
		: d3d11::Layer(device, false /* flip */) {
		frame_buffer_ = std::make_shared<d3d11::FrameBuffer>(device_);
	}

	void BrowserLayer::render(const std::shared_ptr<d3d11::Context>& ctx) {
		// Use the base class method to draw our texture.
		render_texture(ctx, frame_buffer_->texture());
	}

	void BrowserLayer::on_paint(void* share_handle) {
		if (!frame_buffer_)
		{
			return;
		}
		frame_buffer_->on_paint(share_handle);
	}

	void BrowserLayer::copy_shared_textrue()
	{
		if (frame_buffer_)
		{
			frame_buffer_->copy_shared_texture();
		}
	}

	std::pair<uint32_t, uint32_t> BrowserLayer::texture_size() const {
		const auto texture = frame_buffer_->texture();
		return std::make_pair(texture->width(), texture->height());
	}

	PopupLayer::PopupLayer(const std::shared_ptr<d3d11::Device>& device)
		: BrowserLayer(device) {
	}

	void PopupLayer::set_bounds(const CefRect& bounds) {
		const auto comp = composition();
		if (!comp) {
			return;
		}

		const auto outer_width = comp->width();
		const auto outer_height = comp->height();
		if (outer_width == 0 || outer_height == 0) {
			return;
		}

		original_bounds_ = bounds;
		bounds_ = bounds;

		// If x or y are negative, move them to 0.
		if (bounds_.x < 0) {
			bounds_.x = 0;
		}
		if (bounds_.y < 0) {
			bounds_.y = 0;
		}
		// If popup goes outside the view, try to reposition origin
		if (bounds_.x + bounds_.width > outer_width) {
			bounds_.x = outer_width - bounds_.width;
		}
		if (bounds_.y + bounds_.height > outer_height) {
			bounds_.y = outer_height - bounds_.height;
		}
		// If x or y became negative, move them to 0 again.
		if (bounds_.x < 0) {
			bounds_.x = 0;
		}
		if (bounds_.y < 0) {
			bounds_.y = 0;
		}

		const auto x = bounds_.x / float(outer_width);
		const auto y = bounds_.y / float(outer_height);
		const auto w = bounds_.width / float(outer_width);
		const auto h = bounds_.height / float(outer_height);
		move(x, y, w, h);
	}
	crender_window::crender_window()
	{
	}

	crender_window::~crender_window()
	{
	}

	bool crender_window::init(int32_t width, int32_t height, bool show)
	{
		const cef_color_t background_color = 0;// MainContext::Get()->GetBackgroundColor();
		const HBRUSH background_brush = CreateSolidBrush(
			RGB(CefColorGetR(background_color), CefColorGetG(background_color),
				CefColorGetB(background_color)));

		static ATOM wc_atom = 0;
		if (wc_atom == 0)
		{
		
			//RTC_DCHECK(wnd_class_ != 0);
		//	return wnd_class_ != 0;

#if 1
			//WNDCLASSA wc = {};
			////wc.cbSize = sizeof(WNDCLASSEX);
			//wc.style = NULL;// CS_HREDRAW | CS_VREDRAW;
			//wc.lpfnWndProc = WindowProc;
			//wc.hCursor = LoadCursor(NULL, IDC_ARROW);;// LoadCursor(::GetModuleHandle(nullptr), MAKEINTRESOURCE(IDC_ARROW));
			////wc.hCursor = LoadCursor(::GetModuleHandle(nullptr), MAKEINTRESOURCE(230));
			//wc.hbrBackground = (HBRUSH)GetStockObject(COLOR_APPWORKSPACE);//reinterpret_cast<HBRUSH>(COLOR_WINDOW);
			//wc.lpszClassName = kD3DClassName;
			//wc.hIcon = LoadIcon(::GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_APPLICATION));
			//
			WNDCLASSEX wcex;
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = WindowProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = ::GetModuleHandle(nullptr);
			wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); //(HICON)LoadImage(hInstance, "bullet_ico.ico", IMAGE_ICON, 0,0, LR_LOADTRANSPARENT);//LR_LOADFROMFILE);
			wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
			wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wcex.lpszMenuName = 0;
			wcex.lpszClassName = kD3DClassName;
			wcex.hIconSm = 0;
			wc_atom = RegisterClassEx(&wcex);
			//wc_atom = RegisterClass(&wc);
		//	  WNDCLASSA wc = {};

		//	wc.style = CS_HREDRAW | CS_VREDRAW;
		////	wc.style = CS_OWNDC;
		//	wc.lpfnWndProc = WindowProc;
		//	wc.cbClsExtra = 0;
		//	wc.cbWndExtra = 0;
		//	wc.hInstance = ::GetModuleHandle(nullptr);
		//	wc.hIcon = nullptr;
		//	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		//	wc.hbrBackground = background_brush;
		//	wc.lpszMenuName = nullptr;
		//	wc.lpszClassName = kD3DClassName;
		//	wc.lpfnWndProc = WindowProc;
		//	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		//	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
		//	//wc.lpszClassName = kD3DClassName;

		//	wc_atom = RegisterClassA(&wc); 
#else 
			WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
			wcex.style = CS_DBLCLKS;
			wcex.hInstance = GetModuleHandle(NULL);
			wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
			wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
			wcex.lpfnWndProc = WindowProc;
			wcex.lpszClassName = L"browser_render";
			wc_atom = ::RegisterClassExW(&wcex);
#endif 
#if 0
			WNDCLASSEX wcex;

			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_OWNDC;
			wcex.lpfnWndProc = WindowProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = ::GetModuleHandle(nullptr);
			wcex.hIcon = nullptr;
			wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wcex.hbrBackground = background_brush;
			wcex.lpszMenuName = nullptr;
			wcex.lpszClassName = kD3DClassName;
			//wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

			RegisterClassEx(&wcex);
#endif 
			if (wc_atom == 0)
			{
				return false;
			}
		}
		width_ = width;
		height_ = height;
		DWORD ex_style = WS_POPUP;
		//return  true;
		//if (GetWindowLongPtr(parent_hwnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE) {
		//	// Don't activate the browser window on creation.
		//	ex_style |= WS_EX_NOACTIVATE;
		//}
#if 1

		RECT clientSize;
		clientSize.top = 0;
		clientSize.left = 0;
		clientSize.right = width_;
		clientSize.bottom = height_;
		//ex_style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
		AdjustWindowRect(&clientSize, ex_style, false);
		hwnd_ = CreateWindow(kD3DClassName, "render", ex_style/*WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN*/, 0, 0,
		 	static_cast<int>(width_), static_cast<int>(height_),
			NULL, NULL, ::GetModuleHandle(nullptr), this);
		printf("hwnd_ = %p \n", hwnd_);
		/*
		if (!called_enable_non_client_dpi_scaling_ && IsProcessPerMonitorDpiAware()) {
			// This call gets Windows to scale the non-client area when WM_DPICHANGED
			// is fired on Windows versions < 10.0.14393.0.
			// Derived signature; not available in headers.
			typedef LRESULT(WINAPI* EnableChildWindowDpiMessagePtr)(HWND, BOOL);
			static EnableChildWindowDpiMessagePtr func_ptr =
				reinterpret_cast<EnableChildWindowDpiMessagePtr>(GetProcAddress(
					GetModuleHandle("user32.dll"), "EnableChildWindowDpiMessage"));
			if (func_ptr) {
				func_ptr(hwnd_, TRUE);
			}
		}
		
		*/
#else 
		hwnd_ =
			::CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, L"browser_render", L"browser_render",
				WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), this);

#endif // 
		
		if (show)
		{
			::ShowWindow(hwnd_, SW_SHOW);
			//::ShowWindow(hwnd_, SW_SHOWACTIVE);
		}
		else
		{
			::ShowWindow(hwnd_, SW_SHOW);
			::ShowWindow(hwnd_, SW_HIDE);
		}
		::UpdateWindow(hwnd_);


	/*	std::thread([=]() {
				MSG msg;
				while (GetMessage(&msg, NULL, 0, 0))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}).detach();*/
	/*	LONG dwNewLong = GetWindowLong(hwnd_, GWL_EXSTYLE);
		dwNewLong |= WS_EX_TRANSPARENT | WS_EX_LAYERED;
		SetWindowLong(hwnd_, GWL_EXSTYLE, dwNewLong);*/
		/*::SendMessage(hwnd_, WM_SETFONT, reinterpret_cast<WPARAM>(reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT))),
			TRUE);*/
		// Create a D3D11 device instance.
		device_ = d3d11::Device::create();
		//DCHECK(device_);
		if (!device_) 
		{
			printf("create 3D render failed !!!\n");
			return false;
		}

		// Create a D3D11 swapchain for the window.
		swap_chain_ = device_->create_swapchain(hwnd_);
		//DCHECK(swap_chain_);
		if (!swap_chain_) 
		{
			printf("create 3D render  swap failed !!!\n");
			return false;
		}

		// Create the browser layer.
		browser_layer_ = std::make_shared<BrowserLayer>(device_);

		// Set up the composition.
		composition_ = std::make_shared<d3d11::Composition>(device_, width, height);
		composition_->add_layer(browser_layer_);

		// Size to the whole composition.
		browser_layer_->move(0.0f, 0.0f, 1.0f, 1.0f);

		start_time_ = GetTimeNow();
		
	
		device_->create_shared_texture(DXGI_FORMAT_B8G8R8A8_UNORM, width, height);
		printf("[%s][%d][g_gpu_addresses_callback_ptr = %p][pSharedHandle = %p]\n", 
			__FUNCTION__, __LINE__, g_gpu_addresses_callback_ptr , device_->pSharedHandle);
		if (g_gpu_addresses_callback_ptr)
		{

			g_gpu_addresses_callback_ptr(device_->pSharedHandle);
		}
		//g_rtc_mgr.webrtc_texture(device_->pSharedHandle, 87, width_, height_);
		//SetBrowser(browser);
		return true;
		return true;
	}

	void crender_window::update()
	{
	}

	void crender_window::destroy()
	{
	}



	void crender_window::OnAcceleratedPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const CefAcceleratedPaintInfo& info)
	{
		//return;
		if (type == PET_POPUP) 
		{
			//return ;
			popup_layer_->on_paint(info.shared_texture_handle);
			
		}
		else 
		{
			browser_layer_->on_paint(info.shared_texture_handle);
			
		}

		Render();


		if (type == PET_POPUP) {
			popup_layer_->copy_shared_textrue();

		}
		else {
			browser_layer_->copy_shared_textrue( );

		}

		//g_rtc_mgr.webrtc_texture(device_->pSharedHandle, 87, width_, height_);
	}

	void  crender_window::Render()
	{
		// Update composition + layers based on time.
		const auto t = (GetTimeNow() - start_time_) / 1000000.0;
		composition_->tick(t);

		auto ctx = device_->immedidate_context();
		swap_chain_->bind(ctx);

		const auto texture_size = browser_layer_->texture_size();

		// Resize the composition and swap chain to match the texture if necessary.
		composition_->resize(!true, texture_size.first,
			texture_size.second);
		swap_chain_->resize(texture_size.first, texture_size.second);

		// Clear the render target.
		swap_chain_->clear(0.0f, 0.0f, 1.0f, 1.0f);

		// Render the scene.
		composition_->render(ctx);

		// Present to window.
		swap_chain_->present(true ? 0 : 1);
	}

	void crender_window::_on_mouse_event(UINT message, WPARAM wParam, LPARAM lParam)
	{
		printf("[%s][%d]\n", __FUNCTION__, __LINE__);
		if (_is_mouse_event_from_touch(message)) 
		{

			return;
		}

#if 1
		/*if (!browser_source_ptr)
		{
			return;
		}
		browser_source_ptr->
		CefRefPtr<CefBrowserHost> browser_host;
		if (browser_) {
			browser_host = browser_->GetHost();
		}*/

#endif // 
		LONG currentTime = 0;
		bool cancelPreviousClick = false;

		if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN ||
			message == WM_MBUTTONDOWN || message == WM_MOUSEMOVE ||
			message == WM_MOUSELEAVE) {
			currentTime = GetMessageTime();
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
#if 0
			cancelPreviousClick =
				(abs(last_click_x_ - x) > (GetSystemMetrics(SM_CXDOUBLECLK) / 2)) ||
				(abs(last_click_y_ - y) > (GetSystemMetrics(SM_CYDOUBLECLK) / 2)) ||
				((currentTime - last_click_time_) > GetDoubleClickTime());
			if (cancelPreviousClick &&
				(message == WM_MOUSEMOVE || message == WM_MOUSELEAVE)) {
				last_click_count_ = 1;
				last_click_x_ = 0;
				last_click_y_ = 0;
				last_click_time_ = 0;
			}
#endif 
		}

		switch (message) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN: 
		{
			::SetCapture(hwnd_);
			::SetFocus(hwnd_);
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			if (wParam & MK_SHIFT) {
				// Start rotation effect.
				/*last_mouse_pos_.x = current_mouse_pos_.x = x;
				last_mouse_pos_.y = current_mouse_pos_.y = y;
				mouse_rotation_ = true;*/
			}
			else {
				CefBrowserHost::MouseButtonType btnType =
					(message == WM_LBUTTONDOWN
						? MBT_LEFT
						: (message == WM_RBUTTONDOWN ? MBT_RIGHT : MBT_MIDDLE));
				/*if (!cancelPreviousClick && (btnType == last_click_button_)) {
					++last_click_count_;
				}
				else {
					last_click_count_ = 1;
					last_click_x_ = x;
					last_click_y_ = y;
				}
				last_click_time_ = currentTime;
				last_click_button_ = btnType;*/

				if (browser_source_ptr)
				{
					CefMouseEvent mouse_event;
					mouse_event.x = x;
					mouse_event.y = y;
					/*last_mouse_down_on_view_ = !IsOverPopupWidget(x, y);
					ApplyPopupOffset(mouse_event.x, mouse_event.y);
					DeviceToLogical(mouse_event, device_scale_factor_);*/
					mouse_event.modifiers = GetCefMouseModifiers(wParam);
					browser_source_ptr->SendMouseClick(mouse_event, btnType, false,
						1/*last_click_count_*/);
				}
			}
		} 
		break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			if (GetCapture() == hwnd_) {
				ReleaseCapture();
			}
			//if (mouse_rotation_) {
			//	// End rotation effect.
			//	mouse_rotation_ = false;
			//	render_handler_->SetSpin(0, 0);
			//}
			//else
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				CefBrowserHost::MouseButtonType btnType =
					(message == WM_LBUTTONUP
						? MBT_LEFT
						: (message == WM_RBUTTONUP ? MBT_RIGHT : MBT_MIDDLE));
				if (browser_source_ptr)
				{
					CefMouseEvent mouse_event;
					mouse_event.x = x;
					mouse_event.y = y;
					/*if (last_mouse_down_on_view_ && IsOverPopupWidget(x, y) &&
						(GetPopupXOffset() || GetPopupYOffset())) {
						break;
					}
					ApplyPopupOffset(mouse_event.x, mouse_event.y);
					DeviceToLogical(mouse_event, device_scale_factor_);*/
					mouse_event.modifiers = GetCefMouseModifiers(wParam);
					browser_source_ptr->SendMouseClick(mouse_event, btnType, true,
						1/*last_click_count_*/);
				}
			}
			break;

		case WM_MOUSEMOVE: {
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			//if (mouse_rotation_) {
			//	// Apply rotation effect.
			//	current_mouse_pos_.x = x;
			//	current_mouse_pos_.y = y;
			//	render_handler_->IncrementSpin(
			//		current_mouse_pos_.x - last_mouse_pos_.x,
			//		current_mouse_pos_.y - last_mouse_pos_.y);
			//	last_mouse_pos_.x = current_mouse_pos_.x;
			//	last_mouse_pos_.y = current_mouse_pos_.y;
			//}
			//else 
			{
#if 0
				if (!mouse_tracking_)
				{
					// Start tracking mouse leave. Required for the WM_MOUSELEAVE event to
					// be generated.
					TRACKMOUSEEVENT tme;
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = hwnd_;
					TrackMouseEvent(&tme);
					mouse_tracking_ = true;
				}
#endif 
				if (browser_source_ptr) {
					CefMouseEvent mouse_event;
					mouse_event.x = x;
					mouse_event.y = y;
				//	ApplyPopupOffset(mouse_event.x, mouse_event.y);
					//DeviceToLogical(mouse_event, device_scale_factor_);
					mouse_event.modifiers = GetCefMouseModifiers(wParam);
					browser_source_ptr->SendMouseMove(mouse_event, false);
				}
			}
			break;
		}

		case WM_MOUSELEAVE: {
#if 0
			if (mouse_tracking_) {
				// Stop tracking mouse leave.
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE & TME_CANCEL;
				tme.hwndTrack = hwnd_;
				TrackMouseEvent(&tme);
				mouse_tracking_ = false;
			}

#endif


			if (browser_source_ptr)
			{
				// Determine the cursor position in screen coordinates.
				POINT p;
				::GetCursorPos(&p);
				::ScreenToClient(hwnd_, &p);

				CefMouseEvent mouse_event;
				mouse_event.x = p.x;
				mouse_event.y = p.y;
			//	DeviceToLogical(mouse_event, device_scale_factor_);
				mouse_event.modifiers = GetCefMouseModifiers(wParam);
				browser_source_ptr->SendMouseMove(mouse_event, true);
			}
		} break;

		case WM_MOUSEWHEEL:
			if (browser_source_ptr)
			{
				printf("[WM_MOUSEWHEEL]\n");
				POINT screen_point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				HWND scrolled_wnd = ::WindowFromPoint(screen_point);
				if (scrolled_wnd != hwnd_) {
					break;
				}

				ScreenToClient(hwnd_, &screen_point);
				int delta = GET_WHEEL_DELTA_WPARAM(wParam);

				CefMouseEvent mouse_event;
				mouse_event.x = screen_point.x;
				mouse_event.y = screen_point.y;
				//ApplyPopupOffset(mouse_event.x, mouse_event.y);
				//DeviceToLogical(mouse_event, device_scale_factor_);
				mouse_event.modifiers = GetCefMouseModifiers(wParam);
				browser_source_ptr->SendMouseWheel(mouse_event,
					IsKeyDown(VK_SHIFT) ? delta : 0,
					!IsKeyDown(VK_SHIFT) ? delta : 0);
			}
			else
			{
				printf("[WM_MOUSEWHEEL]!!! failed \n");
			}
			break;
		}
	}

	void crender_window::_on_size()
	{
		// Keep |client_rect_| up to date.
		RECT client_rect_ = {0};
		::GetClientRect(hwnd_, &client_rect_);

		if (browser_source_ptr) 
		{
			browser_source_ptr->OnSize();
			//browser_->GetHost()->WasResized();
		}
	}

	void crender_window::_on_focus(bool set_focus)
	{
		if (browser_source_ptr)
		{
			browser_source_ptr->SendFocus(set_focus);
		}
	}

	void crender_window::_on_capture_lost()
	{
		if (mouse_rotation_) {
			return;
		}

		if (browser_source_ptr)
		{
			browser_source_ptr->SendCaptureLostEvent();
		}
	}

	void crender_window::_on_key_event(UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (!browser_source_ptr)
		{
			return;
		}

		CefKeyEvent event;
		event.windows_key_code = wParam;
		event.native_key_code = lParam;
		event.is_system_key = message == WM_SYSCHAR || message == WM_SYSKEYDOWN ||
			message == WM_SYSKEYUP;

		if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN) {
			event.type = KEYEVENT_RAWKEYDOWN;
		}
		else if (message == WM_KEYUP || message == WM_SYSKEYUP) {
			event.type = KEYEVENT_KEYUP;
		}
		else {
			event.type = KEYEVENT_CHAR;
		}
		event.modifiers = GetCefKeyboardModifiers(wParam, lParam);

		// mimic alt-gr check behaviour from
		// src/ui/events/win/events_win_utils.cc: GetModifiersFromKeyState
		if ((event.type == KEYEVENT_CHAR) && IsKeyDown(VK_RMENU)) {
			// reverse AltGr detection taken from PlatformKeyMap::UsesAltGraph
			// instead of checking all combination for ctrl-alt, just check current char
			HKL current_layout = ::GetKeyboardLayout(0);

			// https://docs.microsoft.com/en-gb/windows/win32/api/winuser/nf-winuser-vkkeyscanexw
			// ... high-order byte contains the shift state,
			// which can be a combination of the following flag bits.
			// 1 Either SHIFT key is pressed.
			// 2 Either CTRL key is pressed.
			// 4 Either ALT key is pressed.
			SHORT scan_res = ::VkKeyScanExW(wParam, current_layout);
			constexpr auto ctrlAlt = (2 | 4);
			if (((scan_res >> 8) & ctrlAlt) == ctrlAlt) {  // ctrl-alt pressed
				event.modifiers &= ~(EVENTFLAG_CONTROL_DOWN | EVENTFLAG_ALT_DOWN);
				event.modifiers |= EVENTFLAG_ALTGR_DOWN;
			}
		}

		browser_source_ptr->SendKeyClick(event);
	}

	void crender_window::_on_paint()
	{
		// Paint nothing here. Invalidate will cause OnPaint to be called for the
		// render handler.
		PAINTSTRUCT ps;
		BeginPaint(hwnd_, &ps);
		EndPaint(hwnd_, &ps);
		if (browser_source_ptr)
		{
			browser_source_ptr->OnPaint();
		}
	}

	bool crender_window::_on_erase_bkgnd()
	{
		// Erase the background when the browser does not exist.
		return  browser_source_ptr == nullptr ;
	}

	void crender_window::_on_nc_create(LPCREATESTRUCT lpCreateStruct)
	{
		if (IsProcessPerMonitorDpiAware()) {
			// This call gets Windows to scale the non-client area when WM_DPICHANGED
			// is fired on Windows versions >= 10.0.14393.0.
			typedef BOOL(WINAPI* EnableNonClientDpiScalingPtr)(HWND);
			static EnableNonClientDpiScalingPtr func_ptr =
				reinterpret_cast<EnableNonClientDpiScalingPtr>(GetProcAddress(
					GetModuleHandle("user32.dll"), "EnableNonClientDpiScaling"));
			called_enable_non_client_dpi_scaling_ = !!(func_ptr && func_ptr(hwnd_));
		}
	}

	void crender_window::_on_create(LPCREATESTRUCT lpCreateStruct)
	{
		const HINSTANCE hInstance = lpCreateStruct->hInstance;

		RECT rect;
		GetClientRect(hwnd_, &rect);

		//if (with_controls_) {
		//	// Create the child controls.
		//	int x_offset = 0;

		//	const int button_width = GetButtonWidth(hwnd_);
		//	const int urlbar_height = GetURLBarHeight(hwnd_);

		//	back_hwnd_ = CreateWindow(
		//		L"BUTTON", L"Back", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED,
		//		x_offset, 0, button_width, urlbar_height, hwnd_,
		//		reinterpret_cast<HMENU>(IDC_NAV_BACK), hInstance, nullptr);
		//	CHECK(back_hwnd_);
		//	x_offset += button_width;

		//	forward_hwnd_ = CreateWindow(
		//		L"BUTTON", L"Forward",
		//		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED, x_offset, 0,
		//		button_width, urlbar_height, hwnd_,
		//		reinterpret_cast<HMENU>(IDC_NAV_FORWARD), hInstance, nullptr);
		//	CHECK(forward_hwnd_);
		//	x_offset += button_width;

		//	reload_hwnd_ = CreateWindow(
		//		L"BUTTON", L"Reload",
		//		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED, x_offset, 0,
		//		button_width, urlbar_height, hwnd_,
		//		reinterpret_cast<HMENU>(IDC_NAV_RELOAD), hInstance, nullptr);
		//	CHECK(reload_hwnd_);
		//	x_offset += button_width;

		//	stop_hwnd_ = CreateWindow(
		//		L"BUTTON", L"Stop", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED,
		//		x_offset, 0, button_width, urlbar_height, hwnd_,
		//		reinterpret_cast<HMENU>(IDC_NAV_STOP), hInstance, nullptr);
		//	CHECK(stop_hwnd_);
		//	x_offset += button_width;

		//	edit_hwnd_ =
		//		CreateWindow(L"EDIT", nullptr,
		//			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |
		//			ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_DISABLED,
		//			x_offset, 0, rect.right - button_width * 4, urlbar_height,
		//			hwnd_, nullptr, hInstance, nullptr);
		//	CHECK(edit_hwnd_);

		//	// Override the edit control's window procedure.
		//	edit_wndproc_old_ = SetWndProcPtr(edit_hwnd_, EditWndProc);

		//	// Associate |this| with the edit window.
		//	SetUserDataPtr(edit_hwnd_, this);

		//	rect.top += urlbar_height;

		//	if (!with_osr_) {
		//		// Remove the menu items that are only used with OSR.
		//		HMENU hMenu = ::GetMenu(hwnd_);
		//		if (hMenu) {
		//			HMENU hTestMenu = ::GetSubMenu(hMenu, 2);
		//			if (hTestMenu) {
		//				::RemoveMenu(hTestMenu, ID_TESTS_OSR_FPS, MF_BYCOMMAND);
		//				::RemoveMenu(hTestMenu, ID_TESTS_OSR_DSF, MF_BYCOMMAND);
		//			}
		//		}
		//	}
		//}
		//else {
			// No controls so also remove the default menu.
			::SetMenu(hwnd_, nullptr);
		//}

		const float device_scale_factor = GetWindowScaleFactor(hwnd_);

		/*if (with_osr_) {
			browser_window_->SetDeviceScaleFactor(device_scale_factor);
		}*/

		//if (!is_popup_) {
		//	// Create the browser window.
		//	CefRect cef_rect(rect.left, rect.top, rect.right - rect.left,
		//		rect.bottom - rect.top);
		//	browser_window_->CreateBrowser(hwnd_, cef_rect, browser_settings_, nullptr,
		//		delegate_->GetRequestContext());
		//}
		//else 
		{
			// With popups we already have a browser window. Parent the browser window
			// to the root window and show it in the correct location.
			//browser_window_->ShowPopup(hwnd_, rect.left, rect.top,
			//	rect.right - rect.left, rect.bottom - rect.top);
		}

		//window_created_ = true;
	}

	bool crender_window::_is_mouse_event_from_touch(uint32_t message)
	{
		// Helper function to detect mouse messages coming from emulation of touch
// events. These should be ignored.
#define MOUSEEVENTF_FROMTOUCH 0xFF515700
		return (message >= WM_MOUSEFIRST) && (message <= WM_MOUSELAST) &&
			(GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) ==
			MOUSEEVENTF_FROMTOUCH;
	}


	LRESULT  CALLBACK crender_window::WindowProc(HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam) 
	{
		if (message != WM_PAINT)
		{
			printf("[%s][%d][hWnd = %p][message = %u]\n", __FUNCTION__, __LINE__, hWnd, message);
		}
		
		if (message == WM_GETMINMAXINFO)
		{
			MINMAXINFO* pMinMaxInfo = (MINMAXINFO*)lParam;
			pMinMaxInfo->ptMinTrackSize.x = 200; // 最小宽度
			pMinMaxInfo->ptMinTrackSize.y = 100; // 最小高度
			pMinMaxInfo->ptMaxTrackSize.x = 1920; // 最大宽度
			pMinMaxInfo->ptMaxTrackSize.y = 1080; // 最大高度
			return 0;
		}

		//if (message == WM_DESTROY || (message == WM_CHAR && message == VK_RETURN)) {
		//	PostQuitMessage(0);
		//	return 0;
		//}
		crender_window* self = NULL;
		/*if (message != WM_NCCREATE) {
			self = GetUserDataPtr<crender_window*>(hWnd);
			if (!self) {
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			DCHECK_EQ(hWnd, self->hwnd_);
		}*/
		
		//return DefWindowProc(hWnd, message, wParam, lParam);
		//crender_window* 
		self = reinterpret_cast<crender_window*>(GetWindowLongPtr (hWnd, GWLP_USERDATA));
		//return 0;
		//crender_window* me =
		//	reinterpret_cast<crender_window*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA))
		/*
		* // 创建窗口会发送三个事件
		[chen::crender_window::WindowProc][689][hWnd = 000000000005207C][message = 36] ==> WM_GETMINMAXINFO
		[chen::crender_window::WindowProc][689][hWnd = 000000000005207C][message = 129] ==> WM_NCCREATE
		[chen::crender_window::WindowProc][689][hWnd = 000000000005207C][message = 130]
		*/
		if (!self && (/* WM_CREATE == message */  WM_NCCREATE == message ))
		{
		 	CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			 self = reinterpret_cast<crender_window*>(cs->lpCreateParams);
			 if (!self)
			 {
				 printf("[%s][%d]!self\n", __FUNCTION__, __LINE__);
				 return DefWindowProc(hWnd, message, wParam, lParam);
				
			 }
			 else
			 {
				 self->hwnd_ = hWnd;
				 // Associate |self| with the main window.
				 //SetUserDataPtr(hWnd, self);
				 //self->hwnd_ = hWnd;
				 self->_on_nc_create(cs);
				 ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
				// return DefWindowProc(hWnd, message, wParam, lParam);
			 }
			// return DefWindowProc(hWnd, mssag);
			printf("[%s][%d]\n", __FUNCTION__, __LINE__);
			
		}
		else if (!self )
		{
			printf("[%s][%d]!self\n", __FUNCTION__, __LINE__);
			return DefWindowProc(hWnd, message, wParam, lParam);
			return 0;
		}
		/*
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 36]
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 129]
[chen::crender_window::WindowProc][903]!self
[chen::crender_window::WindowProc][914]
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 131]
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 1]
 
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 24] = > WM_QUIT
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 70] => WM_WINDOWPOSCHANGING => SetWindowPos
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 133]
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 20]  => WM_Close
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 71] = > WM_WINDOWPOSCHANGING
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 127] => WM_NCXBUTTONDOWN
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 127]
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 127]
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 5]
[chen::crender_window::WindowProc][862][hWnd = 00000000000925E6][message = 3]
																		  136 ==> WM_SYNCPAINT
		
		*/
		// We want to handle IME events before the OS does any default handling.
		switch (message) 
		{
		case WM_CREATE:
		{
			self->_on_create(reinterpret_cast<CREATESTRUCT*>(lParam));
			break;
		}
		case WM_QUIT:
		{
			return 0;
			break;
		}
		case WM_CLOSE: // 20
		{
			break;
		}
		case WM_SYNCPAINT:
		{
			break;
		}
		case WM_NCXBUTTONDOWN:
		{
			uint32_t xPos = GET_X_LPARAM(lParam);
			uint32_t yPos = GET_Y_LPARAM(lParam);
			break;
		}
		/*case WM_SIZE:
		{
		
			break;
		}*/
		case WM_WINDOWPOSCHANGING:
		case WM_WINDOWPOSCHANGED:
		{
		
			// Retrieve current window placement information.
			//WINDOWPLACEMENT placement;
			//::GetWindowPlacement(self->hwnd_, &placement);

			//// Retrieve information about the display that contains the window.
			//HMONITOR monitor =
			//	MonitorFromRect(&placement.rcNormalPosition, MONITOR_DEFAULTTONEAREST);
			//MONITORINFO info;
			//info.cbSize = sizeof(info);
			//GetMonitorInfo(monitor, &info);

			//// Make sure the window is inside the display.
			//CefRect display_rect(info.rcWork.left, info.rcWork.top,
			//	info.rcWork.right - info.rcWork.left,
			//	info.rcWork.bottom - info.rcWork.top);
			//CefRect window_rect(0, 0, 1920, 1080);
			////WindowTestRunner::ModifyBounds(display_rect, window_rect);
			//SetWindowPos(self->hwnd_, nullptr, window_rect.x, window_rect.y,
			//	window_rect.width, window_rect.height, SWP_NOZORDER);
			break;
		}
		case WM_IME_SETCONTEXT:
		{
			// We handle the IME Composition Window ourselves (but let the IME Candidates
			  // Window be handled by IME through DefWindowProc()), so clear the
			  // ISC_SHOWUICOMPOSITIONWINDOW flag:
			lParam &= ~ISC_SHOWUICOMPOSITIONWINDOW;
			::DefWindowProc(hWnd, message, wParam, lParam);
			return 0;
		}
		//  TODO@chensong 2024-12-26   处理游标消息，不处理就会一直转圈	 
		case WM_SETCURSOR:
		{
			//switch (LOWORD(lParam))
			//{
			//default:
			//	::SetCursor(LoadCursor(NULL, IDC_ARROW));
			//	break;
			//}


			uint32_t xPos = GET_X_LPARAM(lParam);
			uint32_t yPos = GET_Y_LPARAM(lParam);
			break;
		}
		case WM_IME_STARTCOMPOSITION:
		{
			//self->OnIMEStartComposition();
			return 0;
		}
		case WM_IME_COMPOSITION:
		{
			//self->OnIMEComposition(message, wParam, lParam);
			return 0;
		}
		case WM_IME_ENDCOMPOSITION:
		{
			//self->OnIMECancelCompositionEvent();
			// Let WTL call::DefWindowProc() and release its resources.
			break;
		}
#if defined(CEF_USE_ATL)
		case WM_GETOBJECT: {
			// Only the lower 32 bits of lParam are valid when checking the object id
			// because it sometimes gets sign-extended incorrectly (but not always).
			DWORD obj_id = static_cast<DWORD>(static_cast<DWORD_PTR>(lParam));

			// Accessibility readers will send an OBJID_CLIENT message.
			if (static_cast<DWORD>(OBJID_CLIENT) == obj_id) {
				//if (self->accessibility_root_) {
				//	return LresultFromObject(
				//		IID_IAccessible, wParam,
				//		static_cast<IAccessible*>(self->accessibility_root_));
				//}
				//else {
				//	// Notify the renderer to enable accessibility.
				//	if (self->browser_ && self->browser_->GetHost()) {
				//		self->browser_->GetHost()->SetAccessibilityState(STATE_ENABLED);
				//	}
				//}
			}
		} break;
#endif
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_MOUSELEAVE:
		case WM_MOUSEWHEEL:
		{
			self->_on_mouse_event(message, wParam, lParam);
			break;
		}

		case WM_SIZE:
			self->_on_size();
			break;

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			self->_on_focus(message == WM_SETFOCUS);
			break;

		case WM_CAPTURECHANGED:
		case WM_CANCELMODE:
			self->_on_capture_lost();
			break;

		case WM_SYSCHAR:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
			self->_on_key_event(message, wParam, lParam);
			break;

		case WM_PAINT:
		case WM_NCPAINT: // 133
			self->_on_paint();
			return 0;

		case WM_ERASEBKGND:
			if (self->_on_erase_bkgnd())
			{
				break;
			}
			// Don't erase the background.
			return 0;

			// If your application does not require Win7 support, please do consider
			// using WM_POINTER* messages instead of WM_TOUCH. WM_POINTER are more
			// intutive, complete and simpler to code.
			// https://msdn.microsoft.com/en-us/library/hh454903(v=vs.85).aspx
		case WM_TOUCH:
			/*if (self->OnTouchEvent(message, wParam, lParam)) {
				return 0;
			}*/
			break;

		case WM_NCDESTROY:
			{
				// Clear the reference to |self|.
				SetUserDataPtr(hWnd, nullptr);
				self->hwnd_ = nullptr;
				break;
			}
			default:
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
		return true;
	}
}
 