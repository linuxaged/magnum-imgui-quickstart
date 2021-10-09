#pragma once

#include "Types.h"
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Shaders/Phong.h>
#include <Corrade/Containers/Optional.h>
#include <Magnum/Trade/MeshData.h>
#include <array>
#include "ArcBallCamera.h"

class Renderer {
public:
	Renderer(Scene3D& scene, Magnum::Vector2i size);
	Magnum::SceneGraph::DrawableGroup3D& getDrawableOrigin() {
		return _groupOrigin;
	}
	Magnum::SceneGraph::DrawableGroup3D& getDrawableSegment() {
		return _groupSegment;
	}
	bool loadStl(const char* path);
	void loadSegment(const std::vector<unsigned int>& indices);

    void bind() {
        _offscreen.bind();
    }
    void blit() {
        _offscreen.blit();
    }
    void draw() {
        _camera.draw(getDrawableOrigin());
        _camera.draw(getDrawableSegment());
    }
    void resize(Magnum::Vector2i newSize) {
        _offscreen.resize(newSize);
    }
    Magnum::GL::Texture2D& getTexture() {
        return _offscreen.renderTexture;
    }
private:
	struct Vertex {
		Magnum::Vector3 pos;
		Magnum::Vector3 normal;
	};
	Scene3D& _scene;
	Corrade::Containers::Optional<Magnum::Trade::MeshData> _data;
	Magnum::GL::Buffer _vbuffer;
	Magnum::GL::Mesh _meshOrigin;
	
	Magnum::Shaders::Phong _phongShaders[2];
	Magnum::SceneGraph::DrawableGroup3D _groupOrigin;
	Magnum::SceneGraph::DrawableGroup3D _groupSegment;
	std::vector<Vertex> _verticesOrigin;
	std::array<std::vector<Vertex>, 32>  _verticesSegment;
	std::array<Magnum::GL::Mesh, 32> _meshesSegment;
    /*
     * Render to Texture
     */
    struct Offscreen {
        Magnum::GL::Framebuffer framebufferMSAA{Magnum::NoCreate};
        Magnum::GL::Framebuffer framebuffer{Magnum::NoCreate};
        Magnum::GL::Texture2D renderTexture;
        Magnum::GL::Renderbuffer colorBuffer;
        Magnum::GL::Renderbuffer depthStencilBuffer;
        Magnum::Vector2i size;
        void init(Magnum::Vector2i Size) {
            size = Size;

            framebuffer = Magnum::GL::Framebuffer{{ { 0, 0 }, size }};
            renderTexture.setWrapping(Magnum::GL::SamplerWrapping::ClampToEdge)
                .setMagnificationFilter(Magnum::GL::SamplerFilter::Linear)
                .setMinificationFilter(Magnum::GL::SamplerFilter::Linear)
                .setStorage(1, Magnum::GL::TextureFormat::RGBA8, size);
            framebuffer.attachTexture(Magnum::GL::Framebuffer::ColorAttachment{0},
                                      renderTexture, 0);

            framebufferMSAA = Magnum::GL::Framebuffer{{ { 0, 0 }, size }};
            colorBuffer.setStorageMultisample(8, Magnum::GL::RenderbufferFormat::RGBA8, size);
            depthStencilBuffer.setStorageMultisample(8, Magnum::GL::RenderbufferFormat::Depth24Stencil8, size);
            framebufferMSAA.attachRenderbuffer(Magnum::GL::Framebuffer::ColorAttachment{0}, colorBuffer);
            framebufferMSAA.attachRenderbuffer(Magnum::GL::Framebuffer::BufferAttachment::DepthStencil, depthStencilBuffer);
        }
        void bind() {
            framebufferMSAA
                .clear(Magnum::GL::FramebufferClear::Color | Magnum::GL::FramebufferClear::Depth)
                .bind();
        }
        void blit() {
            framebuffer.clear(Magnum::GL::FramebufferClear::Color)
                .bind();
            Magnum::GL::Framebuffer::blit(framebufferMSAA, framebuffer,
                                  {{}, size}, Magnum::GL::FramebufferBlit::Color);
        }
        void resize(Magnum::Vector2i newSize) {
            size = newSize;

            renderTexture = Magnum::GL::Texture2D{};
            renderTexture.setWrapping(Magnum::GL::SamplerWrapping::ClampToEdge)
                .setMagnificationFilter(Magnum::GL::SamplerFilter::Linear)
                .setMinificationFilter(Magnum::GL::SamplerFilter::Linear)
                .setStorage(1, Magnum::GL::TextureFormat::RGBA8, size);
            framebuffer.attachTexture(Magnum::GL::Framebuffer::ColorAttachment{0},
                                      renderTexture, 0);
            //
            colorBuffer = Magnum::GL::Renderbuffer{};
            colorBuffer.setStorageMultisample(8, Magnum::GL::RenderbufferFormat::RGBA8, size);
            depthStencilBuffer = Magnum::GL::Renderbuffer{};
            depthStencilBuffer.setStorageMultisample(8, Magnum::GL::RenderbufferFormat::Depth24Stencil8, size);
            framebufferMSAA.attachRenderbuffer(Magnum::GL::Framebuffer::ColorAttachment{0}, colorBuffer);
            framebufferMSAA.attachRenderbuffer(Magnum::GL::Framebuffer::BufferAttachment::DepthStencil, depthStencilBuffer);

            framebuffer.setViewport({{}, size});
            framebufferMSAA.setViewport({{}, size});
        }
    };
    Offscreen _offscreen;
    Magnum::Examples::ArcBallCamera _camera;
};