#include "UIElements.h"
#include "cCamera.h"

// Should be removed (maybe?)
#include "cRenderManager.h"
#include "cTextureManager.h"

void cUIWidget::DrawWidget()
{
	for (int i = 0; i < children.size(); i++)
	{
		children[i].DrawWidget();
	}

	cRenderManager::GetInstance()->use("debug");
	cTextureManager::GetInstance()->SetupTexture(textureName);

	float height = CalculateHeightScreenPercent();
	float width = CalculateWidthScreenPercent();

	cRenderManager::GetInstance()->setFloat("widthPercent", width);
	cRenderManager::GetInstance()->setFloat("heightPercent", height);

	glBindVertexArray(cRenderManager::GetInstance()->quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void cUIWidget::AddChild(cUIWidget& newChild)
{
	newChild.parent = this;
	children.push_back(newChild);
}

float cUIWidget::CalculateHeightPixels()
{
	if (!parent) return heightPercent * cCamera::GetInstance()->SCR_HEIGHT;

	return heightPercent * parent->CalculateHeightPixels();
}

float cUIWidget::CalculateHeightScreenPercent()
{
	if (!parent) return heightPercent;

	float heightInPixels = parent->CalculateHeightPixels() * heightPercent;

	return heightInPixels / cCamera::GetInstance()->SCR_HEIGHT;
}

float cUIWidget::CalculateWidthPixels()
{
	return CalculateHeightPixels() / aspectRatio;
}

float cUIWidget::CalculateWidthScreenPercent()
{
	return CalculateWidthPixels() / cCamera::GetInstance()->SCR_WIDTH;;
}
