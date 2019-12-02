#include "framebuffer.h"

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_framebuffer);
}

bool Framebuffer::Create(const Name& name)
{
	m_name = name;

	glGenFramebuffers(1, &m_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

	return true;
}

void Framebuffer::AttachTexture(const std::shared_ptr<Texture>& texture, GLenum attachment)
{
	m_texture = texture;

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_texture->m_texture, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	ASSERT(result == GL_FRAMEBUFFER_COMPLETE);
}

void Framebuffer::CreateDepthbuffer(u32 width, u32 height)
{

}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

 void Framebuffer::Unbind()
{
	glFlush();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}