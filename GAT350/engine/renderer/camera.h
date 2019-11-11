#pragma once
#include "../framework/actor.h"

class Camera : public Actor
{
public:
	OBJECT_DECLARATION(Camera, Actor)

	virtual ~Camera() {}
	
	virtual void Update() override;
	void SetProjection(float fov, float aspect_ratio, float near_clip, float far_clip);

public:
	glm::mat4 m_view_matrix = glm::mat4(0.0f);
	glm::mat4 m_projection_matrix = glm::mat4(0.0f);

	float fov = 0.0f;
	float aspect_ratio = 0.0f;
	float near_clip = 0.0f;
	float far_clip = 0.0f;
};