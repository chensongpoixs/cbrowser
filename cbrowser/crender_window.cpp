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
#include "crender_browser.h"
namespace chen {
	//cmediasoup_mgr g_rtc_mgr;;
	set_gpu_addresses_callback     g_gpu_addresses_callback_ptr = NULL;;
	crender_window  render_window_ptr;
	const char kD3DClassName[] = "browser_render";
	BrowserLayer::BrowserLayer(const std::shared_ptr<d3d11::Device>& device)
		: d3d11::Layer(device, false /* flip */) {
		frame_buffer_ = std::make_shared<d3d11::FrameBuffer>(device_);
	}

	void BrowserLayer::render(const std::shared_ptr<d3d11::Context>& ctx) {
		// Use the base class method to draw our texture.
		render_texture(ctx, frame_buffer_->texture());
	}

	void BrowserLayer::on_paint(void* share_handle) {
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

	bool crender_window::init(int32_t width, int32_t height)
	{
		

		static ATOM wc_atom = 0;
		if (wc_atom == 0) {
			WNDCLASSA wc = {};

			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = WindowProc;
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
			wc.lpszClassName = kD3DClassName;

			wc_atom = RegisterClassA(&wc);
			if (wc_atom == 0)
				return false;
		}
		width_ = width;
		height_ = height;
		hwnd_ = CreateWindowA(kD3DClassName, "render", WS_OVERLAPPEDWINDOW, 0, 0,
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
		ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
		ShowWindow(hwnd_, SW_HIDE);
		device_->create_shared_texture(DXGI_FORMAT_B8G8R8A8_UNORM, width, height);

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


	LRESULT WINAPI crender_window::WindowProc(HWND hwnd,
		UINT msg,
		WPARAM wparam,
		LPARAM lparam) {
		if (msg == WM_DESTROY || (msg == WM_CHAR && wparam == VK_RETURN)) {
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProcA(hwnd, msg, wparam, lparam);
	}
}
 