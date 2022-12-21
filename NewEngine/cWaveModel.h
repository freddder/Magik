#pragma once
#include "cRenderModel.h"

class cWaveModel : public cRenderModel
{
public:

	glm::vec3 textureOffset;
	float timer;

	cWaveModel();

	virtual void SetUpUniforms();
};