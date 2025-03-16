#pragma once
#include <string>

namespace aie {

// Texture class for handling OpenGL textures
class Texture {
public:

	enum Format : unsigned int {
		RED = 1,  // Greyscale
		RG,       // Red-Green
		RGB,      // Standard colour image
		RGBA      // Colour image with transparency
	};

	Texture();
	Texture(const char* filename);
	Texture(unsigned int width, unsigned int height, Format format, unsigned char* pixels = nullptr);
	virtual ~Texture();

	// Loads an image file into an OpenGL texture
	bool load(const char* filename);

	// Creates a texture from raw pixel data
	void create(unsigned int width, unsigned int height, Format format, unsigned char* pixels = nullptr);

	// Returns the filename or "none" if not loaded from a file
	const std::string& getFilename() const { return m_filename; }

	// Binds the texture to a texture unit
	void bind(unsigned int slot) const;

	// Returns the OpenGL texture handle
	unsigned int getHandle() const { return m_glHandle; }
	unsigned int getWidth() const { return m_width; }
	unsigned int getHeight() const { return m_height; }
	unsigned int getFormat() const { return m_format; }
	const unsigned char* getPixels() const { return m_loadedPixels; }

protected:

	std::string		m_filename;
	unsigned int	m_width;
	unsigned int	m_height;
	unsigned int	m_glHandle;
	unsigned int	m_format;
	unsigned char*	m_loadedPixels;
	};
} 