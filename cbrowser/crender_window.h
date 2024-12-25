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
#ifndef _C_RENDER_WINDOW_H_
#define _C_RENDER_WINDOW_H_
//#include "cconfig.h"
//#include "cconfig.h"
//#include "csingleton.h"
#include <iostream>
#include "cd3d11_win.h"
#include "include/internal/cef_types_wrappers.h"
#include "cmediasoup_mgr.h"
#include "crender_browser.h"
namespace chen {



	class BrowserLayer : public d3d11::Layer {
	public:
		explicit BrowserLayer(const std::shared_ptr<d3d11::Device>& device);

		void render(const std::shared_ptr<d3d11::Context>& ctx) override;

		void on_paint(void* share_handle);
		void copy_shared_textrue();

		// After calling on_paint() we can query the texture size.
		std::pair<uint32_t, uint32_t> texture_size() const;

	private:
		std::shared_ptr<d3d11::FrameBuffer> frame_buffer_;

		DISALLOW_COPY_AND_ASSIGN(BrowserLayer);
	};

	class PopupLayer : public BrowserLayer {
	public:
		explicit PopupLayer(const std::shared_ptr<d3d11::Device>& device);

		void set_bounds(const CefRect& bounds);

		bool contains(int x, int y) const { return bounds_.Contains(x, y); }
		int xoffset() const { return original_bounds_.x - bounds_.x; }
		int yoffset() const { return original_bounds_.y - bounds_.y; }

	private:
		CefRect original_bounds_;
		CefRect bounds_;

		DISALLOW_COPY_AND_ASSIGN(PopupLayer);
	};



	class crender_window
	{
	public:
		explicit crender_window();
		virtual ~crender_window();


	public:

		bool init(int32_t width, int32_t height, bool show);
		void update();
		void destroy();
	public:
		static LRESULT WINAPI WindowProc(HWND hwnd,
			UINT msg,
			WPARAM wparam,
			LPARAM lparam);
		

		void OnAcceleratedPaint(
			CefRefPtr<CefBrowser> browser,
			CefRenderHandler::PaintElementType type,
			const CefRenderHandler::RectList& dirtyRects,
			const CefAcceleratedPaintInfo& info);



	private:
		void Render()  ;
		


		void _on_mouse_event(UINT message, WPARAM wParam, LPARAM lParam);
		void _on_key_event(UINT message, WPARAM wParam, LPARAM lParam);
		void _on_size();
		void _on_focus(bool set_focus);
		void _on_capture_lost();
		
		void _on_paint();
		bool _on_erase_bkgnd();
	private:


		bool _is_mouse_event_from_touch(uint32_t message);

	private:

		size_t width_, height_;

		HWND hwnd_;
		uint64_t start_time_ = 0;
		bool mouse_rotation_ = false;
		std::shared_ptr<d3d11::Device> device_;
		std::shared_ptr<d3d11::SwapChain> swap_chain_;
		std::shared_ptr<d3d11::Composition> composition_;
		std::shared_ptr<BrowserLayer> browser_layer_;
		std::shared_ptr<PopupLayer> popup_layer_;

		//DISALLOW_COPY_AND_ASSIGN(OsrRenderHandlerWinD3D11);
	};

	//extern cmediasoup_mgr g_rtc_mgr;;

	extern  set_gpu_addresses_callback     g_gpu_addresses_callback_ptr;
	extern  crender_window  render_window_ptr;
}


#endif // _C_RENDER_WINDOW_H_
 