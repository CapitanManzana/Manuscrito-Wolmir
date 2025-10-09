#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer() {

	transform = gameObject->getComponent<Transform>();

	row = 0;
	col = 0;

	isEnabled = true;
}

SpriteRenderer::SpriteRenderer(Texture* texture, int row, int col) {
	this->texture = texture;
	this->row = row;
	this->col = col;

	transform = gameObject->getComponent<Transform>();

	isEnabled = true;
}

SpriteRenderer::~SpriteRenderer() {
	texture = nullptr;
	transform = nullptr;
}

void SpriteRenderer::render() {
	if (texture && transform && isEnabled) {
		texture->renderFrame(transform->dstRect, row, col);
	}
}