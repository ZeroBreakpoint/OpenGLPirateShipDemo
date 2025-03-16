#include "glad.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace aie {

// Default constructor
Texture::Texture() 
	: m_filename("none"),
	m_width(0),
	m_height(0),
	m_glHandle(0),
	m_format(0),
	m_loadedPixels(nullptr) {
}

// Constructor that loads a texture from a file
Texture::Texture(const char * filename)
	: m_filename("none"),
	m_width(0),
	m_height(0),
	m_glHandle(0),
	m_format(0),
	m_loadedPixels(nullptr) {

	load(filename);
}

// Constructor that creates a blank texture with the specified dimensions and format
Texture::Texture(unsigned int width, unsigned int height, Format format, unsigned char* pixels)
	: m_filename("none"),
	m_width(width),
	m_height(height),
	m_format(format),
	m_loadedPixels(nullptr) {

	create(width, height, format, pixels);
}

// Destructor
Texture::~Texture() {
	// Free GPU memory for textures when destroyed
	if (m_glHandle != 0)
		glDeleteTextures(1, &m_glHandle);
	if (m_loadedPixels != nullptr)
		stbi_image_free(m_loadedPixels);
}

bool Texture::load(const char* filename) {
	
	// If a texture was previously loaded, delete it before loading a new one
	if (m_glHandle != 0) {
		glDeleteTextures(1, &m_glHandle);
		m_glHandle = 0;
		m_width = 0;
		m_height = 0;
		m_filename = "none";
	}
	
	// Load image file using stb_image
	int x = 0, y = 0, comp = 0;
	m_loadedPixels = stbi_load(filename, &x, &y, &comp, STBI_default);

	// Check if image loading was successful
	if (m_loadedPixels != nullptr) {
		glGenTextures(1, &m_glHandle);
		glBindTexture(GL_TEXTURE_2D, m_glHandle);

		// Determine texture format based on number of colour channels
		switch (comp) {
		case STBI_grey:
			m_format = RED;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y,
						 0, GL_RED, GL_UNSIGNED_BYTE, m_loadedPixels);
			break;
		case STBI_grey_alpha:
			m_format = RG;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, x, y,
						 0, GL_RG, GL_UNSIGNED_BYTE, m_loadedPixels);
			break;
		case STBI_rgb:
			m_format = RGB;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y,
						 0, GL_RGB, GL_UNSIGNED_BYTE, m_loadedPixels);
			break;
		case STBI_rgb_alpha:
			m_format = RGBA;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y,
						 0, GL_RGBA, GL_UNSIGNED_BYTE, m_loadedPixels);
			break;
		default:	break;
		};
		
		// Set texture filtering parameters (Linear filtering for smooth textures)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Generate mipmaps for better scaling quality
		glGenerateMipmap(GL_TEXTURE_2D);

		// Unbind the texture after setup
		glBindTexture(GL_TEXTURE_2D, 0);

		// Store texture dimensions and filename
		m_width = (unsigned int)x;
		m_height = (unsigned int)y;
		m_filename = filename;
		return true;
	}
	// If image loading failed, return false
	return false;
}

void Texture::create(unsigned int width, unsigned int height, Format format, unsigned char* pixels) {
	// If an existing texture handle exists, delete it
	if (m_glHandle != 0) {
		glDeleteTextures(1, &m_glHandle);
		m_glHandle = 0;
		m_filename = "none"; // Reset filename since it's a generated texture
	}

	// Store texture properties
	m_width = width;
	m_height = height;
	m_format = format;

	// Generate an OpenGL texture
	glGenTextures(1, &m_glHandle);
	glBindTexture(GL_TEXTURE_2D, m_glHandle);

	// Set default texture filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Set texture wrapping to repeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Upload texture data to GPU
	switch (m_format) {
	case RED:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
		break;
	case RG:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_width, m_height, 0, GL_RG, GL_UNSIGNED_BYTE, pixels);
		break;
	case RGB:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		break;
	case RGBA:
	default:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	};

	// Unbind texture after setup
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(unsigned int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot); // Activate the specified texture slot
	glBindTexture(GL_TEXTURE_2D, m_glHandle); // Bind the texture to that slot

	}
}