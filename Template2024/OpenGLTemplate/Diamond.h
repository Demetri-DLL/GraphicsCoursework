#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"

// Class for generating a unit sphere
class CDiamond
{
public:
	CDiamond();
	~CDiamond();
	void Create();
	void Render();
	void Release();
private:
	UINT m_vao;
	//CVertexBufferObject m_vbo;
	CTexture m_texture;
	string m_directory;
	string m_filename;
	int m_numTriangles;
};