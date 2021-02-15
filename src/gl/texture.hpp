#ifndef TOPAZ_GL_TEXTURE_HPP
#define TOPAZ_GL_TEXTURE_HPP
#include "gl/image.hpp"
#include "gl/texture_sentinel.hpp"
#include "core/vector.hpp"
#include <optional>

namespace tz::gl
{
	/**
	 * Retrieve the global bindless texture sentinel.
	 * Note: This is stubbed out in Release.
	 * @return Reference to global sentinel.
	 */
	TextureSentinel& sentinel();

	/**
	 * \addtogroup tz_gl Topaz Graphics Library (tz::gl)
	 * @{
	 */

	using TextureName = GLuint;
	using BindlessTextureHandle = GLuint64;

	struct TextureParameters;
	enum class TextureComponentType : GLenum;
	enum class TextureInternalFormat : GLint;
	enum class TextureFormat : GLenum;

	struct TextureDataDescriptor
	{
		TextureComponentType component_type;
		TextureInternalFormat internal_format;
		TextureFormat format;
	
		unsigned int width = 0;
		unsigned int height = 0;
		/// Note: Excludes width and height, only formatting data.
		bool operator==(const TextureDataDescriptor& rhs) const;
		bool operator!=(const TextureDataDescriptor& rhs) const;
	};

	/**
	 * Used to store image data in VRAM. Can be attached to a shader to bind a texture to a given tz::gl::Object.
	 */
	class Texture
	{
	public:
		/**
		 * Create an empty Texture.
		 */
		Texture();
		Texture(const Texture& copy) = delete;
		Texture(Texture&& move);

		Texture& operator=(const Texture& rhs) = delete;
		Texture& operator=(Texture&& rhs);
		~Texture();
		/**
		 * Set the parameters of the texture.
		 */
		void set_parameters(const TextureParameters& params);
		/**
		 * Clear the data-store and resize it adhereing to the giving description.
		 * 
		 * @param descriptor Type of the underlying data and width/height to resize to.
		 */
		void resize(const TextureDataDescriptor& descriptor);
		/**
		 * Retrieve the number of pixels in the Texture.
		 * @return Size, in number of pixels.
		 */
		std::size_t size() const;
		/**
		 * Query as to whether there are any pixels within the Texture.
		 * @return True if there are no pixels. False otherwise.
		 */
		bool empty() const;
		/**
		 * Retrieve the width of the texture, in pixels.
		 * @return Width in pixels.
		 */
		unsigned int get_width() const;
		/**
		 * Retrieve the height of the texture, in pixels.
		 * @return Height in pixels.
		 */
		unsigned int get_height() const;
		/**
		 * Query as to whether the texture has been made terminal.
		 * @return True if texture is terminal. Otherwise false.
		 */
		bool is_terminal() const;
		/**
		 * Make the texture terminal.
		 * Precondition: Texture is nonterminal prior to invocation. Otherwise, this will assert and invoke UB.
		 */
		void make_terminal();
		/**
		 * Retrieve a bindless texture handle which can be inserted into a UBO/SSBO.
		 * Precondition: Texture is terminal. Otherwise, this will hard-assert.
		 */
		BindlessTextureHandle get_terminal_handle() const;
		/**
		 * Bind the texture, causing sampler2Ds at the given binding-id to reference this texture in subsequent render invocations.
		 * @param binding_id ID of the sampler2D that should resolve to this texture.
		 */
		void bind(std::size_t binding_id) const;
		/**
		 * Replace the existing underlying image data with some new data. Any data in the data-store beforehand will be discarded.
		 * Note: If the new image data format is different or the dimensions do not match, the underlying image data store is automatically resized.
		 * Precondition: If the texture is terminal and the underlying image data store is automatically resized, this will assert and invoke UB.
		 * 
		 * Example: tz::gl::Image<tz::gl::PixelRGBA8> resolves to PixelType == tz::gl::PixelRGBA, ComponentType == std::byte.
		 * @tparam PixelType Type of the pixel template to use. Example: tz::gl::PixelRGBA
		 * @tparam ComponentType Underlying data type of the components within the pixel.
		 * @param image Image whose data should be uploaded to VRAM.
		 */
		template<template<typename> class PixelType, typename ComponentType>
		void set_data(const tz::gl::Image<PixelType<ComponentType>>& image);
		/**
		 * Retrieve the image data from within the data-store.
		 * Any data in the data-store beforehand will be discarded.
		 * 
		 * Example: tz::gl::Image<tz::gl::PixelRGBA8> resolves to PixelType == tz::gl::PixelRGBA, ComponentType == std::byte.
		 * @tparam PixelType Type of the pixel template to use. Example: tz::gl::PixelRGBA
		 * @tparam ComponentType Underlying data type of the components within the pixel.
		 * @return Image containing a copy of the data-store.
		 */
		template<template<typename> class PixelType, typename ComponentType>
		tz::gl::Image<PixelType<ComponentType>> get_data() const;
		/**
		 * Bind the texture to the currently-bound tz::gl::Frame using the given attachment.
		 * Note: Probably not worth invoking this yourself; let the Frame sort this out for you.
		 * @param attachment Desired attachment with which to associate the texture.
		 */
		void bind_to_frame(GLenum attachment) const;
		void dui_draw(tz::Vec2 size = {-1.0f, -1.0f}, tz::Vec2 uv0 = {0.0f, 0.0f}, tz::Vec2 uv1 = {1.0f, 1.0f}, tz::Vec4 tint_col = {1.0f, 1.0f, 1.0f, 1.0f}, tz::Vec4 border_col = {0.0f, 0.0f, 0.0f, 0.0f}) const;
	protected:
		void internal_bind() const;
		void internal_unbind() const;

