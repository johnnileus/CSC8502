#pragma once
#include "../nclgl/OGLRenderer.h"

class HeightMap;
class Camera;

class Renderer : public OGLRenderer {
public:
    Renderer(Window& parent);
    ~Renderer(void);
    void RenderScene() override;
    void UpdateScene(float dt) override;
protected:
    HeightMap* heightMap;
    Shader* testPlaneShader;
    Camera* camera;
    GLuint terrainTex;
};

inline Renderer::~Renderer(void) {
    delete heightMap;
    delete camera;
    delete testPlaneShader;
}

