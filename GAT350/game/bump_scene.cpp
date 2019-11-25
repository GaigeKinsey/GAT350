#include "bump_scene.h"
#include "../engine/engine.h"
#include "../engine/renderer/renderer.h"
#include "../engine/renderer/program.h"
#include "../engine/renderer/texture.h"
#include "../engine/renderer/material.h"
#include "../engine/renderer/light.h"
#include "../engine/renderer/mesh.h"
#include "../engine/renderer/model.h"
#include "../engine/renderer/camera.h"
#include "../engine/renderer/gui.h"
#include "../engine/editor/editor.h"

bool BumpScene::Create(const Name& name)
{
	// shader
	{
		auto shader = m_engine->Factory()->Create<Program>(Program::GetClassName());
		shader->m_name = "shader";
		shader->m_engine = m_engine;
		shader->CreateShaderFromFile("shaders/texture_phong_normal.vert", GL_VERTEX_SHADER);
		shader->CreateShaderFromFile("shaders/texture_phong_normal.frag", GL_FRAGMENT_SHADER);
		shader->Link();
		m_engine->Resources()->Add("phong_shader", std::move(shader));
	}

	{
		auto shader = m_engine->Factory()->Create<Program>(Program::GetClassName());
		shader->m_name = "shader";
		shader->m_engine = m_engine;
		shader->CreateShaderFromFile("shaders/texture_phong.vert", GL_VERTEX_SHADER);
		shader->CreateShaderFromFile("shaders/texture_phong_fx.frag", GL_FRAGMENT_SHADER);
		shader->Link();
		m_engine->Resources()->Add("phong_shader_fx", std::move(shader));
	}

	{
		auto shader = m_engine->Factory()->Create<Program>(Program::GetClassName());
		shader->m_name = "shader";
		shader->m_engine = m_engine;
		shader->CreateShaderFromFile("shaders/basic_color.vert", GL_VERTEX_SHADER);
		shader->CreateShaderFromFile("shaders/basic.frag", GL_FRAGMENT_SHADER);
		shader->Link();
		m_engine->Resources()->Add("debug_shader", std::move(shader));
	}

	// material
	auto material = m_engine->Factory()->Create<Material>(Material::GetClassName());
	material->m_name = "material";
	material->m_engine = m_engine;
	material->ambient = glm::vec3(1.0f);
	material->diffuse = glm::vec3(1.0f);
	material->specular = glm::vec3(8.0f);
	material->shininess = 128.0f;

	// texture
	{
		auto texture = m_engine->Resources()->Get<Texture>("textures/ogre_diffuse.bmp");
		material->textures.push_back(texture);
	}
	{
		auto texture = m_engine->Resources()->Get<Texture>("textures/ogre_normal.bmp");
		texture->m_unit = GL_TEXTURE1;
		material->textures.push_back(texture);
	}
	m_engine->Resources()->Add("material", std::move(material));

	material = m_engine->Factory()->Create<Material>(Material::GetClassName());
	material->m_name = "debug_material";
	material->m_engine = m_engine;
	material->ambient = glm::vec3(1.0f);
	material->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	material->specular = glm::vec3(1.0f);
	material->shininess = 128.0f;
	m_engine->Resources()->Add("debug_material", std::move(material));

	// scene actors

	// model
	auto model = m_engine->Factory()->Create<Model>(Model::GetClassName());
	model->m_name = "model1";
	model->m_engine = m_engine;
	model->m_scene = this;
	model->m_transform.translation = glm::vec3(0.0f);
	model->m_transform.scale = glm::vec3(1);
	model->m_mesh = m_engine->Resources()->Get<Mesh>("meshes/ogre.obj");
	model->m_mesh->m_material = m_engine->Resources()->Get<Material>("material");
	model->m_shader = m_engine->Resources()->Get<Program>("phong_shader");
	Add(std::move(model));

	//model = m_engine->Factory()->Create<Model>(Model::GetClassName());
	//model->m_name = "model2";
	//model->m_engine = m_engine;
	//model->m_scene = this;
	//model->m_transform.translation = glm::vec3(0.0f, -2.0f, 0.0f);
	//model->m_transform.scale = glm::vec3(10.0f);
	//model->m_mesh = m_engine->Resources()->Get<Mesh>("meshes/plane.obj");
	//model->m_mesh->m_material = m_engine->Resources()->Get<Material>("material");
	//model->m_shader = m_engine->Resources()->Get<Program>("phong_shader");
	//Add(std::move(model));

	// light
	auto light = m_engine->Factory()->Create<Light>(Light::GetClassName());
	light->m_name = "light";
	light->m_engine = m_engine;
	light->m_scene = this;
	light->Create("light");
	light->m_transform.translation = glm::vec3(0.0f, 2.0f, 1.0f);
	light->m_transform.rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));
	light->ambient = glm::vec3(0.3f);
	light->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	light->specular = glm::vec3(1.0f);
	light->cutoff = 30.0f;
	light->exponent = 64.0f;
	Add(std::move(light));

	// camera
	auto camera = m_engine->Factory()->Create<Camera>(Camera::GetClassName());
	camera->m_name = "camera";
	camera->m_engine = m_engine;
	camera->m_scene = this;
	camera->Create("camera");
	camera->m_transform.translation = glm::vec3(0.0f, 0.0f, 5.0f);
	camera->m_transform.rotation = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	camera->SetProjection(45.0f, 1280.0f / 720.0f, 0.01f, 100.0f);

	Add(std::move(camera));

	return true;
}

void BumpScene::Update()
{
	Scene::Update();

	// set shader uniforms
	Light* light = Get<Light>("light");
	light->m_transform.translation = light->m_transform.translation * glm::angleAxis(glm::radians(45.0f) * g_timer.dt(), glm::vec3(0, 0, 1));
	light->SetShader(m_engine->Resources()->Get<Program>("phong_shader").get());

	// gui
	GUI::Update(m_engine->GetEvent());
	GUI::Begin(m_engine->Get<Renderer>());
	m_engine->Get<Editor>()->UpdateGUI();
	GUI::End();
}

void BumpScene::Draw()
{
	m_engine->Get<Renderer>()->ClearBuffer();

	Scene::Draw();
	GUI::Draw();

	m_engine->Get<Renderer>()->SwapBuffer();
}
