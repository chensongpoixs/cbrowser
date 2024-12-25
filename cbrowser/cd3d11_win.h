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
#ifndef _C_D3D11_WIN_H_
#define _C_D3D11_WIN_H_
//#include "cconfig.h"
//#include "cconfig.h"
//#include "csingleton.h"
#include <iostream>
#include <d3d11_1.h>
#include <memory>
#include <string>
#include <vector>
#include "include/base/cef_weak_ptr.h"
#include "include/cef_render_handler.h"
#include "include/base/cef_macros.h"
                    
 



namespace chen {


    //uint64_t GetTimeNow();
    namespace d3d11
    {

        class Composition;
        class Context;
        class Effect;
        class Geometry;
        class SwapChain;
        class Texture2D;

        // Basic rect for floats.
        struct Rect {
            float x;
            float y;
            float width;
            float height;
        };

        template <class T>
        class ScopedBinder {
        public:
            ScopedBinder(const std::shared_ptr<Context>& ctx,
                const std::shared_ptr<T>& target)
                : target_(target) {
                if (target_) {
                    target_->bind(ctx);
                }
            }
            ~ScopedBinder() {
                if (target_) {
                    target_->unbind();
                }
            }

        private:
            const std::shared_ptr<T> target_;

            DISALLOW_COPY_AND_ASSIGN(ScopedBinder);
        };

        class Context {
        public:
            explicit Context(ID3D11DeviceContext*);

            void flush();

            explicit operator ID3D11DeviceContext* () { return ctx_.get(); }

       // private:
            const std::shared_ptr<ID3D11DeviceContext> ctx_;
        };

        // Encapsulate a D3D11 Device object.
        class Device {
        public:
            Device(ID3D11Device*, ID3D11DeviceContext*);

            static std::shared_ptr<Device> create();

            std::string adapter_name() const;

            explicit operator ID3D11Device* () { return device_.get(); }

            std::shared_ptr<Context> immedidate_context();

            std::shared_ptr<SwapChain> create_swapchain(HWND,
                int width = 0,
                int height = 0);

            std::shared_ptr<Geometry> create_quad(float x,
                float y,
                float width,
                float height,
                bool flip = false);



            bool   create_shared_texture(DXGI_FORMAT format, uint32_t width, uint32_t height);



            void  copy_shared_texture(ID3D11Texture2D* src);


            std::shared_ptr<Texture2D> create_texture(int width,
                int height,
                DXGI_FORMAT format,
                const void* data,
                size_t row_stride);

            std::shared_ptr<Texture2D> open_shared_texture(void*);

            // Create some basic shaders so we can draw a textured-quad.
            std::shared_ptr<Effect> create_default_effect();

            std::shared_ptr<Effect> create_effect(const std::string& vertex_code,
                const std::string& vertex_entry,
                const std::string& vertex_model,
                const std::string& pixel_code,
                const std::string& pixel_entry,
                const std::string& pixel_model);
            HANDLE pSharedHandle;
        private:
            std::shared_ptr<ID3DBlob> compile_shader(const std::string& source_code,
                const std::string& entry_point,
                const std::string& model);

            HMODULE lib_compiler_;

            const std::shared_ptr<ID3D11Device> device_;
            const std::shared_ptr<Context> ctx_;

            ID3D11Texture2D* shared_texture = nullptr;
           

            DISALLOW_COPY_AND_ASSIGN(Device);
        };

        // Encapsulate a DXGI swapchain for a window.
        class SwapChain {
        public:
            SwapChain(IDXGISwapChain*,
                ID3D11RenderTargetView*,
                ID3D11SamplerState*,
                ID3D11BlendState*);

            void bind(const std::shared_ptr<Context>& ctx);
            void unbind();

            void clear(float red, float green, float blue, float alpha);

            void present(int sync_interval);
            void resize(int width, int height);

            int width() const { return width_; }
            int height() const { return height_; }

        private:
            const std::shared_ptr<ID3D11SamplerState> sampler_;
            const std::shared_ptr<ID3D11BlendState> blender_;
            const std::shared_ptr<IDXGISwapChain> swapchain_;
            std::shared_ptr<ID3D11RenderTargetView> rtv_;
            std::shared_ptr<Context> ctx_;
            int width_ = 0;
            int height_ = 0;

            DISALLOW_COPY_AND_ASSIGN(SwapChain);
        };

        class Texture2D {
        public:
            Texture2D(ID3D11Texture2D* tex, ID3D11ShaderResourceView* srv);

            void bind(std::shared_ptr<Context> const& ctx);
            void unbind();

