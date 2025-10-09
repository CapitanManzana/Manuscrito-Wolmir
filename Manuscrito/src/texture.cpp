#include "texture.h"

#include <SDL3_image/SDL_image.h>
#include <string>

using namespace std;

SDL_Texture* tryLoadTexture(SDL_Renderer* renderer, const char* filename)
{
	SDL_Texture* texture = IMG_LoadTexture(renderer, filename);

	if (texture == nullptr)
		throw "load image texture: "s + filename;

	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

	return texture;
}

Texture::Texture(SDL_Renderer* renderer, SDL_Texture* texture, int rows, int columns)
  : renderer(renderer)
  , texture(texture)
  , nrows(rows)
  , ncolumns(columns)
{
	width = texture->w;
	height = texture->h;

	frameWidth = width / ncolumns;
	frameHeight = height / nrows;
}

Texture::Texture(SDL_Renderer* renderer, const char* filename, int rows, int columns)
  : Texture(renderer, tryLoadTexture(renderer, filename), rows, columns)
{
}

Texture::~Texture()
{
	SDL_DestroyTexture(texture);
}

Texture::Texture(Texture &&other) noexcept
 : renderer(other.renderer)
 , texture(other.texture)
 , width(other.width)
 , height(other.height)
 , nrows(other.nrows)
 , ncolumns(other.ncolumns)
 , frameWidth(other.frameWidth)
 , frameHeight(other.frameHeight)
{
	other.texture = nullptr;
}

Texture&
Texture::operator=(Texture&& other) noexcept
{
	renderer = other.renderer;
	texture = other.texture;
	width = other.width;
	height = other.height;
	nrows = other.nrows;
	ncolumns = other.ncolumns;
	frameWidth = other.frameWidth;
	frameHeight = other.frameHeight;

	other.texture = nullptr;

	return *this;
}

SDL_FRect
Texture::getFrameRect(int row, int col) const
{
	return SDL_FRect{float(col * frameWidth), float(row * frameHeight),
	                 float(frameWidth), float(frameHeight)};
}

void
Texture::render() const
{
	SDL_RenderTexture(renderer, texture, nullptr, nullptr);
}

void
Texture::render(const SDL_FRect& rect) const
{
	SDL_RenderTexture(renderer, texture, nullptr, &rect);
}

void
Texture::renderRect(const SDL_FRect& source) const
{
	SDL_RenderTexture(renderer, texture, &source, nullptr);
}

void
Texture::render(const SDL_FRect& rect, double angle, const SDL_FPoint* center, SDL_FlipMode flip) const
{
	SDL_RenderTextureRotated(renderer, texture, nullptr, &rect, angle, center, flip);
}

void
Texture::renderFrame(const SDL_FRect& rect, int row, int col) const
{
	SDL_FRect origin = getFrameRect(row, col);
	SDL_RenderTexture(renderer, texture, &origin, &rect);
}

void
Texture::renderFrame(const SDL_FRect& rect, int row, int col, SDL_FlipMode flip) const
{
	SDL_FRect origin = getFrameRect(row, col);
	SDL_RenderTextureRotated(renderer, texture, &origin, &rect, 0, nullptr, flip);
}

void
Texture::renderFrame(const SDL_FRect& rect,
                     int row,
                     int col,
                     double angle,
                     const SDL_FPoint* center,
                     SDL_FlipMode flip) const
{
	SDL_FRect origin = getFrameRect(row, col);
	SDL_RenderTextureRotated(renderer, texture, &origin, &rect, angle, center, flip);
}

void
Texture::render(const SDL_FRect& rect, SDL_Color color) const
{
	// Guarda el color original para reestablecerlo después
	SDL_Color original;
	SDL_GetTextureColorMod(texture, &original.r, &original.g, &original.b);

	SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
	render(rect);
	SDL_SetTextureColorMod(texture, original.r, original.g, original.b);
}
