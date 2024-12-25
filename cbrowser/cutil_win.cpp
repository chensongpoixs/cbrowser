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
#include "cutil_win.h"


namespace chen {
	namespace {

		LARGE_INTEGER qi_freq_ = {};

	}  // namespace

	uint64_t GetTimeNow() {
		if (!qi_freq_.HighPart && !qi_freq_.LowPart) {
			QueryPerformanceFrequency(&qi_freq_);
		}
		LARGE_INTEGER t = {};
		QueryPerformanceCounter(&t);
		return static_cast<uint64_t>((t.QuadPart / double(qi_freq_.QuadPart)) *
			1000000);
	}





    void SetUserDataPtr(HWND hWnd, void* ptr) {
        SetLastError(ERROR_SUCCESS);
        LONG_PTR result =
            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
        CHECK(result != 0 || GetLastError() == ERROR_SUCCESS);
    }

    WNDPROC SetWndProcPtr(HWND hWnd, WNDPROC wndProc) {
        WNDPROC old =
            reinterpret_cast<WNDPROC>(::GetWindowLongPtr(hWnd, GWLP_WNDPROC));
        CHECK(old != nullptr);
        LONG_PTR result = ::SetWindowLongPtr(hWnd, GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(wndProc));
        CHECK(result != 0 || GetLastError() == ERROR_SUCCESS);
        return old;
    }

    std::wstring GetResourceString(UINT id) {
#define MAX_LOADSTRING 100
        TCHAR buff[MAX_LOADSTRING] = { 0 };
        LoadString(::GetModuleHandle(nullptr), id, buff, MAX_LOADSTRING);
        return buff;
    }

    int GetCefMouseModifiers(WPARAM wparam) {
        int modifiers = 0;
        if (wparam & MK_CONTROL) {
            modifiers |= EVENTFLAG_CONTROL_DOWN;
        }
        if (wparam & MK_SHIFT) {
            modifiers |= EVENTFLAG_SHIFT_DOWN;
        }
        if (IsKeyDown(VK_MENU)) {
            modifiers |= EVENTFLAG_ALT_DOWN;
        }
        if (wparam & MK_LBUTTON) {
            modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
        }
        if (wparam & MK_MBUTTON) {
            modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
        }
        if (wparam & MK_RBUTTON) {
            modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
        }

        // Low bit set from GetKeyState indicates "toggled".
        if (::GetKeyState(VK_NUMLOCK) & 1) {
            modifiers |= EVENTFLAG_NUM_LOCK_ON;
        }
        if (::GetKeyState(VK_CAPITAL) & 1) {
            modifiers |= EVENTFLAG_CAPS_LOCK_ON;
        }
        return modifiers;
    }

    int GetCefKeyboardModifiers(WPARAM wparam, LPARAM lparam) {
        int modifiers = 0;
        if (IsKeyDown(VK_SHIFT)) {
            modifiers |= EVENTFLAG_SHIFT_DOWN;
        }
        if (IsKeyDown(VK_CONTROL)) {
            modifiers |= EVENTFLAG_CONTROL_DOWN;
        }
        if (IsKeyDown(VK_MENU)) {
            modifiers |= EVENTFLAG_ALT_DOWN;
        }

        // Low bit set from GetKeyState indicates "toggled".
        if (::GetKeyState(VK_NUMLOCK) & 1) {
            modifiers |= EVENTFLAG_NUM_LOCK_ON;
        }
        if (::GetKeyState(VK_CAPITAL) & 1) {
            modifiers |= EVENTFLAG_CAPS_LOCK_ON;
        }

        switch (wparam) {
        case VK_RETURN:
            if ((lparam >> 16) & KF_EXTENDED) {
                modifiers |= EVENTFLAG_IS_KEY_PAD;
            }
            break;
        case VK_INSERT:
        case VK_DELETE:
        case VK_HOME:
        case VK_END:
        case VK_PRIOR:
        case VK_NEXT:
        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
            if (!((lparam >> 16) & KF_EXTENDED)) {
                modifiers |= EVENTFLAG_IS_KEY_PAD;
            }
            break;
        case VK_NUMLOCK:
        case VK_NUMPAD0:
        case VK_NUMPAD1:
        case VK_NUMPAD2:
        case VK_NUMPAD3:
        case VK_NUMPAD4:
        case VK_NUMPAD5:
        case VK_NUMPAD6:
        case VK_NUMPAD7:
        case VK_NUMPAD8:
        case VK_NUMPAD9:
        case VK_DIVIDE:
        case VK_MULTIPLY:
        case VK_SUBTRACT:
        case VK_ADD:
        case VK_DECIMAL:
        case VK_CLEAR:
            modifiers |= EVENTFLAG_IS_KEY_PAD;
            break;
        case VK_SHIFT:
            if (IsKeyDown(VK_LSHIFT)) {
                modifiers |= EVENTFLAG_IS_LEFT;
            }
            else if (IsKeyDown(VK_RSHIFT)) {
                modifiers |= EVENTFLAG_IS_RIGHT;
            }
            break;
        case VK_CONTROL:
            if (IsKeyDown(VK_LCONTROL)) {
                modifiers |= EVENTFLAG_IS_LEFT;
            }
            else if (IsKeyDown(VK_RCONTROL)) {
                modifiers |= EVENTFLAG_IS_RIGHT;
            }
            break;
        case VK_MENU:
            if (IsKeyDown(VK_LMENU)) {
                modifiers |= EVENTFLAG_IS_LEFT;
            }
            else if (IsKeyDown(VK_RMENU)) {
                modifiers |= EVENTFLAG_IS_RIGHT;
            }
            break;
        case VK_LWIN:
            modifiers |= EVENTFLAG_IS_LEFT;
            break;
        case VK_RWIN:
            modifiers |= EVENTFLAG_IS_RIGHT;
            break;
        }
        return modifiers;
    }

    bool IsKeyDown(WPARAM wparam) {
        return (GetKeyState(wparam) & 0x8000) != 0;
    }

    float GetDeviceScaleFactor() {
        static float scale_factor = 1.0;
        static bool initialized = false;

        if (!initialized) {
            // This value is safe to cache for the life time of the app since the user
            // must logout to change the DPI setting. This value also applies to all
            // screens.
            HDC screen_dc = ::GetDC(nullptr);
            int dpi_x = GetDeviceCaps(screen_dc, LOGPIXELSX);
            scale_factor = static_cast<float>(dpi_x) / 96.0f;
            ::ReleaseDC(nullptr, screen_dc);
            initialized = true;
        }

        return scale_factor;
    }

}