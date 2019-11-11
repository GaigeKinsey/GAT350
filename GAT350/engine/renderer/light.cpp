#include "light.h"
#include "program.h"
#include "camera.h"

void Light::SetShader(class Program* shader)
{
	ASSERT(shader);

	shader->Use();

	shader->SetUniform("light.ambient", ambient);
	shader->SetUniform("light.diffuse", diffuse);
	shader->SetUniform("light.specular", specular);

	std::vector<Camera*> cameras = m_scene->Get<Camera>();
	ASSERT(!cameras.empty());

	glm::mat4 light_view_matrix = m_transform.GetMatrix() * cameras[0]->m_view_matrix;
	shader->SetUniform("light.position", light_view_matrix[3]);
}

void Light::Edit()
{
	ImGui::PushID("Light");
	ImGui::Text("Light");
	ImGui::ColorEdit3("Ambient", (float*)&ambient);
	ImGui::ColorEdit3("Diffuse", (float*)&diffuse);
	ImGui::ColorEdit3("Specular", (float*)&specular);
	ImGui::SliderFloat3("Position", (float*)&m_transform.translation, -100, 100);
	ImGui::PopID();
}
