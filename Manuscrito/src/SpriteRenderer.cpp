#include "SpriteRenderer.h"
#include "texture.h"
#include "GameObject.h"

SpriteRenderer::SpriteRenderer() {
	row = 0;
	col = 0;

	isEnabled = true;
}

SpriteRenderer::SpriteRenderer(Texture* texture, int row, int col) {
	this->texture = texture;
	this->row = row;
	this->col = col;

	isEnabled = true;
}

SpriteRenderer::~SpriteRenderer() {
	texture = nullptr;
}

void SpriteRenderer::render() {
	if (texture && isEnabled) {
		texture->renderFrame(gameObject->transform->dstRect, row, col);
	}
}