		TextureName handle;
		std::optional<TextureDataDescriptor> descriptor;
		std::optional<BindlessTextureHandle> bindless;
	};

	enum class FrameAttachment : GLenum
	{
		Colour = GL_COLOR_ATTACHMENT0,
		Depth = GL_DEPTH_ATTACHMENT,
		ExtraColour1 = GL_COLOR_ATTACHMENT1,
		ExtraColour2 = GL_COLOR_ATTACHMENT2,
		ExtraColour3 = GL_COLOR_ATTACHMENT3,
	};

	/**
	 * Basically a write-only texture.
	 */
	class RenderBuffer
	{
	public:
		/**
		 * Initialise an empty renderbuffer. Will not be usable as a Frame attachment.
		 */
		RenderBuffer();
		RenderBuffer(const RenderBuffer& copy) = delete;
		RenderBuffer(RenderBuffer&& move);
		/**
		 * Construct a renderbuffer based upon the given descriptor.
		 * @param descriptor Description of the data expected to be stored in this renderbuffer.
		 */
		RenderBuffer(TextureDataDescriptor descriptor);
		~RenderBuffer();

		/**
		 * Bind the renderbuffer. Mostly only intended for internal use.
		 */
		void bind() const;
		/**
		 * Unbind the renderbuffer.
		 */
		void unbind() const;
		/**
		 * Bind the renderbuffer to the currently-bound tz::gl::Frame using the given attachment.
		 * Note: Probably not worth invoking this yourself; let the Frame sort this out for you.
		 * @param attachment Desired attachment with which to associate the renderbuffer.
		 */
		void bind_to_frame(FrameAttachment attachment) const;
	private:
		GLuint handle;
		TextureDataDescriptor descriptor;
	};

	enum class TextureComponentType : GLenum
	{
		Byte = GL_UNSIGNED_BYTE,
		Short = GL_UNSIGNED_SHORT,
		Int = GL_UNSIGNED_INT,
		HalfFloat = GL_HALF_FLOAT,
		Float = GL_FLOAT,
	};

	enum class TextureInternalFormat : GLint
	{
		// Unsized format types
		DepthUnsized = GL_DEPTH_COMPONENT,
		DepthStencilUnsized = GL_DEPTH_STENCIL,
		RedUnsized = GL_RED,
		RedGreenUnsized = GL_RG,
		RedGreenBlueUnsized = GL_RGB,
		RedGreenBlueAlphaUnsized = GL_RGBA,

