#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

enum eAnchor
{
	TOP_LEFT,
	TOP_MIDDLE,
	TOP_RIGHT,
	MIDDLE_LEFT,
	MIDDLE_MIDDLE,
	MIDDLE_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_MIDDLE,
	BOTTOM_RIGHT,
	ANCHOR_ENUM_COUNT
};

const static char* Anchors_Strings[] =
{
	"Top Left",
	"Top Middle",
	"Top Right",
	"Middle Left",
	"Middle Middle",
	"Middle Right",
	"Bottom Left",
	"Bottom Middle",
	"Bottom Right"
};

class cUIWidget
{
public:

	virtual ~cUIWidget();

	bool focusable = false;
	
	float heightPercent = 1.f; // Number from 0 to 1 that represents percentage of vertical space this widget take from parent (or window height if parent is null)
	float aspectRatio = 1.f; // Ratio between widget height and width (height / width)
	eAnchor anchor = MIDDLE_MIDDLE;

	virtual void SetupWidget();

private:
	std::vector<cUIWidget*> children;
	cUIWidget* parent;
public:
	// Do NOT set manually, use AddChild functions
	void AddChild(cUIWidget* newChild);

protected:
	const float CalculateHeightPixels();
	const float CalculateHeightScreenPercent();
	const float CalculateWidthPixels();
	const float CalculateWidthScreenPercent();
	const float CalculateVerticalTranslate();
	const float CalculateHorizontalTranslate();
	const glm::vec2 CalculateBottomLeftTranslate();

	friend class cRenderManager;
};

class cUIStaticImage : public cUIWidget
{
	~cUIStaticImage() {}
public:
	std::string textureName;
	virtual void SetupWidget();
};

class cUIAnimatedSprite : public cUIWidget
{
public:
	std::string spriteTextureName;
	virtual void SetupWidget();
};

struct sCharBufferData
{
	int posX, posY, sizeX, sizeY;	// position from origin and screen percent size
	//int charId;
};

class cUIText
{
public:
	cUIText(cUIWidget* parent);
	~cUIText();

	cUIWidget* parentWidget;
	//unsigned int bufferDataId;
	std::vector<sCharBufferData> data;
	unsigned int drawCharCount = 0;
	std::string fontName;
	std::string text;
	glm::vec3 color;
	glm::vec2 origin = glm::vec2(0.f); // [0-1]
	float heightPercent; // to parent widget
	float widthCutoffPercent = 1;

	//virtual void SetupWidget();
};