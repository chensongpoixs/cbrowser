/***********************************************************************************************
created: 		2022-01-20


author:			chensong


purpose:		assertion macros
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
//#include "crender_browser.h"　
#include <functional>
#include <sstream>
#include <thread>
#include <mutex>
#include "crender_window.h"
//#include <nlohmann/json.hpp>
//#include <obs-websocket-api.h>

#include "obs-browser-source.hpp"
#include "browser-scheme.hpp"
#include "browser-app.hpp"
#include "browser-version.h"
#include "cmediasoup_mgr.h"
#include "cef-headers.hpp"
#include "ComPtr.hpp"
#ifdef _WIN32
//#include <util/windows/ComPtr.hpp>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#else
#include "signal-restore.hpp"
#endif

//#ifdef ENABLE_WAYLAND
//#include <obs-nix-platform.h>
//#endif
//
//#ifdef ENABLE_BROWSER_QT_LOOP
//#include <QApplication>
//#include <QThread>
//#endif

//OBS_DECLARE_MODULE()
//OBS_MODULE_USE_DEFAULT_LOCALE("obs-browser", "en-US")
//MODULE_EXPORT const char *obs_module_description(void)
//{
//	return "CEF-based web browser source & panels";
//}
#if  10
//using namespace std;
#include <iostream>
#if defined(_WIN32)
static int adapterCount = 0;
#endif
static std::wstring deviceId;

bool hwaccel = true;



	static std::thread manager_thread;
	static bool manager_initialized = false;


	static BrowserSource* browser_source_ptr = NULL;

	static  std::string  g_work_app;
	static std::string	 g_browser_config_path;
	//os_event_t *cef_started_event = nullptr;

#

	/* ========================================================================= */

#ifdef ENABLE_BROWSER_QT_LOOP
	extern MessageObject messageObject;
