#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();

	void UpdateTextureMatrix(float rotation);
	void ToggleRepeating();
	void ToggleFiltering();


protected:
	Camera* camera;
	Shader* matrixShader;
	float scale;
	float rotation;
	Vector3 position;

	Shader* shader;
	Mesh* triangle;
	GLuint texture;
	bool filtering;
	bool repeating;
};