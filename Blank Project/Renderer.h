#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"




class Camera;
class Mesh;
class HeightMap;

class Renderer : public OGLRenderer {
public:
    Renderer(Window& parent);
    ~Renderer(void);

    void UpdateScene(float dt) override;
    void RenderScene() override;

protected:

    void BuildNodeLists(SceneNode* from);
    void SortNodeLists();
    void ClearNodeLists();
    void DrawNodes();
    void DrawNode(SceneNode* n);

    void DrawHeightmap();
    void DrawWater();
    void DrawSkybox();

    void DrawShadowScene();
    void DrawMainScene();

    GLuint shadowTex;
    GLuint shadowFBO;


    GLuint sceneDiffuse;
    GLuint sceneBump;
    float sceneTime;

    Shader* sceneShader;
    Shader* shadowShader;
    Shader* skyboxShader;
    Shader* lightShader;

    vector<Mesh*> sceneMeshes;
    vector<Matrix4> sceneTransforms;

    HeightMap* heightMap;
    Mesh* skyboxQuad;

    Camera* camera;
    Light* light;

    GLuint cubeMap;
    GLuint waterTex;
    GLuint earthTex;
    GLuint earthBump;

    SceneNode* root;
    Mesh* quad;
    Mesh* cube;
    Shader* shader;
    GLuint texture;
    Frustum frameFrustum;



    vector<SceneNode*> transparentNodeList;
    vector<SceneNode*> nodeList;

};