#endif

	class BrowserTask : public CefTask {
	public:
		std::function<void()> task;

		inline BrowserTask(std::function<void()> task_) : task(task_) {}
		virtual void Execute() override
		{
#ifdef ENABLE_BROWSER_QT_LOOP
			/* you have to put the tasks on the Qt event queue after this
			 * call otherwise the CEF message pump may stop functioning
			 * correctly, it's only supposed to take 10ms max */
			QMetaObject::invokeMethod(&messageObject, "ExecuteTask", Qt::QueuedConnection,
				Q_ARG(MessageTask, task));
#else
			task();
#endif
		}

		IMPLEMENT_REFCOUNTING(BrowserTask);
	};

	bool QueueCEFTask(std::function<void()> task)
	{
		/*if (task)
		{
			  task();
		}
		return false;*/
		//printf("[%s][%d]\n", __FUNCTION__, __LINE__);
		return CefPostTask(TID_UI, CefRefPtr<BrowserTask>(new BrowserTask(task)));
	}

	/* ========================================================================= */

	static const char* default_css = "\
body { \
background-color: rgba(0, 0, 0, 0); \
margin: 0px auto; \
overflow: hidden; \
}";

	//static void browser_source_get_defaults(obs_data_t *settings)
	//{
	//	obs_data_set_default_string(settings, "url", "https://obsproject.com/browser-source");
	//	obs_data_set_default_int(settings, "width", 800);
	//	obs_data_set_default_int(settings, "height", 600);
	//	obs_data_set_default_int(settings, "fps", 30);
	//#ifdef ENABLE_BROWSER_SHARED_TEXTURE
	//	obs_data_set_default_bool(settings, "fps_custom", false);
	//#else
	//	obs_data_set_default_bool(settings, "fps_custom", true);
	//#endif
	//	obs_data_set_default_bool(settings, "shutdown", false);
	//	obs_data_set_default_bool(settings, "restart_when_active", false);
	//	obs_data_set_default_int(settings, "webpage_control_level", (int)DEFAULT_CONTROL_LEVEL);
	//	obs_data_set_default_string(settings, "css", default_css);
	//	obs_data_set_default_bool(settings, "reroute_audio", false);
	//}
	//
	//static bool is_local_file_modified(obs_properties_t *props, obs_property_t *, obs_data_t *settings)
	//{
	//	bool enabled = obs_data_get_bool(settings, "is_local_file");
	//	obs_property_t *url = obs_properties_get(props, "url");
	//	obs_property_t *local_file = obs_properties_get(props, "local_file");
	//	obs_property_set_visible(url, !enabled);
	//	obs_property_set_visible(local_file, enabled);
	//
	//	return true;
	//}
	//
	//static bool is_fps_custom(obs_properties_t *props, obs_property_t *, obs_data_t *settings)
	//{
	//	bool enabled = obs_data_get_bool(settings, "fps_custom");
	//	obs_property_t *fps = obs_properties_get(props, "fps");
	//	obs_property_set_visible(fps, enabled);
	//
	//	return true;
	//}
	//
	//static obs_properties_t *browser_source_get_properties(void *data)
	//{
	//	obs_properties_t *props = obs_properties_create();
	//	BrowserSource *bs = static_cast<BrowserSource *>(data);
	//	DStr path;
	//
	//	obs_properties_set_flags(props, OBS_PROPERTIES_DEFER_UPDATE);
	//	obs_property_t *prop = obs_properties_add_bool(props, "is_local_file", obs_module_text("LocalFile"));
	//
	//	if (bs && !bs->url.empty()) {
	//		const char *slash;
	//
	//		dstr_copy(path, bs->url.c_str());
	//		dstr_replace(path, "\\", "/");
	//		slash = strrchr(path->array, '/');
	//		if (slash)
	//			dstr_resize(path, slash - path->array + 1);
	//	}
	//
	//	obs_property_set_modified_callback(prop, is_local_file_modified);
	//	obs_properties_add_path(props, "local_file", obs_module_text("LocalFile"), OBS_PATH_FILE, "*.*", path->array);
	//	obs_properties_add_text(props, "url", obs_module_text("URL"), OBS_TEXT_DEFAULT);
	//
	//	obs_properties_add_int(props, "width", obs_module_text("Width"), 1, 8192, 1);
	//	obs_properties_add_int(props, "height", obs_module_text("Height"), 1, 8192, 1);
	//
	//	obs_properties_add_bool(props, "reroute_audio", obs_module_text("RerouteAudio"));
	//
	//	obs_property_t *fps_set = obs_properties_add_bool(props, "fps_custom", obs_module_text("CustomFrameRate"));
	//	obs_property_set_modified_callback(fps_set, is_fps_custom);
	//
	//#ifndef ENABLE_BROWSER_SHARED_TEXTURE
	//	obs_property_set_enabled(fps_set, false);
	//#endif
	//
	//	obs_properties_add_int(props, "fps", obs_module_text("FPS"), 1, 60, 1);
	//
	//	obs_property_t *p = obs_properties_add_text(props, "css", obs_module_text("CSS"), OBS_TEXT_MULTILINE);
	//	obs_property_text_set_monospace(p, true);
	//	obs_properties_add_bool(props, "shutdown", obs_module_text("ShutdownSourceNotVisible"));
	//	obs_properties_add_bool(props, "restart_when_active", obs_module_text("RefreshBrowserActive"));
	//
	//	obs_property_t *controlLevel = obs_properties_add_list(props, "webpage_control_level",
	//							       obs_module_text("WebpageControlLevel"),
	//							       OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);
	//
	//	obs_property_list_add_int(controlLevel, obs_module_text("WebpageControlLevel.Level.None"),
	//				  (int)ControlLevel::None);
	//	obs_property_list_add_int(controlLevel, obs_module_text("WebpageControlLevel.Level.ReadObs"),
	//				  (int)ControlLevel::ReadObs);
	//	obs_property_list_add_int(controlLevel, obs_module_text("WebpageControlLevel.Level.ReadUser"),
	//				  (int)ControlLevel::ReadUser);
	//	obs_property_list_add_int(controlLevel, obs_module_text("WebpageControlLevel.Level.Basic"),
	//				  (int)ControlLevel::Basic);
	//	obs_property_list_add_int(controlLevel, obs_module_text("WebpageControlLevel.Level.Advanced"),
	//				  (int)ControlLevel::Advanced);
	//	obs_property_list_add_int(controlLevel, obs_module_text("WebpageControlLevel.Level.All"),
	//				  (int)ControlLevel::All);
	//
	//	obs_properties_add_button(props, "refreshnocache", obs_module_text("RefreshNoCache"),
	//				  [](obs_properties_t *, obs_property_t *, void *data) {
	//					  static_cast<BrowserSource *>(data)->Refresh();
	//					  return false;
	//				  });
	//	return props;
	//}

	//static void missing_file_callback(void *src, const char *new_path, void *data)
	//{
	//	BrowserSource *bs = static_cast<BrowserSource *>(src);
	//
	//	if (bs) {
	//		obs_source_t *source = bs->source;
	//		OBSDataAutoRelease settings = obs_source_get_settings(source);
	//		obs_data_set_string(settings, "local_file", new_path);
	//		obs_source_update(source, settings);
	//	}
	//
	//	UNUSED_PARAMETER(data);
	//}
	//
	//static obs_missing_files_t *browser_source_missingfiles(void *data)
	//{
	//	BrowserSource *bs = static_cast<BrowserSource *>(data);
	//	obs_missing_files_t *files = obs_missing_files_create();
	//
	//	if (bs) {
	//		obs_source_t *source = bs->source;
	//		OBSDataAutoRelease settings = obs_source_get_settings(source);
	//
	//		bool enabled = obs_data_get_bool(settings, "is_local_file");
	//		const char *path = obs_data_get_string(settings, "local_file");
	//
	//		if (enabled && strcmp(path, "") != 0) {
	//			if (!os_file_exists(path)) {
	//				obs_missing_file_t *file = obs_missing_file_create(
	//					path, missing_file_callback, OBS_MISSING_FILE_SOURCE, bs->source, NULL);
	//
	//				obs_missing_files_add_file(files, file);
	//			}
	//		}
	//	}
	//
	//	return files;
	//}

	static CefRefPtr<BrowserApp> app;

	static void BrowserInit(void)
	{
		//std::string path = "D:\\Work\\cbrowser\\cbrowser";//obs_get_module_binary_path(obs_current_module());
		//path = path.substr(0, path.find_last_of('/') + 1);
		//path += "//obs-browser-page";
		//path += "//cbrowser_render";
		std::string path = g_work_app;
#ifdef _WIN32
		path += ".exe";
		CefMainArgs args;
#else
		/* On non-windows platforms, ie macOS, we'll want to pass thru flags to
		 * CEF */
		struct obs_cmdline_args cmdline_args = obs_get_cmdline_args();
		CefMainArgs args(cmdline_args.argc, cmdline_args.argv);
#endif

		//BPtr<char> conf_path = obs_module_config_path("");
		//os_mkdir(conf_path);

		CefSettings settings;
		settings.log_severity = LOGSEVERITY_DISABLE;
		//BPtr<char> log_path = obs_module_config_path("debug.log");
		//BPtr<char> log_path_abs = os_get_abs_path_ptr(log_path);
		// g_browser_config_path + "/debug.log"; //
		CefString(&settings.log_file) = g_browser_config_path + "\\debug.log";
		settings.windowless_rendering_enabled = true;
		settings.no_sandbox = false;

		uint32_t obs_ver = 33;//obs_get_version();
		uint32_t obs_maj = obs_ver >> 24;
		uint32_t obs_min = (obs_ver >> 16) & 0xFF;
		uint32_t obs_pat = obs_ver & 0xFFFF;

		/* This allows servers the ability to determine that browser panels and
		 * browser sources are coming from OBS. */
		std::stringstream prod_ver;
		prod_ver << "Chrome/";
		prod_ver << std::to_string(cef_version_info(4)) << "." << std::to_string(cef_version_info(5)) << "."
			<< std::to_string(cef_version_info(6)) << "." << std::to_string(cef_version_info(7));
		prod_ver << " OBS/";
		prod_ver << std::to_string(obs_maj) << "." << std::to_string(obs_min) << "." << std::to_string(obs_pat);

#if CHROME_VERSION_BUILD >= 4472
		CefString(&settings.user_agent_product) = prod_ver.str();
#else
		CefString(&settings.product_version) = prod_ver.str();
#endif

#ifdef ENABLE_BROWSER_QT_LOOP
		settings.external_message_pump = true;
		settings.multi_threaded_message_loop = false;
#endif
		//	settings.multi_threaded_message_loop = false;
#if !defined(_WIN32) && !defined(__APPLE__)
	// Override locale path from OBS binary path to plugin binary path
		string locales = obs_get_module_binary_path(obs_current_module());
		locales = locales.substr(0, locales.find_last_of('/') + 1);
		locales += "locales";
		BPtr<char> abs_locales = os_get_abs_path_ptr(locales.c_str());
		CefString(&settings.locales_dir_path) = abs_locales;
#endif

		std::string obs_locale = "zh-CN";//obs_get_locale();
		std::string accepted_languages;
		if (obs_locale != "en-US") {
			accepted_languages = obs_locale;
			accepted_languages += ",";
			accepted_languages += "en-US,en";
		}
		else {
			accepted_languages = "en-US,en";
		}

		//BPtr<char> conf_path_abs = os_get_abs_path_ptr(conf_path);
		CefString(&settings.locale) = obs_locale;//obs_get_locale();
		CefString(&settings.accept_language_list) = accepted_languages;
#if CHROME_VERSION_BUILD <= 6533
		settings.persist_user_preferences = 1;
#endif
		CefString(&settings.cache_path) = g_browser_config_path;//"D:\\Work\\cbrowser\\cbrowser\\browser_config\\";
		//CefString(&settings.cache_path) = "C:\\Users\\Administrator\\AppData\\Roaming\\obs-studio\\plugin_config\\obs-browser";
#if !defined(__APPLE__) || defined(ENABLE_BROWSER_LEGACY)
	//char *abs_path = os_get_abs_path_ptr(path.c_str());
		CefString(&settings.browser_subprocess_path) = path.c_str();
		//bfree(abs_path);
#endif

		bool tex_sharing_avail = false;

#ifdef ENABLE_BROWSER_SHARED_TEXTURE
		if (hwaccel) {
			//obs_enter_graphics();
			hwaccel = tex_sharing_avail = true;// gs_shared_texture_available();
			//obs_leave_graphics();
		}
#endif

#if defined(__APPLE__) || defined(_WIN32) || !defined(ENABLE_WAYLAND)
		app = new BrowserApp(tex_sharing_avail);
#else
		app = new BrowserApp(tex_sharing_avail, obs_get_nix_platform() == OBS_NIX_PLATFORM_WAYLAND);
#endif

#ifdef _WIN32
		CefExecuteProcess(args, app, nullptr);
#endif

#if !defined(_WIN32)
		BackupSignalHandlers();
		CefInitialize(args, settings, app, nullptr);
		RestoreSignalHandlers();
#elif (CHROME_VERSION_BUILD > 3770)
		bool ret = CefInitialize(args, settings, app, nullptr);
		printf("CefInitialize = %u\n", ret);
#else
		/* Massive (but amazing) hack to prevent chromium from modifying our
		 * process tokens and permissions, which caused us problems with winrt,
		 * used with window capture.  Note, the structure internally is just
		 * two pointers normally.  If it causes problems with future versions
		 * we'll just switch back to the static library but I doubt we'll need
		 * to. */
		uintptr_t zeroed_memory_lol[32] = {};
		CefInitialize(args, settings, app, zeroed_memory_lol);
#endif

#if !ENABLE_LOCAL_FILE_URL_SCHEME
		/* Register http://absolute/ scheme handler for older
		 * CEF builds which do not support file:// URLs */
		CefRegisterSchemeHandlerFactory("http", "absolute", new BrowserSchemeHandlerFactory());
#endif
		//os_event_signal(cef_started_event);
	}

	static void BrowserShutdown(void)
	{
#if !ENABLE_LOCAL_FILE_URL_SCHEME
		CefClearSchemeHandlerFactories();
#endif
#ifdef ENABLE_BROWSER_QT_LOOP
		while (messageObject.ExecuteNextBrowserTask())
			;
		CefDoMessageLoopWork();
#endif
		CefShutdown();
		app = nullptr;
	}

