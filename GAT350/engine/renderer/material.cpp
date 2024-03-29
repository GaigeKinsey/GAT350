#include "material.h"

void Material::SetShader(Program* shader)
{
	shader->SetUniform("material.ambient", ambient);
	shader->SetUniform("material.diffuse", diffuse);
	shader->SetUniform("material.specular", specular);
	shader->SetUniform("material.shininess", shininess);
}

void Material::Use()
{
	(blend == TRANSPARENT || blend == ADDITIVE) ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	if (blend == TRANSPARENT) glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	else if (blend == ADDITIVE) glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	for (auto texture : textures)
	{
		texture->Bind();
	}
}

void Material::Edit() {
	ImGui::PushID("Material");
	ImGui::Text("Material");
	ImGui::ColorEdit3("Ambient", (float*)&ambient);
	ImGui::ColorEdit3("Diffuse", (float*)&diffuse);
	ImGui::ColorEdit3("Specular", (float*)&specular);
	ImGui::SliderFloat("Shininess", &shininess, 1.0f, 128.0f);
	ImGui::PopID();
}