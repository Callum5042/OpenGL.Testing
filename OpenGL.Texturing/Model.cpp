#include "Model.h"
#include "Application.h"
#include <SDL_messagebox.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL.h>
#include <iostream>
#include <SDL_timer.h>
#include "LoadDDS.h"

glm::mat4 tex_transform2 = glm::mat4(1.0f);

struct Vertex
{
	Vertex(float x, float y, float z, float u, float v) : x(x), y(y), z(z), u(u), v(v) {}

	float x;
	float y;
	float z;

	float u;
	float v;
};

GL::Model::~Model()
{
	glDeleteTextures(1, &m_DiffuseTextureId);

	glDeleteVertexArrays(1, &m_VertexArrayObject);
	glDeleteBuffers(1, &m_VertexBuffer);
	glDeleteBuffers(1, &m_IndexBuffer);
}

void GL::Model::Load()
{
	Vertex vertices[] =
	{
		{ -1.0f, -1.0f, -1.0f, 0.0f, 1.0f },
		{ -1.0f, +1.0f, -1.0f, 0.0f, 0.0f },
		{ +1.0f, +1.0f, -1.0f, 1.0f, 0.0f },
		{ +1.0f, -1.0f, -1.0f, 1.0f, 1.0f },

		{ -1.0f, -1.0f, +1.0f, 1.0f, 1.0f },
		{ +1.0f, -1.0f, +1.0f, 0.0f, 1.0f },
		{ +1.0f, +1.0f, +1.0f, 0.0f, 0.0f },
		{ -1.0f, +1.0f, +1.0f, 1.0f, 0.0f },

		{ -1.0f, +1.0f, -1.0f, 0.0f, 1.0f },
		{ -1.0f, +1.0f, +1.0f, 0.0f, 0.0f },
		{ +1.0f, +1.0f, +1.0f, 1.0f, 0.0f },
		{ +1.0f, +1.0f, -1.0f, 1.0f, 1.0f },

		{ -1.0f, -1.0f, -1.0f, 1.0f, 1.0f },
		{ +1.0f, -1.0f, -1.0f, 0.0f, 1.0f },
		{ +1.0f, -1.0f, +1.0f, 0.0f, 0.0f },
		{ -1.0f, -1.0f, +1.0f, 1.0f, 0.0f },

		{ -1.0f, -1.0f, +1.0f, 0.0f, 1.0f },
		{ -1.0f, +1.0f, +1.0f, 0.0f, 0.0f },
		{ -1.0f, +1.0f, -1.0f, 1.0f, 0.0f },
		{ -1.0f, -1.0f, -1.0f, 1.0f, 1.0f },

		{ +1.0f, -1.0f, -1.0f, 0.0f, 1.0f },
		{ +1.0f, +1.0f, -1.0f, 0.0f, 0.0f },
		{ +1.0f, +1.0f, +1.0f, 1.0f, 0.0f },
		{ +1.0f, -1.0f, +1.0f, 1.0f, 1.0f }
	};

	GLuint indices[] =
	{
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23,
	};

	// Vertex Array Object
	glCreateVertexArrays(1, &m_VertexArrayObject);
	glBindVertexArray(m_VertexArrayObject);

	// Vertex Buffer
	glCreateBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

	glNamedBufferStorage(m_VertexBuffer, sizeof(vertices), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(m_VertexBuffer, 0, sizeof(vertices), vertices);

	// Index Buffer
	glCreateBuffers(1, &m_IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Something pipeline
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);


	tex_transform2 = glm::scale(tex_transform2, glm::vec3(-1.0f, 1.0f, 1.0f));
}

void GL::Model::Render()
{
	Application* app = reinterpret_cast<Application*>(Engine::Get());

	// Update shader transform
	glm::mat4 transform = glm::mat4(1.0f);
	unsigned int transformLoc = glGetUniformLocation(app->Shader()->ShaderId(), "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	// Update shader view
	unsigned int viewLoc = glGetUniformLocation(app->Shader()->ShaderId(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(app->Camera()->View()));

	// Update shader projection
	unsigned int projLoc = glGetUniformLocation(app->Shader()->ShaderId(), "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(app->Camera()->Projection()));

	// Update texture transform
	glm::mat4 tex_transform = glm::mat4(1.0f);
	tex_transform = glm::scale(tex_transform, glm::vec3(-1.0f, 1.0f, 1.0f));

	unsigned int projTexLoc = glGetUniformLocation(app->Shader()->ShaderId(), "tex_matrix");
	glUniformMatrix4fv(projTexLoc, 1, GL_FALSE, glm::value_ptr(tex_transform));

	// Update texture transform
	tex_transform2 = glm::rotate(tex_transform2, glm::radians(90.0f * (float)app->GetTimer().DeltaTime()), glm::vec3(0.0f, 0.0f, 1.0f));

	unsigned int projTexLoc1 = glGetUniformLocation(app->Shader()->ShaderId(), "tex_matrix2");
	glUniformMatrix4fv(projTexLoc1, 1, GL_FALSE, glm::value_ptr(tex_transform2));

	// Draw
	glBindVertexArray(m_VertexArrayObject);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
} 

void GL::Model::LoadTexture(std::string&& texture_path)
{
	RV::LoadDDS dds;
	dds.Load(std::move(texture_path));

	glCreateTextures(GL_TEXTURE_2D, 1, &m_DiffuseTextureId);
	glTextureStorage2D(m_DiffuseTextureId, dds.MipmapCount(), dds.Format(), dds.Width(), dds.Height());

	for (auto& mipmap : dds.mipmaps)
	{
		glCompressedTextureSubImage2D(m_DiffuseTextureId, mipmap.level, 0, 0, mipmap.width, mipmap.height, dds.Format(), mipmap.texture_size, mipmap.data);
	}

	glBindTextureUnit(0, m_DiffuseTextureId);

	// Sampler
	GLuint sampler;
	glCreateSamplers(1, &sampler);

	glSamplerParameterf(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameterf(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameterf(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameterf(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, GL_MAX_TEXTURE_MAX_ANISOTROPY);
	glSamplerParameterf(sampler, GL_TEXTURE_BASE_LEVEL, 0);
	glSamplerParameterf(sampler, GL_TEXTURE_MAX_LEVEL, 0);

	// Don't know if this is needed?
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);

	glBindSampler(0, sampler);
	glBindSampler(1, sampler);


	/// Second
	RV::LoadDDS dds1;
	std::string path("C:/Users/Callum/Pictures/fireball.dds");
	dds1.Load(path);

	GLuint texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, dds1.MipmapCount(), dds1.Format(), dds1.Width(), dds1.Height());

	for (auto& mipmap : dds1.mipmaps)
	{
		glCompressedTextureSubImage2D(texture, mipmap.level, 0, 0, mipmap.width, mipmap.height, dds1.Format(), mipmap.texture_size, mipmap.data);
	}

	glBindTextureUnit(1, texture);
}