#ifndef ENABLE_BROWSER_QT_LOOP
	static void BrowserManagerThread(void)
	{
		BrowserInit();
		CefRunMessageLoop();
		BrowserShutdown();
	}
#endif

	/*extern "C" EXPORT*/ void obs_browser_initialize(void)
	{
		//if (!os_atomic_set_bool(&manager_initialized, true)) 
		{
#ifdef ENABLE_BROWSER_QT_LOOP
			BrowserInit();
#else
			manager_thread = std::thread(BrowserManagerThread);
#endif
		}
	}

	void RegisterBrowserSource()
	{
		//	struct obs_source_info info = {};
		//	info.id = "browser_source";
		//	info.type = OBS_SOURCE_TYPE_INPUT;
		//	info.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_AUDIO | OBS_SOURCE_CUSTOM_DRAW | OBS_SOURCE_INTERACTION |
		//			    OBS_SOURCE_DO_NOT_DUPLICATE | OBS_SOURCE_SRGB;
		//	info.get_properties = browser_source_get_properties;
		//	info.get_defaults = browser_source_get_defaults;
		//	info.icon_type = OBS_ICON_TYPE_BROWSER;
		//
		//	info.get_name = [](void *) {
		//		return obs_module_text("BrowserSource");
		//	};
		//	info.create = [](/*obs_data_t *settings, obs_source_t *source*/) -> void * {
		//		obs_browser_initialize();
		//		return new BrowserSource(/*settings, source*/);
		//	};
		//	info.destroy = [](void *data) {
		//		static_cast<BrowserSource *>(data)->Destroy();
		//	};
		//	info.missing_files = browser_source_missingfiles;
		//	//info.update = [](void *data /*, obs_data_t *settings*/) {
		//	//	static_cast<BrowserSource *>(data)->Update( ""/*settings*/);
		//	//};
		//	info.get_width = [](void *data) {
		//		return (uint32_t) static_cast<BrowserSource *>(data)->width;
		//	};
		//	info.get_height = [](void *data) {
		//		return (uint32_t) static_cast<BrowserSource *>(data)->height;
		//	};
		//	info.video_tick = [](void *data, float) {
		//		static_cast<BrowserSource *>(data)->Tick();
		//	};
		//	info.video_render = [](void *data/*, gs_effect_t **/) {
		//		static_cast<BrowserSource *>(data)->Render();
		//	};
		//#if CHROME_VERSION_BUILD < 4103
		//	info.audio_mix = [](void *data, uint64_t *ts_out, struct audio_output_data *audio_output, size_t channels,
		//			    size_t sample_rate) {
		//		return static_cast<BrowserSource *>(data)->AudioMix(ts_out, audio_output, channels, sample_rate);
		//	};
		//	info.enum_active_sources = [](void *data, obs_source_enum_proc_t cb, void *param) {
		//		static_cast<BrowserSource *>(data)->EnumAudioStreams(cb, param);
		//	};
		//#endif
		//	info.mouse_click = [](void *data, const struct obs_mouse_event *event, int32_t type, bool mouse_up,
		//			      uint32_t click_count) {
		//		static_cast<BrowserSource *>(data)->SendMouseClick(event, type, mouse_up, click_count);
		//	};
		//	info.mouse_move = [](void *data, const struct obs_mouse_event *event, bool mouse_leave) {
		//		static_cast<BrowserSource *>(data)->SendMouseMove(event, mouse_leave);
		//	};
		//	info.mouse_wheel = [](void *data, const struct obs_mouse_event *event, int x_delta, int y_delta) {
		//		static_cast<BrowserSource *>(data)->SendMouseWheel(event, x_delta, y_delta);
		//	};
		//	info.focus = [](void *data, bool focus) {
		//		static_cast<BrowserSource *>(data)->SendFocus(focus);
		//	};
		//	info.key_click = [](void *data, const struct obs_key_event *event, bool key_up) {
		//		static_cast<BrowserSource *>(data)->SendKeyClick(event, key_up);
		//	};
		//	info.show = [](void *data) {
		//		static_cast<BrowserSource *>(data)->SetShowing(true);
		//	};
		//	info.hide = [](void *data) {
		//		static_cast<BrowserSource *>(data)->SetShowing(false);
		//	};
		//	info.activate = [](void *data) {
		//		BrowserSource *bs = static_cast<BrowserSource *>(data);
		//		if (bs->restart)
		//			bs->Refresh();
		//		bs->SetActive(true);
		//	};
		//	info.deactivate = [](void *data) {
		//		static_cast<BrowserSource *>(data)->SetActive(false);
		//	};

			//obs_register_source(&info);
	}

	/* ========================================================================= */

	extern void DispatchJSEvent(std::string eventName, std::string jsonString, BrowserSource* browser = nullptr);

	//static void handle_obs_frontend_event(enum obs_frontend_event event, void *)
	//{
	//	switch (event) {
	//	case OBS_FRONTEND_EVENT_STREAMING_STARTING:
	//		DispatchJSEvent("obsStreamingStarting", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_STREAMING_STARTED:
	//		DispatchJSEvent("obsStreamingStarted", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_STREAMING_STOPPING:
	//		DispatchJSEvent("obsStreamingStopping", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_STREAMING_STOPPED:
	//		DispatchJSEvent("obsStreamingStopped", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_RECORDING_STARTING:
	//		DispatchJSEvent("obsRecordingStarting", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_RECORDING_STARTED:
	//		DispatchJSEvent("obsRecordingStarted", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_RECORDING_PAUSED:
	//		DispatchJSEvent("obsRecordingPaused", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_RECORDING_UNPAUSED:
	//		DispatchJSEvent("obsRecordingUnpaused", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_RECORDING_STOPPING:
	//		DispatchJSEvent("obsRecordingStopping", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_RECORDING_STOPPED:
	//		DispatchJSEvent("obsRecordingStopped", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STARTING:
	//		DispatchJSEvent("obsReplaybufferStarting", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STARTED:
	//		DispatchJSEvent("obsReplaybufferStarted", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_REPLAY_BUFFER_SAVED:
	//		DispatchJSEvent("obsReplaybufferSaved", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STOPPING:
	//		DispatchJSEvent("obsReplaybufferStopping", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STOPPED:
	//		DispatchJSEvent("obsReplaybufferStopped", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_VIRTUALCAM_STARTED:
	//		DispatchJSEvent("obsVirtualcamStarted", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_VIRTUALCAM_STOPPED:
	//		DispatchJSEvent("obsVirtualcamStopped", "null");
	//		break;
	//	case OBS_FRONTEND_EVENT_SCENE_CHANGED: {
	//		OBSSourceAutoRelease source = obs_frontend_get_current_scene();
	//
	//		if (!source)
	//			break;
	//
	//		const char *name = obs_source_get_name(source);
	//		if (!name)
	//			break;
	//
	//		nlohmann::json json = {{"name", name},
	//				       {"width", obs_source_get_width(source)},
	//				       {"height", obs_source_get_height(source)}};
	//
	//		DispatchJSEvent("obsSceneChanged", json.dump());
	//		break;
	//	}
	//	case OBS_FRONTEND_EVENT_SCENE_LIST_CHANGED: {
	//		struct obs_frontend_source_list list = {};
	//		obs_frontend_get_scenes(&list);
	//		std::vector<const char *> scenes_vector;
	//		for (size_t i = 0; i < list.sources.num; i++) {
	//			obs_source_t *source = list.sources.array[i];
	//			scenes_vector.push_back(obs_source_get_name(source));
	//		}
	//		nlohmann::json json = scenes_vector;
	//		obs_frontend_source_list_free(&list);
	//
	//		DispatchJSEvent("obsSceneListChanged", json.dump());
	//		break;
	//	}
	//	case OBS_FRONTEND_EVENT_TRANSITION_CHANGED: {
	//		OBSSourceAutoRelease source = obs_frontend_get_current_transition();
	//
	//		if (!source)
	//			break;
	//
	//		const char *name = obs_source_get_name(source);
	//		if (!name)
	//			break;
	//
	//		nlohmann::json json = {{"name", name}};
	//
	//		DispatchJSEvent("obsTransitionChanged", json.dump());
	//		break;
	//	}
	//	case OBS_FRONTEND_EVENT_TRANSITION_LIST_CHANGED: {
	//		struct obs_frontend_source_list list = {};
	//		obs_frontend_get_transitions(&list);
	//		std::vector<const char *> transitions_vector;
	//		for (size_t i = 0; i < list.sources.num; i++) {
	//			obs_source_t *source = list.sources.array[i];
	//			transitions_vector.push_back(obs_source_get_name(source));
	//		}
	//		nlohmann::json json = transitions_vector;
	//		obs_frontend_source_list_free(&list);
	//
	//		DispatchJSEvent("obsTransitionListChanged", json.dump());
	//		break;
	//	}
	//	case OBS_FRONTEND_EVENT_EXIT:
	//		DispatchJSEvent("obsExit", "null");
	//		break;
	//	default:;
	//	}
	//}

