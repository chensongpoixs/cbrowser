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
namespace chen {
	//cmediasoup_mgr g_rtc_mgr;;
	set_gpu_addresses_callback     g_gpu_addresses_callback_ptr = NULL;;
	crender_window  render_window_ptr;
	const wchar_t kD3DClassName[] = L"browser_render";
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
		const cef_color_t background_color = MainContext::Get()->GetBackgroundColor();
		const HBRUSH background_brush = CreateSolidBrush(
			RGB(CefColorGetR(background_color), CefColorGetG(background_color),
				CefColorGetB(background_color)));

		static ATOM wc_atom = 0;
		if (wc_atom == 0) {
			/*WNDCLASSA wc = {};

			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = WindowProc;
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
			wc.lpszClassName = kD3DClassName;

			wc_atom = RegisterClassA(&wc);*/


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
			wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

			RegisterClassEx(&wcex);
			if (wc_atom == 0)
				return false;
		}
		width_ = width;
		height_ = height;
		DWORD ex_style = 0;
		if (GetWindowLongPtr(parent_hwnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE) {
			// Don't activate the browser window on creation.
			ex_style |= WS_EX_NOACTIVATE;
		}
		hwnd_ = CreateWindowEx(kD3DClassName, "render", WS_OVERLAPPEDWINDOW, 0, 0,
			static_cast<int>(width_), static_cast<int>(height_),
			NULL, NULL, NULL, NULL);
		// Create a D3D11 device instance.
		device_ = d3d11::Device::create();
		//DCHECK(device_);
		if (!device_) {
			return false;
		}

		// Create a D3D11 swapchain for the window.
		swap_chain_ = device_->create_swapchain(hwnd_);
		//DCHECK(swap_chain_);
		if (!swap_chain_) {
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
		
		if (show)
		{
			::ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
		}
		else
		{
			::ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
			::ShowWindow(hwnd_, SW_HIDE);
		}
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
		if (type == PET_POPUP) {
			popup_layer_->on_paint(info.shared_texture_handle);
			
		}
		else {
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
		if (_is_mouse_event_from_touch(message)) {
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
			if (browser_source_ptr) {
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

	bool crender_window::_is_mouse_event_from_touch(uint32_t message)
	{
		// Helper function to detect mouse messages coming from emulation of touch
// events. These should be ignored.
#define MOUSEEVENTF_FROMTOUCH 0xFF515700
		return (message >= WM_MOUSEFIRST) && (message <= WM_MOUSELAST) &&
			(GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) ==
			MOUSEEVENTF_FROMTOUCH;
	}


	LRESULT WINAPI crender_window::WindowProc(HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam) {
		/*if (msg == WM_DESTROY || (msg == WM_CHAR && wparam == VK_RETURN)) {
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProcA(hwnd, msg, wparam, lparam);*/
		crender_window* self = reinterpret_cast<crender_window*>(GetWindowLongPtr (hWnd, GWLP_USERDATA));
		if (!self) 
		{
			printf("[%s][%d]\n", __FUNCTION__, __LINE__);
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		// We want to handle IME events before the OS does any default handling.
		switch (message) {
		case WM_IME_SETCONTEXT:
		{
			// We handle the IME Composition Window ourselves (but let the IME Candidates
			  // Window be handled by IME through DefWindowProc()), so clear the
			  // ISC_SHOWUICOMPOSITIONWINDOW flag:
			lParam &= ~ISC_SHOWUICOMPOSITIONWINDOW;
			::DefWindowProc(hWnd, message, wParam, lParam);
			return 0;
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
			self->_on_mouse_event(message, wParam, lParam);
			break;

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
			// Clear the reference to |self|.
			SetUserDataPtr(hWnd, nullptr);
			self->hwnd_ = nullptr;
			break;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
 