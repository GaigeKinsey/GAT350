#pragma once
#include "../engine/framework/scene.h"

class CubemapScene : public Scene
{
public:
	OBJECT_DECLARATION(CubemapScene, Scene)

	bool Create(const Name& name) override;
	void Update() override;
	void Draw() override;

public:
	float m_index = 1.0f;
	float m_mix = 1.0f;
};