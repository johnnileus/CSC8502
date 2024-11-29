#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include "SceneNodeVariants.h"




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

    void DrawWater();
    void DrawSkybox();

    void DrawShadowScene();
    void DrawMainScene();

    GLuint shadowTex;
    GLuint shadowFBO;

    GLuint depthTex;
    GLuint depthTexFBO;

    GLuint bufferColourTex[2];
    GLuint bufferFBO;
    GLuint processFBO;

    GLuint sceneDiffuse;
    GLuint sceneBump;
    float sceneTime;

    Shader* sceneShader;
    Shader* shadowShader;
    Shader* skyboxShader;
    Shader* lightShader;
    Shader* reflectShader;
    Shader* fullScreenShader;
    Shader* blurShader;
    Shader* robotShader;

    vector<Mesh*> sceneMeshes;
    vector<Matrix4> sceneTransforms;

    HeightMap* heightMap;
    HeightMapNode* map;
    HeightMapNode* procMap;
    Mesh* skyboxQuad;

    Camera* camera;
    Light* mainLight;
    Light* sideLight;

    GLuint cubeMap;
    GLuint waterTex;

    RobotNode* robot;

    SceneNode* root;
    Mesh* quad;
    Mesh* cube;
    Shader* testPlaneShader;
    GLuint planeTexture;
    Frustum frameFrustum;

    float waterRotate;
    float waterCycle;


    vector<SceneNode*> transparentNodeList;
    vector<SceneNode*> nodeList;

    bool enableFog;
    bool enableBlur;
    bool enableBloom;
    bool enableRotating;




};
