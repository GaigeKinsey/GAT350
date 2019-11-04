#include "engine/engine.h"
#include "engine/input/input_system.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/program.h"
#include "engine/renderer/vertex_array.h"
#include "engine/renderer/vertex_index_array.h"
#include "engine/renderer/texture.h"
#include "engine/renderer/material.h"
#include "engine/renderer/light.h"
#include "engine/renderer/mesh.h"
#include "engine/renderer/gui.h"
#include "engine/math/math.h"

int main(int argc, char** argv) {
	Name::AllocNames();
	filesystem::set_current_path("content");

	int result = SDL_Init(SDL_INIT_VIDEO);
	if (result != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
	}

	std::shared_ptr<InputSystem> input = std::make_shared<InputSystem>();
	input->Initialize();

	std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>();
	renderer->Initialize(1280, 720);

	GUI::Initialize(renderer.get());

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	
	Mesh::Load("meshes/sphere.obj", positions, normals, texcoords);
	Transform transform;

	if (normals.empty()) {
		for (size_t i = 0; i < positions.size() - 2; i+=3) {
			glm::vec3 normal = math::calculate_normal(positions[i], positions[i + 1], positions[i + 2]);
			normals.push_back(normal);
			normals.push_back(normal);
			normals.push_back(normal);
		}
	}

	//glm::mat3 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//math::transform(positions, rotate);
	//math::transform(normals, rotate);

	VertexArray vertex_array;
	if (!positions.empty()) {
		vertex_array.CreateBuffer(VertexArray::POSITION, 
			static_cast<GLsizei>(positions.size() * sizeof(glm::vec3)),
			static_cast<GLsizei>(positions.size()), (void*)&positions[0]);

		vertex_array.SetAttribute(VertexArray::POSITION, 3, 0, 0);
	}

	if (!normals.empty()) {
		vertex_array.CreateBuffer(VertexArray::NORMAL,
			static_cast<GLsizei>(normals.size() * sizeof(glm::vec3)),
			static_cast<GLsizei>(normals.size()), (void*)&normals[0]);

		vertex_array.SetAttribute(VertexArray::NORMAL, 3, 0, 0);
	}

	if (!texcoords.empty()) {
		vertex_array.CreateBuffer(VertexArray::TEXCOORD,
			static_cast<GLsizei>(texcoords.size() * sizeof(glm::vec2)),
			static_cast<GLsizei>(texcoords.size()), (void*)&texcoords[0]);

		vertex_array.SetAttribute(VertexArray::TEXCOORD, 2, 0, 0);
	}

	Program* shader = new Program();
	shader->CompileShaderFromFile("shaders/texture_phong.vert", GL_VERTEX_SHADER);
	shader->CompileShaderFromFile("shaders/texture_phong_fog.frag", GL_FRAGMENT_SHADER);
	shader->Link();
	shader->Use();

	Material material;
	material.ambient = glm::vec3(1.0f);
	material.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	material.specular = glm::vec3(1.0f);
	material.shininess = 32.0f;

	Texture* texture = new Texture();
	texture->CreateTexture("textures/uvgrid.jpg");
	material.textures.push_back(texture);

	material.SetShader(shader);
	material.Use();

	Light light;
	light.GetTransform().translation = glm::vec3(5.0f);
	light.ambient = glm::vec3(0.1f);
	light.diffuse = glm::vec3(1.0f);
	light.specular = glm::vec3(1.0f);

	glm::mat4 mxProjection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.01f, 1000.0f);
	glm::mat4 mxView = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	int steps = 4;

	bool quit = false;
	while (!quit) {
		SDL_Event event;
		SDL_PollEvent(&event);

		switch (event.type) {
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				quit = true;
			}
			break;
		}

		SDL_PumpEvents();

		g_timer.tick();

		glm::vec3 translate(0.0f, 0.0f, 0.0f);
		float speed = 10.0f;
		if (input->GetKey(SDL_SCANCODE_RIGHT))	translate.x = speed;
		if (input->GetKey(SDL_SCANCODE_LEFT))	translate.x = -speed;
		if (input->GetKey(SDL_SCANCODE_UP))		translate.y = speed;
		if (input->GetKey(SDL_SCANCODE_DOWN))	translate.y = -speed;
		if (input->GetKey(SDL_SCANCODE_W))		translate.z = speed;
		if (input->GetKey(SDL_SCANCODE_S))		translate.z = -speed;

		transform.translation = transform.translation + translate * g_timer.dt();
		transform.rotation = transform.rotation * glm::angleAxis(glm::radians(45.0f) * g_timer.dt(), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 model_view_matrix = mxView * transform.GetMatrix();
		shader->SetUniform("model_view_matrix", model_view_matrix);
		glm::mat4 mvp_matrix = mxProjection * model_view_matrix;
		shader->SetUniform("mvp_matrix", mvp_matrix);

		//set uniforms
		material.SetShader(shader);
		light.SetShader(shader, mxView);
		//shader->SetUniform("steps", steps);
		shader->SetUniform("fog.min_distance", 10.0f);
		shader->SetUniform("fog.max_distance", 30.0f);
		shader->SetUniform("fog.color", glm::vec3(0.85f));

		GUI::Update(event);
		GUI::Begin(renderer.get());

		ImGui::Text("Hello World!");
		light.Edit();
		material.Edit();
		ImGui::SliderInt("Steps", &steps, 1, 16);
		GUI::End();

		renderer->ClearBuffer();

		GUI::Draw();
		vertex_array.Draw();

		renderer->SwapBuffer();
	}

	GUI::Shutdown();
	delete shader;
	material.Destroy();

	input->Shutdown();
	renderer->Shutdown();

	SDL_Quit();

	Name::FreeNames();

	return 0;
}