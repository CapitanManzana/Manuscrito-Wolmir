#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL3/SDL.h>

/**
 * Object-oriented wrapper for SDL textures with frame support.
 */
class Texture
{
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;
	int width;
	int height;
	int nrows;
	int ncolumns;
	int frameWidth;
	int frameHeight;

	SDL_FRect getFrameRect(int row, int col) const;

public:
	Texture(SDL_Renderer* renderer, const char* filename, int rows = 1, int columns = 1);
	Texture(SDL_Renderer* renderer, SDL_Texture* texture, int rows = 1, int columns = 1);
	~Texture();

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) noexcept;
	Texture& operator=(Texture&&) noexcept;

	/// Width of a frame in pixels
	int getFrameWidth() const;
	/// Height of a frame in pixels
	int getFrameHeight() const;
	/// Number of frames in each row
	int getNumRows() const;
	/// Number of frames in each column
	int getNumColumns() const;

	/// Render the whole picture filling the entire screen
	void render() const;
	/// Render the whole picture to the given rectangle
	void render(const SDL_FRect& target) const;
	/// Render a rectangle full screen
	void renderRect(const SDL_FRect& source) const;
	/// Render the whole picture with additional options from SDL_RenderCopyEx
	void render(const SDL_FRect& target,
	            double angle,
	            const SDL_FPoint* center = nullptr,
	            SDL_FlipMode flip = SDL_FLIP_NONE) const;

	/// Render a frame to the given rectangle
	void renderFrame(const SDL_FRect& target, int row, int col) const;
	/// Render a frame to the given rectangle
	void renderFrame(const SDL_FRect& target, int row, int col, SDL_FlipMode flip) const;
	/// Render a frame with additional options
	void renderFrame(const SDL_FRect& target,
	                 int row,
	                 int col,
	                 double angle,
	                 const SDL_FPoint* center = nullptr,
	                 SDL_FlipMode flip = SDL_FLIP_NONE) const;

	/// Render the whole picture to the given rectangle with modified color
	void render(const SDL_FRect& target, SDL_Color color) const;
};

inline int
Texture::getFrameWidth() const
{
	return frameWidth;
}

inline int
Texture::getFrameHeight() const
{
	return frameHeight;
}

inline int
Texture::getNumRows() const
{
	return nrows;
}

inline int
Texture::getNumColumns() const
{
	return ncolumns;
}

#endif // TEXTURE_H
