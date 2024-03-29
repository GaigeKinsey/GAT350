#include "mesh.h"

bool Mesh::Create(const Name& name)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec2> texcoords;

	Mesh::Load(name.c_str(), positions, normals, texcoords);
		
	if (normals.empty())
	{
		for (size_t i = 0; i < positions.size(); i += 3)
		{
			glm::vec3 normal = math::calculate_normal(positions[i + 0], positions[i + 1], positions[i + 2]);
			normals.push_back(normal);
			normals.push_back(normal);
			normals.push_back(normal);
		}
	}

	if (tangents.empty())
	{
		for (size_t i = 0; i < positions.size() - 2; i += 3)
		{
			glm::vec3 tangent = math::calculate_tangent(positions[i + 0], positions[i + 1], positions[i + 2], texcoords[i + 0], texcoords[i + 1], texcoords[i + 2]);
			tangents.push_back(tangent);
			tangents.push_back(tangent);
			tangents.push_back(tangent);
		}

		// Gram-Schmidt orthogonalize
		for (size_t i = 0; i < tangents.size(); i++)
		{
			tangents[i] = glm::normalize(tangents[i] - (normals[i] * glm::dot(normals[i], tangents[i])));
		}
	}

	if (!positions.empty())
	{
		m_vertex_array.CreateBuffer(VertexArray::POSITION, static_cast<GLsizei>(positions.size() * sizeof(glm::vec3)), static_cast<GLsizei>(positions.size()), (void*)&positions[0]);
		m_vertex_array.SetAttribute(VertexArray::POSITION, 3, 0, 0);
	}
	if (!normals.empty())
	{
		m_vertex_array.CreateBuffer(VertexArray::NORMAL, static_cast<GLsizei>(normals.size() * sizeof(glm::vec3)), static_cast<GLsizei>(normals.size()), (void*)&normals[0]);
		m_vertex_array.SetAttribute(VertexArray::NORMAL, 3, 0, 0);
	}
	if (!texcoords.empty())
	{
		m_vertex_array.CreateBuffer(VertexArray::TEXCOORD, static_cast<GLsizei>(texcoords.size() * sizeof(glm::vec2)), static_cast<GLsizei>(texcoords.size()), (void*)&texcoords[0]);
		m_vertex_array.SetAttribute(VertexArray::TEXCOORD, 2, 0, 0);
	}
	if (!tangents.empty())
	{
		m_vertex_array.CreateBuffer(VertexArray::TANGENT, static_cast<GLsizei>(tangents.size() * sizeof(glm::vec3)), static_cast<GLsizei>(tangents.size()), (void*)&tangents[0]);
		m_vertex_array.SetAttribute(VertexArray::TANGENT, 3, 0, 0);
	}

	return true;
}

void Mesh::Draw(GLenum primitiveType)
{
	(m_flags[ENABLE_DEPTH_TEST]) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	if (m_flags[CULL_FRONT]) glCullFace(GL_FRONT);
	if (m_flags[CULL_BACK]) glCullFace(GL_BACK);

	m_material->Use();
	m_vertex_array.Draw(primitiveType);
}

void Mesh::SetShader(Program* shader)
{
	m_material->SetShader(shader);
}

bool Mesh::Load(const std::string& filename, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& texcoords)
{
	std::vector<glm::vec3> mesh_positions;
	std::vector<glm::vec3> mesh_normals;
	std::vector<glm::vec2> mesh_texcoords;

#if 1
	std::ifstream stream(filename, std::ios::binary);
	if (!stream.is_open())
	{
		SDL_Log("Error opening file: %s", filename.c_str());
		return false;
	}
#else
	std::string stream_string;
	filesystem::read_file(filename.c_str(), stream_string);
	std::istringstream stream(stream_string);
#endif

	std::string line;
	while (std::getline(stream, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			std::istringstream string_stream(line.substr(2));
			glm::vec3 position;
			string_stream >> position.x;
			string_stream >> position.y;
			string_stream >> position.z;
			mesh_positions.push_back(position);
		}
		else if (line.substr(0, 3) == "vn ")
		{
			std::istringstream string_stream(line.substr(3));
			glm::vec3 normal;
			string_stream >> normal.x;
			string_stream >> normal.y;
			string_stream >> normal.z;
			mesh_normals.push_back(normal);
		}
		else if (line.substr(0, 3) == "vt ")
		{
			std::istringstream string_stream(line.substr(3));
			glm::vec2 texcoord;
			string_stream >> texcoord.x;
			string_stream >> texcoord.y;
			mesh_texcoords.push_back(texcoord);
		}
		if (line.substr(0, 2) == "f ")
		{
			std::istringstream string_stream(line.substr(2));
			std::string vertex_string;
			while (std::getline(string_stream, vertex_string, ' '))
			{
				std::istringstream vertex_stream(vertex_string);
				std::string index_string;

				size_t i = 0;
				u32 index[3] = { 0, 0, 0 };
				while (std::getline(vertex_stream, index_string, '/'))
				{
					if (!index_string.empty())
					{
						std::istringstream index_stream(index_string);
						index_stream >> index[i];
					}
					i++;
				}

				if (index[0])
				{
					glm::vec3 position = mesh_positions[index[0] - 1];
					positions.push_back(position);
				}

				if (index[1])
				{
					glm::vec2 texcoord = mesh_texcoords[index[1] - 1];
					texcoords.push_back(texcoord);
				}

				if (index[2])
				{
					glm::vec3 normal = mesh_normals[index[2] - 1];
					normals.push_back(normal);
				}
			}
		}
	}

	stream.close();

	return true;
}