            uint32_t width() const;
            uint32_t height() const;
            DXGI_FORMAT format() const;

            bool has_mutex() const;

            bool lock_key(uint64_t key, uint32_t timeout_ms);
            void unlock_key(uint64_t key);

            void* share_handle() const;

            void copy_from(const std::shared_ptr<Texture2D>&);


            std::shared_ptr<ID3D11Texture2D>  get_shared_texture();

        private:
            HANDLE share_handle_;

            const std::shared_ptr<ID3D11Texture2D> texture_;
            const std::shared_ptr<ID3D11ShaderResourceView> srv_;
            std::shared_ptr<IDXGIKeyedMutex> keyed_mutex_;
            std::shared_ptr<Context> ctx_;

             DISALLOW_COPY_AND_ASSIGN(Texture2D);
        };

        class Effect {
        public:
            Effect(ID3D11VertexShader* vsh,
                ID3D11PixelShader* psh,
                ID3D11InputLayout* layout);

            void bind(const std::shared_ptr<Context>& ctx);
            void unbind();

        private:
            const std::shared_ptr<ID3D11VertexShader> vsh_;
            const std::shared_ptr<ID3D11PixelShader> psh_;
            const std::shared_ptr<ID3D11InputLayout> layout_;
            std::shared_ptr<Context> ctx_;

            DISALLOW_COPY_AND_ASSIGN(Effect);
        };

        class Geometry {
        public:
            Geometry(D3D_PRIMITIVE_TOPOLOGY primitive,
                uint32_t vertices,
                uint32_t stride,
                ID3D11Buffer*);

            void bind(const std::shared_ptr<Context>& ctx);
            void unbind();

            void draw();

        private:
            D3D_PRIMITIVE_TOPOLOGY primitive_;
            uint32_t vertices_;
            uint32_t stride_;
            const std::shared_ptr<ID3D11Buffer> buffer_;
            std::shared_ptr<Context> ctx_;

            DISALLOW_COPY_AND_ASSIGN(Geometry);
        };

        // Abstraction for a 2D layer within a composition.
        class Layer {
        public:
            Layer(const std::shared_ptr<Device>& device, bool flip);
            virtual ~Layer();

            void attach(const std::shared_ptr<Composition>&);

            // Uses normalized 0-1.0 coordinates.
            virtual void move(float x, float y, float width, float height);

            virtual void tick(double t);
            virtual void render(const std::shared_ptr<Context>& ctx) = 0;

            Rect bounds() const;

            std::shared_ptr<Composition> composition() const;

        protected:
            // Helper method for derived classes to draw a textured-quad.
            void render_texture(const std::shared_ptr<Context>& ctx,
                const std::shared_ptr<Texture2D>& texture);

            const std::shared_ptr<Device> device_;
            const bool flip_;

            Rect bounds_;
            std::shared_ptr<Geometry> geometry_;
            std::shared_ptr<Effect> effect_;

        private:
            std::weak_ptr<Composition> composition_;

             DISALLOW_COPY_AND_ASSIGN(Layer);
        };

        // A collection of layers. Will render 1-N layers to a D3D11 device.
        class Composition : public std::enable_shared_from_this<Composition> {
        public:
            explicit Composition(const std::shared_ptr<Device>& device,
                int width = 0,
                int height = 0);

            int width() const { return width_; }
            int height() const { return height_; }

            double fps() const;
            double time() const;

            bool is_vsync() const;

            void tick(double);
            void render(const std::shared_ptr<Context>&);

            void add_layer(const std::shared_ptr<Layer>& layer);
            bool remove_layer(const std::shared_ptr<Layer>& layer);
            void resize(bool vsync, int width, int height);

        private:
            int width_;
            int height_;
            uint32_t frame_;
            int64_t fps_start_;
            double fps_;
            double time_;
            bool vsync_ = true;

            const std::shared_ptr<Device> device_;
            std::vector<std::shared_ptr<Layer>> layers_;

            DISALLOW_COPY_AND_ASSIGN(Composition);
        };

        class FrameBuffer {
        public:
            explicit FrameBuffer(const std::shared_ptr<Device>& device);

            // Called in response to CEF's OnAcceleratedPaint notification.
            void on_paint(void* shared_handle);

            // Returns what should be considered the front buffer.
            std::shared_ptr<Texture2D> texture() const { return shared_buffer_; }

            void copy_shared_texture();
        private:
            const std::shared_ptr<Device> device_;
            std::shared_ptr<Texture2D> shared_buffer_;

             DISALLOW_COPY_AND_ASSIGN(FrameBuffer);
        };

    }
}


#endif // _C_D3D11_WIN_H_
 