#include "Renderer.h"
#include <algorithm> // For std::sort ...


#include "../nclgl/Light.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Heightmap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/CubeRobot.h"





#define SHADOWSIZE 2048

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

    root = new SceneNode();
    cube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");
    root->AddChild(new CubeRobot(cube));


    heightMap = new HeightMap(TEXTUREDIR "noise.png");
    Vector3 heightmapSize = heightMap->GetHeightmapSize();


    skyboxQuad = Mesh::GenerateQuad();
    camera = new Camera(-30.0f, 315.0f, Vector3(-8.0f, 5.0f, 8.0f));

    light = new Light(
        heightmapSize * Vector3(0.5f, 1.5f, 0.5f),
        Vector4(1, 1, 1, 1), heightmapSize.x
    );


    cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR "rusted_west.jpg", TEXTUREDIR "rusted_east.jpg",TEXTUREDIR "rusted_up.jpg", TEXTUREDIR "rusted_down.jpg",TEXTUREDIR "rusted_south.jpg", TEXTUREDIR "rusted_north.jpg",SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
    earthTex = SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
    earthBump = SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

    if (!cubeMap || !earthTex || !earthBump) {
        return;
    }

    SetTextureRepeating(earthTex, true);
    SetTextureRepeating(earthBump, true);
    //SetTextureRepeating(waterTex, true);

    sceneShader = new Shader("shadowscenevert.glsl", "shadowscenefrag.glsl");
    shadowShader = new Shader("shadowVert.glsl", "shadowFrag.glsl");
    skyboxShader = new Shader(
        "skyboxVertex.glsl", "skyboxFragment.glsl"
    );

    lightShader = new Shader(
        "PerPixelVertex.glsl", "PerPixelFragment.glsl"
    );

    shader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");



    if (!sceneShader->LoadSuccess() ||
        !shadowShader->LoadSuccess() || 
        !skyboxShader->LoadSuccess() ||
        !lightShader->LoadSuccess() || 
        !shader -> LoadSuccess()) {
        return;
    }



    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    sceneMeshes.emplace_back(Mesh::GenerateQuad());
    sceneMeshes.emplace_back(Mesh::LoadFromMeshFile("Sphere.msh"));
    sceneMeshes.emplace_back(Mesh::LoadFromMeshFile("Cylinder.msh"));
    sceneMeshes.emplace_back(Mesh::LoadFromMeshFile("Cone.msh"));

    sceneDiffuse = SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG",
        SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
    sceneBump = SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG",
        SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

    SetTextureRepeating(sceneDiffuse, true);
    SetTextureRepeating(sceneBump, true);
    glEnable(GL_DEPTH_TEST);

    sceneTransforms.resize(4);
    sceneTransforms[0] = Matrix4::Rotation(90, Vector3(1, 0, 0)) *
        Matrix4::Scale(Vector3(10, 10, 1));
    sceneTime = 0.0f;
    init = true;

}

Renderer::~Renderer(void) {
    glDeleteTextures(1, &shadowTex);
    glDeleteFramebuffers(1, &shadowFBO);

    for (auto& i : sceneMeshes) {
        delete i;
    }

    delete camera;
    delete sceneShader;
    delete shadowShader;
}
void Renderer::UpdateScene(float dt) {
    camera->UpdateCamera(dt);
    sceneTime += dt;

    viewMatrix = camera->BuildViewMatrix();
    frameFrustum.FromMatrix(projMatrix * viewMatrix);

    root->Update(dt);

    for (int i = 1; i < 4; ++i) { // skip the floor!
        Vector3 t = Vector3(-10 + (5 * i), 2.0f + sin(sceneTime * i), 0);
        sceneTransforms[i] = Matrix4::Translation(t) *
            Matrix4::Rotation(sceneTime * 10 * i, Vector3(1, 0, 0));
    }
}

void Renderer::RenderScene() {

    BuildNodeLists(root);
    SortNodeLists();


    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    BindShader(shader);
    UpdateShaderMatrices();

    glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
    DrawNodes();
    ClearNodeLists();
    DrawSkybox();

    DrawHeightmap();
    DrawShadowScene();
    DrawMainScene();
}

void Renderer::DrawShadowScene() {
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    BindShader(shadowShader);
    viewMatrix = Matrix4::BuildViewMatrix(
        light->GetPosition(), Vector3(0, 0, 0));
    projMatrix = Matrix4::Perspective(1, 100, 1, 45);
    shadowMatrix = projMatrix * viewMatrix; // used later

    for (int i = 0; i < 4; ++i) {
        modelMatrix = sceneTransforms[i];
        UpdateShaderMatrices();
        sceneMeshes[i]->Draw();
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::BuildNodeLists(SceneNode* from) {
    if (frameFrustum.InsideFrustum(*from)) {
        Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
        from->SetCameraDistance(Vector3::Dot(dir, dir));

        if (from->GetColour().w < 1.0f) {
            transparentNodeList.push_back(from);
        }
        else {
            nodeList.push_back(from);
        }
    }

    for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart();
        i != from->GetChildIteratorEnd(); ++i) {
        BuildNodeLists((*i));
    }
}

void Renderer::SortNodeLists() {
    std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
    std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes() {
    for (const auto& i : nodeList) {
        DrawNode(i);
    }
    for (const auto& i : transparentNodeList) {
        DrawNode(i);
    }
}

void Renderer::DrawNode(SceneNode* n) {
    if (n->GetMesh()) {
        Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
        glUniformMatrix4fv(
            glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values
        );

        glUniform4fv(
            glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour()
        );

        texture = n->GetTexture();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glUniform1i(
            glGetUniformLocation(shader->GetProgram(), "useTexture"), texture
        );

        n->Draw(*this);
    }
}

void Renderer::DrawHeightmap() {
    BindShader(lightShader);
    SetShaderLight(*light);
    glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

    glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, earthTex);

    glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, earthBump);

    modelMatrix.ToIdentity(); // New!
    textureMatrix.ToIdentity(); // New!

    UpdateShaderMatrices();

    heightMap->Draw();
}

void Renderer::DrawMainScene() {
    BindShader(sceneShader);
    SetShaderLight(*light);
    viewMatrix = camera->BuildViewMatrix();
    projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
        (float)width / (float)height, 45.0f);

    glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
    glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex"), 1);
    glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "shadowTex"), 2);

    glUniform3fv(glGetUniformLocation(sceneShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneDiffuse);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sceneBump);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, shadowTex);

    for (int i = 0; i < 4; ++i) {
        modelMatrix = sceneTransforms[i];
        UpdateShaderMatrices();
        sceneMeshes[i]->Draw();
    }


}

void Renderer::ClearNodeLists() {
    transparentNodeList.clear();
    nodeList.clear();
}
void Renderer::DrawSkybox() {
    glDepthMask(GL_FALSE);

    BindShader(skyboxShader);
    UpdateShaderMatrices();

    skyboxQuad->Draw();

    glDepthMask(GL_TRUE);
}

