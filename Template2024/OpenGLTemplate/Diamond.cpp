#include "Common.h"
#include "Diamond.h"


#define _USE_MATH_DEFINES
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


CDiamond::CDiamond()
{}

CDiamond::~CDiamond()
{}

// Create a unit sphere 
void CDiamond::Create()
{

	GLfloat diamondPos[] = { 0.0f, 40.f, 0.0f, -10.0f, 20.0f, 10.0f, 10.0f, 20.0f, 10.0f, 
		0.0f, 40.0f, 0.0, -10.0f, 20.0f, -10.0f, -10.0f, 20.0f, 10.0f,
		0.0f, 40.0f, 0.0, 10.0f, 20.0f, -10.0f, -10.0f, 20.0f, -10.0f,
		0.0f, 40.0f, 0.0, 10.0f, 20.0f, 10.0f, 10.0f, 20.0f, -10.0f,

		0.0f, 0.0f, 0.0f, 10.0f, 20.0f, 10.0f, -10.0f, 20.0f, 10.0f,
		0.0f, 0.0f, 0.0, -10.0f, 20.0f, 10.0f, -10.0f, 20.0f, -10.0f,
		0.0f, 0.0f, 0.0, -10.0f, 20.0f, -10.0f, 10.0f, 20.0f, -10.0f,
		0.0f, 0.0f, 0.0, 10.0f, 20.0f, -10.0f, 10.0f, 20.0f, 10.0f,
	};

//(0.0f, 1.f, 0.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f           , 0.0f, 1.0f, 0.0f, -1.0f, -1.0f, 1.0f,0.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f


	GLfloat diamondCol[] = { 
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(diamondPos) + sizeof(diamondCol), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(diamondPos), diamondPos);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(diamondPos), sizeof(diamondCol), diamondCol);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (CONST GLvoid*) sizeof(diamondPos));

}

// Render the sphere as a set of triangles
void CDiamond::Render()
{
	glBindVertexArray(m_vao);
	//m_texture.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 24);

}

// Release memory on the GPU 
void CDiamond::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	//m_vbo.Release();
}