		sRedGreenBlueUnsized = GL_SRGB,
		sRedGreenBlueAlphaUnsized = GL_SRGB_ALPHA,

		// Sized format types (number represents number of bits per component)
		// e.g RGBA16 = 16 bits per component = 64 bits per element
		Depth16 = GL_DEPTH_COMPONENT16,
		Depth32 = GL_DEPTH_COMPONENT32,
		Red8 = GL_R8,
		Red16 = GL_R16,

		RedGreen8 = GL_RG8,
		RedGreen16 = GL_RG16,

		Red3Green3Blue2 = GL_R3_G3_B2,
		RedGreenBlue4 = GL_RGB4,
		RedGreenBlue8 = GL_RGB8,
		RedGreenBlue16 = GL_RGB16,

		RedGreenBlueAlpha2 = GL_RGBA2,
		RedGreenBlueAlpha4 = GL_RGBA4,
		RedGreenBlueAlpha8 = GL_RGBA8,
		RedGreenBlueAlpha16 = GL_RGBA16,

		sRedGreenBlue8 = GL_SRGB8,
		sRedGreenBlueAlpha8 = GL_SRGB8_ALPHA8,
		// Floating-point sized format types
		// Unlike non floating-point sized formats, elements are not clamped between 0.0 and 1.0. This is useful for HDR.
		Depth32f = GL_DEPTH_COMPONENT32F,

		Red16f = GL_R16F,
		Red32f = GL_R32F,
		RedGreen16f = GL_RG16F,
		RedGreen32f = GL_RG32F,
		RedGreenBlue16f = GL_RGB16F,
		RedGreenBlue32f = GL_RGB32F,
		RedGreenBlueAlpha16f = GL_RGBA16F,
		RedGreenBlueAlpha32f = GL_RGBA32F,
	};

	enum class TextureFormat : GLenum
	{
		Red = GL_RED,
		RedGreen = GL_RG,
		RedGreenBlue = GL_RGB,
		RedGreenBlueAlpha = GL_RGBA,

		BlueGreenRed = GL_BGR,
		BlueGreenRedAlpha = GL_BGRA,
		DepthComponent = GL_DEPTH_COMPONENT,
		DepthStencilComponent = GL_DEPTH_STENCIL,
	};

	enum class TextureMinificationFunction : GLint
	{
		Nearest = GL_NEAREST,
		WeightedAverage = GL_LINEAR,
		NearestClosestMinmap = GL_NEAREST_MIPMAP_NEAREST,
		WeightedAverageClosestMinmap = GL_LINEAR_MIPMAP_NEAREST,
		NearestAverageOfMinmaps = GL_NEAREST_MIPMAP_LINEAR,
		WeightedAverageAverageOfMinmaps = GL_LINEAR_MIPMAP_LINEAR,
	};

	enum class TextureMagnificationFunction : GLint
	{
		Nearest = GL_NEAREST,
		WeightedAverage = GL_LINEAR,
	};

	// s, t == u, v
	enum class TextureWrapFunction : GLint
	{
		ClampToEdge = GL_CLAMP_TO_EDGE,
		MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE,
		ClampToBorder = GL_CLAMP_TO_BORDER,
		MirroredRepeat = GL_MIRRORED_REPEAT,
		Repeat = GL_REPEAT,
	};

	struct TextureParameters
	{
		TextureMinificationFunction min_filter;
		TextureMagnificationFunction mag_filter;
		TextureWrapFunction horizontal_wrap;
		TextureWrapFunction vertical_wrap;
	};

	/**
	 * @}
	 */

	/// For those times where you really don't care about life.
	static constexpr TextureParameters default_texture_params{TextureMinificationFunction::Nearest, TextureMagnificationFunction::Nearest, TextureWrapFunction::Repeat, TextureWrapFunction::Repeat};
}

#include "gl/texture.inl"
#endif // TOPAZ_GL_TEXTURE_HPP