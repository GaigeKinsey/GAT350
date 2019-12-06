#pragma once
#include "../engine/framework/scene.h"

class ShadowMapScene : public Scene
{
public:
	OBJECT_DECLARATION(ShadowMapScene, Scene)

	bool Create(const Name& name) override;
	void Update() override;
	void Draw() override;

protected:
	void RenderToTexture();
	void RenderScene();

public:
	glm::vec3 m_scale = glm::vec3(1);

	float m_time = 0.0f;
	float m_amplitude = 0.0f;
	float m_frequency = 0.0f;
	float m_rate = 1.0f;

	glm::vec2 m_uv_scale = glm::vec2(1);
	glm::vec2 m_uv_offset = glm::vec2(0);
};