#pragma once

typedef int TextureId;

// pixel format for images (CPU)
enum class PixelFormat
{
	RGB8 = 0,
	RGBA8,

	COUNT
};

// size in bytes
const unsigned PIXEL_FORMAT_SIZE[(int)PixelFormat::COUNT];

// The internal GPU formal of the texels
enum class TexelFormat
{
	RGB8,
	RGBA8,
	DEPTH16,
	DEPTH24,
	DEPTH32,
	DEPTH_AUTO,		// OpenGL with choose automatically the resolution
	DEPTH24_STENCIL8,

	COUNT
};

enum class TextureWrapMode
{
	REPEAT = 0,
	MIRROR,
	CLAMP_TO_EDGE,
	CLAMP_TO_BORDER,

	COUNT
};

enum class TextureFilterMode
{
	NEAREST,
	BILINEAR,
	LINEAR = BILINEAR,
	LINEAR_MIPMAP,
	TRILINEAR,
};

enum class TextureUnit
{
	COLOR = 0,
	NORMAL,
	SPECULAR,
	EMISSIVE,
};

// 2D image (CPU)
class Image
{

public:
	Image() : data(nullptr), width(-1), height(-1){}

	PixelFormat getPixelFormat()const { return format; }

	int getWidth()const { return width; }
	int getHeight()const { return height; }

	// return the size of one pixel in bytes
	unsigned getPerPixelSize()const;

	const void* getData()const { return data; }
	void* getData() { return data; }

	void free();

private:
	void* data;
	int width, height;
	PixelFormat format;

public:
	static Image createEmpty(unsigned width, unsigned height, PixelFormat format);
	static Image loadFromFile(const char* fileName);
};

// 2D texture (GPU)
class Texture
{
public:

	Texture() : id(-1), width(-1), height(-1), mipmapLevels(0) {}

	TextureId getId()const { return id; }
	void bindToUnit(unsigned unit)const;
	void bindToUnit(TextureUnit unit)const;

	TexelFormat getTexelFormat()const { return texelFormat; }

	TextureWrapMode getWrapMode()const { return wrapMode; }
	void setWrapMode(TextureWrapMode wrapMode) { setWrapModeUv(wrapMode, wrapMode); }
	void setWrapModeUv(TextureWrapMode wrapMode) { setWrapModeUv(wrapMode, wrapMode); }
	void setWrapModeUv(TextureWrapMode wrapModeU, TextureWrapMode wrapModeV);
	void setWrapModeU(TextureWrapMode wrapMode);
	void setWRapModeV(TextureWrapMode wrapMode);

	TextureFilterMode getFilterMode()const { return filterMode; }
	void setFilterMode(TextureFilterMode filterMode);

	unsigned getMipMapLevels()const { return mipmapLevels; }
	bool hasMipmaps()const { return mipmapLevels > 0; }
	void generateMipmaps();

	// writes the texture to a file
	void save(const char* fileName, bool async=false);

	void free();

private:
	void resetFilterMode();

private:
	TextureId id;
	int width, height;
	TexelFormat texelFormat;
	TextureWrapMode wrapMode;
	TextureFilterMode filterMode;
	unsigned mipmapLevels;

public:
	static Texture createEmpty(unsigned width, unsigned height, TexelFormat texelFormat = TexelFormat::RGBA8);
	// if internalFormat is COUNT it will choose the best match automatically
	static Texture loadFromFile(const char* fileName, TexelFormat internalFormat = TexelFormat::COUNT);
	static Texture createFromImage(const Image& image);
};