#ifdef _WIN32
	static inline void EnumAdapterCount()
	{
		ComPtr<IDXGIFactory1> factory;
		ComPtr<IDXGIAdapter1> adapter;
		HRESULT hr;
		UINT i = 0;

		hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
		if (FAILED(hr))
			return;

		while (factory->EnumAdapters1(i++, &adapter) == S_OK) {
			DXGI_ADAPTER_DESC desc;

			hr = adapter->GetDesc(&desc);
			if (FAILED(hr))
				continue;

			if (i == 1)
				deviceId = desc.Description;

			/* ignore Microsoft's 'basic' renderer' */
			if (desc.VendorId == 0x1414 && desc.DeviceId == 0x8c)
				continue;

			adapterCount++;
		}
	}
#endif

#ifdef ENABLE_BROWSER_SHARED_TEXTURE
#ifdef _WIN32
	static const wchar_t* blacklisted_devices[] = { L"Intel", L"Microsoft", L"Radeon HD 8850M", L"Radeon HD 7660", nullptr };

	static inline bool is_intel(const std::wstring& str)
	{
		return str != L"Intel";
		//return wstrstri(str.c_str(), L"Intel") != 0;
	}

	static void check_hwaccel_support(void)
	{
		/* do not use hardware acceleration if a blacklisted device is the
		 * default and on 2 or more adapters */
		const wchar_t** device = blacklisted_devices;

		if (adapterCount >= 2 || !is_intel(deviceId)) {
			while (*device) {
				//if (!!wstrstri(deviceId.c_str(), *device))
				if (deviceId != *device)
				{
					hwaccel = true;
					printf("\n[obs-browser]: "
						"Blacklisted device "
						"detected, "
						"disabling browser "
						"source hardware "
						"acceleration. [deviceId = %ws]\n", deviceId.c_str());
					break;
				}
				device++;
			}
		}
	}
