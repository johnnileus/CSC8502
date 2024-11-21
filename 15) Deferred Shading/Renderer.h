#pragma once
#include "../nclgl/OGLRenderer.h"

class Camera;
class Mesh;
class HeightMap;

class Renderer : public OGLRenderer {
public:
    Renderer(Window& parent);
    ~Renderer(void);

    void RenderScene() override;
    void UpdateScene(float dt) override;

protected:
    void FillBuffers();             // G-Buffer Fill Render Pass
    void DrawPointLights();         // Lighting Render Pass
    void CombineBuffers();          // Combination Render Pass
    void GenerateScreenTexture(GLuint& into, bool depth = false);

    Shader* sceneShader;            // Shader to fill our G-Buffers
    Shader* pointlightShader;       // Shader to calculate lighting
    Shader* combineShader;          // Shader to stick it all together

    GLuint bufferFBO;               // FBO for our G-Buffer pass
    GLuint bufferColourTex;         // Albedo goes here
    GLuint bufferNormalTex;         // Normals go here
    GLuint bufferDepthTex;          // Depth goes here

    GLuint pointLightFBO;           // FBO for our lighting pass
    GLuint lightDiffuseTex;         // Store diffuse lighting
    GLuint lightSpecularTex;        // Store specular lighting

    HeightMap* heightMap;           // Terrain
    Light* pointLights;             // Array of lighting data
    Mesh* sphere;                   // Light volume
    Mesh* quad;                     // To draw a full-screen quad
    Camera* camera;                 // Our usual camera
    GLuint earthTex;                // Texture for the terrain
    GLuint earthBump;               // Bump map for the terrain
};
