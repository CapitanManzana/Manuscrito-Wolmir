#include "SpriteRenderer.h"
#include "texture.h"
#include "GameObject.h"
#include "Transform.h"

SpriteRenderer::SpriteRenderer() {
	row = 0;
	col = 0;

	isEnabled = true;
	texture = nullptr;
	transform = nullptr;
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
	if (transform == nullptr) throw ("[%s] Renderer no tiene transform asignado", gameObject->getName());

 	if (texture && isEnabled) {
		texture->renderFrame(transform->dstRect, row, col);
	}
}

void SpriteRenderer::onComponentAdd() {
	transform = gameObject->getComponent<Transform>();
	gameObject->spriteRenderer = this;

	if (transform != nullptr && texture != nullptr) {
		transform->updateTextureSize(Vector2D<float>(texture->getWidth(), texture->getHeight()));
	}
}

Texture* SpriteRenderer::getTexture() const
{
	return texture;
}

void SpriteRenderer::setTexture(Texture* newTexture)
{
	if (newTexture == nullptr) return;

	texture = newTexture;
	if (transform != nullptr) {
		transform->updateTextureSize(Vector2D<float>(texture->getWidth(), texture->getHeight()));
	}
}