#else
	static void check_hwaccel_support(void)
	{
		return;
	}
#endif
#endif

	bool obs_module_load(void)
	{
#ifdef ENABLE_BROWSER_QT_LOOP
		qRegisterMetaType<MessageTask>("MessageTask");
#endif

		//os_event_init(&cef_started_event, OS_EVENT_TYPE_MANUAL);

#if defined(_WIN32) && CHROME_VERSION_BUILD < 5615
	/* CefEnableHighDPISupport doesn't do anything on OS other than Windows. Would also crash macOS at this point as CEF is not directly linked */
		CefEnableHighDPISupport();
#endif

#ifdef _WIN32
		EnumAdapterCount();
#else
#if defined(__APPLE__) && !defined(ENABLE_BROWSER_LEGACY)
		/* Load CEF at runtime as required on macOS */
		CefScopedLibraryLoader library_loader;
		if (!library_loader.LoadInMain())
			return false;
#endif
#endif
		printf("[obs-browser]: Version %s", OBS_BROWSER_VERSION_STRING);
		printf("[obs-browser]: CEF Version %i.%i.%i.%i (runtime), %s (compiled)", cef_version_info(4),
			cef_version_info(5), cef_version_info(6), cef_version_info(7), CEF_VERSION);

		// init
		RegisterBrowserSource();
		//obs_frontend_add_event_callback(handle_obs_frontend_event, nullptr);

#ifdef ENABLE_BROWSER_SHARED_TEXTURE
	//OBSDataAutoRelease private_data = obs_get_private_data();
	//hwaccel = obs_data_get_bool(private_data, "BrowserHWAccel");
		hwaccel = true;
		if (hwaccel) {
			check_hwaccel_support();
		}
#endif

#if defined(__APPLE__) && CHROME_VERSION_BUILD < 4183
		// Make sure CEF malloc hijacking happens early in the process
		obs_browser_initialize();
#endif

		return true;
	}

	void obs_module_post_load(void)
	{
#if 0
		auto vendor = obs_websocket_register_vendor("obs-browser");
		if (!vendor)
			return;
		auto emit_event_request_cb = [](obs_data_t* request_data, obs_data_t*, void*) {
			const char* event_name = obs_data_get_string(request_data, "event_name");
			if (!event_name)
				return;

			OBSDataAutoRelease event_data = obs_data_get_obj(request_data, "event_data");
			const char* event_data_string = event_data ? obs_data_get_json(event_data) : "{}";

			DispatchJSEvent(event_name, event_data_string, nullptr);
			};

		if (!obs_websocket_vendor_register_request(vendor, "emit_event", emit_event_request_cb, nullptr))
			blog(LOG_WARNING, "[obs-browser]: Failed to register obs-websocket request emit_event");
#endif 

	}

	void obs_module_unload(void)
	{
#ifdef ENABLE_BROWSER_QT_LOOP
		BrowserShutdown();
#else
		if (manager_thread.joinable()) {
			while (!QueueCEFTask([]() { CefQuitMessageLoop(); }))
				Sleep(5);

			manager_thread.join();
		}
#endif

		//os_event_destroy(cef_started_event);
	}



	//std::function<void(FEvent cevent)>

	/*static void input_device_event(chen::FEvent cevent)
	{
		printf("[%s][%d][type = %u]\n", __FUNCTION__, __LINE__, cevent.Event);
	}*/


 

	 namespace chen {

	void input_device_event(chen::FEvent cevent)
	{

		//printf("[%s][%d][type = %u]\n", __FUNCTION__, __LINE__, cevent.Event);
		switch (cevent.Event)
		{
		case chen::EventType::KEY_DOWN:
		{

			if (browser_source_ptr)
			{
				obs_key_event event;
				event.modifiers = KEYEVENT_RAWKEYDOWN;
				event.native_vkey = cevent.Data.KeyDown.KeyCode;//cevent.GetKeyDown();
				event.native_scancode = cevent.Data.KeyDown.bIsRepeat;
				//event.y = 200;d
				browser_source_ptr->SendKeyClick(&event, 0);
			}
			break;
		}
		case chen::EventType::KEY_PRESS:
		{
			if (browser_source_ptr)
			{
				obs_key_event event;
				event.modifiers = KEYEVENT_CHAR;
				event.native_vkey = cevent.Data.Character.Character;//cevent.GetKeyDown();
				//event.y = 200;
				browser_source_ptr->SendKeyClick(&event, 1);
			}
			break;
		}
		case chen::EventType::KEY_UP:
		{
			if (browser_source_ptr)
			{
				obs_key_event event;
				event.modifiers = KEYEVENT_KEYUP;
				event.native_vkey = cevent.Data.KeyUp.KeyCode;//cevent.GetKeyDown();
				//event.native_scancode = cevent.Data.KeyUp.KeyCode;
				//event.y = 200;d
				browser_source_ptr->SendKeyClick(&event, 2);
			}
			break;
		}

		case chen::EventType::MOUSE_ENTER:
		{
			break;
		}
		case chen::EventType::MOUSE_LEAVE:
		{
			break;
		}
		case chen::EventType::MOUSE_MOVE:
		{
			if (browser_source_ptr)
			{
				/*
				typedef enum {
  EVENTFLAG_NONE = 0,
  EVENTFLAG_CAPS_LOCK_ON = 1 << 0,
  EVENTFLAG_SHIFT_DOWN = 1 << 1,
  EVENTFLAG_CONTROL_DOWN = 1 << 2,
  EVENTFLAG_ALT_DOWN = 1 << 3,
  EVENTFLAG_LEFT_MOUSE_BUTTON = 1 << 4,
  EVENTFLAG_MIDDLE_MOUSE_BUTTON = 1 << 5,
  EVENTFLAG_RIGHT_MOUSE_BUTTON = 1 << 6,
  /// Mac OS-X command key.
  EVENTFLAG_COMMAND_DOWN = 1 << 7,
  EVENTFLAG_NUM_LOCK_ON = 1 << 8,
  EVENTFLAG_IS_KEY_PAD = 1 << 9,
  EVENTFLAG_IS_LEFT = 1 << 10,
  EVENTFLAG_IS_RIGHT = 1 << 11,
  EVENTFLAG_ALTGR_DOWN = 1 << 12,
  EVENTFLAG_IS_REPEAT = 1 << 13,
} cef_event_flags_t;
				*/
				obs_mouse_event event;
				event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
				event.x = cevent.Data.MouseMove.PosX;
				event.y = cevent.Data.MouseMove.PosY;
				//printf("[x = %u][y = %u]\n", event.x, event.y);
				//event.native_vkey = cevent.Data.MouseMove.PosX;//cevent.GetKeyDown();
				//event.y = 200;
				browser_source_ptr->SendMouseMove(&event, false);
			}
			break;
		}
		case chen::EventType::MOUSE_DOWN:
		{
			if (browser_source_ptr)
			{

				obs_mouse_event event;
				uint32_t type = 0;
				/*
				 MBT_LEFT = 0,
  MBT_MIDDLE,
  MBT_RIGHT,
				*/
				if (cevent.Data.MouseButton.Button == 0)
				{
					event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
					type = MBT_LEFT;
				}
				else if (cevent.Data.MouseButton.Button == 1)
				{
					event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
					type = MBT_MIDDLE;
				}
				else
				{
					event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
					type = MBT_RIGHT;
				}

				event.x = cevent.Data.MouseButton.PosX;
				event.y = cevent.Data.MouseButton.PosY;
				//event.native_vkey = cevent.Data.MouseMove.PosX;//cevent.GetKeyDown();
				//event.y = 200;
				browser_source_ptr->SendMouseClick(&event, type, false, 1);
			}
			break;
		}
		case chen::EventType::MOUSE_UP:
		{
			if (browser_source_ptr)
			{

				obs_mouse_event event;
				uint32_t type = 0;
				/*
				 MBT_LEFT = 0,
  MBT_MIDDLE,
  MBT_RIGHT,
				*/
				if (cevent.Data.MouseButton.Button == 0)
				{
					event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
					type = MBT_LEFT;
				}
				else if (cevent.Data.MouseButton.Button == 1)
				{
					event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
					type = MBT_MIDDLE;
				}
				else
				{
					event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
					type = MBT_RIGHT;
				}

				event.x = cevent.Data.MouseButton.PosX;
				event.y = cevent.Data.MouseButton.PosY;
				//event.native_vkey = cevent.Data.MouseMove.PosX;//cevent.GetKeyDown();
				//event.y = 200;
				browser_source_ptr->SendMouseClick(&event, type, true, 1);
			}
			break;
		}
		case chen::EventType::MOUSE_WHEEL:
		{
			if (browser_source_ptr)
			{
				/*
				typedef enum {
  EVENTFLAG_NONE = 0,
  EVENTFLAG_CAPS_LOCK_ON = 1 << 0,
  EVENTFLAG_SHIFT_DOWN = 1 << 1,
  EVENTFLAG_CONTROL_DOWN = 1 << 2,
  EVENTFLAG_ALT_DOWN = 1 << 3,
  EVENTFLAG_LEFT_MOUSE_BUTTON = 1 << 4,
  EVENTFLAG_MIDDLE_MOUSE_BUTTON = 1 << 5,
  EVENTFLAG_RIGHT_MOUSE_BUTTON = 1 << 6,
  /// Mac OS-X command key.
  EVENTFLAG_COMMAND_DOWN = 1 << 7,
  EVENTFLAG_NUM_LOCK_ON = 1 << 8,
  EVENTFLAG_IS_KEY_PAD = 1 << 9,
  EVENTFLAG_IS_LEFT = 1 << 10,
  EVENTFLAG_IS_RIGHT = 1 << 11,
  EVENTFLAG_ALTGR_DOWN = 1 << 12,
  EVENTFLAG_IS_REPEAT = 1 << 13,
} cef_event_flags_t;
				*/
				obs_mouse_event event;
				event.modifiers = EVENTFLAG_MIDDLE_MOUSE_BUTTON;
				event.x = cevent.Data.MouseWheel.PosX;
				event.y = cevent.Data.MouseWheel.PosY;
				//event.native_vkey = cevent.Data.MouseMove.PosX;//cevent.GetKeyDown();
				//event.y = 200;
				browser_source_ptr->SendMouseWheel(&event, (int)cevent.Data.MouseWheel.Delta, (int)cevent.Data.MouseWheel.Delta);
			}
			break;
		}
		/*case chen::EventType::MOUSE_ENTER:
		{
			break;
		}*/
		default:
			break;
		}
	}

	void browser_init(const char* work_app, const char* browser_config_path)
	{

		g_work_app = work_app;
		g_browser_config_path = browser_config_path;
	/*	obs_module_load();

		
		obs_browser_initialize();*/


		obs_module_load();


		obs_browser_initialize();
		//Sleep(100);
		//chen::g_rtc_mgr.init(0);



		//chen::render_window_ptr.init(1920, 1080);


		//// chen::g_rtc_mgr.startup("192.168.1.26", 8888, "chensong", "chensong");
		////input_device_d d;
		//browser_source_ptr = new BrowserSource();



		//browser_source_ptr->width = 1920;
		//browser_source_ptr->height = 1080;
		//browser_source_ptr->canvas_fps = 30;
		//browser_source_ptr->is_showing = true;
		//printf("[%s][%d]\n", __FUNCTION__, __LINE__);
		//browser_source_ptr->url = "http://www.baidu.com";
		//browser_source_ptr->CreateBrowser();
		//Sleep(1);
		//browser_source_ptr->SetActive(true);
		//browser_source_ptr->Refresh();
		//browser_source_ptr->SetShowing(true);
		//Sleep(100);
		//chen::g_rtc_mgr.init(0);



		//chen::render_window_ptr.init(1920, 1080);


		//chen::g_rtc_mgr.startup("192.168.1.26", 8888, "chensong", "chensong");
		
	}
	void browser_startup(const char* url, uint32_t width, uint32_t height, uint32_t fps, set_gpu_addresses_callback callback, bool show)
	{
	//	input_device_d d;
		//Sleep(100);
		chen::g_gpu_addresses_callback_ptr = callback;
		chen::render_window_ptr.init(width, height, show);
		
		
		browser_source_ptr = new BrowserSource();


		
		browser_source_ptr->width = width;
		browser_source_ptr->height = height;
		browser_source_ptr->canvas_fps = fps;
		browser_source_ptr->is_showing = true;
		printf("[%s][%d]\n", __FUNCTION__, __LINE__);
		browser_source_ptr->url = url;
		browser_source_ptr->CreateBrowser();
		//	Sleep(10);
		browser_source_ptr->SetActive(true);
		browser_source_ptr->Refresh();
		browser_source_ptr->SetShowing(true);
		/*std::thread([=]() {
			while (true)
			{
				browser_source_ptr->Refresh();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			}).detach();*/
	}
}

#endif // 

    