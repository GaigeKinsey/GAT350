#include "transform.h"

void Transform::Edit()
{
	ImGui::Separator();
	ImGui::Text("Transform");

	ImGui::SliderFloat3("Translation", (float*)&translation, -1000.0f, 1000.0f);
	ImGui::SliderFloat3("Scale", (float*)&scale, 0.0f, 100.0f);

	ImGui::gizmo3D("Rotation", rotation, 200.0f, imguiGizmo::mode3Axes | imguiGizmo::sphereAtOrigin);

	glm::vec3 euler = glm::degrees(glm::eulerAngles(rotation));
	std::string rotation_string = "X: " + std::to_string(euler.x) + " Y: " + std::to_string(euler.y) + " Z: " + std::to_string(euler.z);
	ImGui::Text(rotation_string.c_str());
}
