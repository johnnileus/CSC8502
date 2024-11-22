#include "Renderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"

const int LIGHT_NUM = 100;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
    sphere = Mesh::LoadFromMeshFile("Sphere.msh");

    terrainQuad = Mesh::GenerateQuad();
    heightMap = new HeightMap(TEXTUREDIR "noise.png");
    skyboxQuad = Mesh::GenerateQuad();

    earthTex = SOIL_load_OGL_texture(
        TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS
    );

    earthBump = SOIL_load_OGL_texture(
        TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS
    );

    skybox = SOIL_load_OGL_cubemap(
        TEXTUREDIR "rusted_west.jpg", TEXTUREDIR "rusted_east.jpg",
        TEXTUREDIR "rusted_up.jpg", TEXTUREDIR "rusted_down.jpg",
        TEXTUREDIR "rusted_south.jpg", TEXTUREDIR "rusted_north.jpg",
        SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0
    );

    waterTex = SOIL_load_OGL_texture(
        TEXTUREDIR "water.TGA", SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS
    );

    if (!earthTex || !earthBump || !skybox || !waterTex) {
        return;
    }



    SetTextureRepeating(earthTex, true);
    SetTextureRepeating(earthBump, true);
    SetTextureRepeating(waterTex, true);


    skyboxShader = new Shader(
        "skyboxVertex.glsl", "skyboxFragment.glsl"
    );
    lightShader = new Shader(
        "PerPixelVertex.glsl", "PerPixelFragment.glsl"
    );

    reflectShader = new Shader(
        "reflectVertex.glsl", "reflectFragment.glsl"
    );

    if (!reflectShader->LoadSuccess() ||
        !skyboxShader->LoadSuccess() ||
        !lightShader->LoadSuccess()) {
        return;
    }

    Vector3 heightmapSize = heightMap->GetHeightmapSize();

    camera = new Camera(-45.0f, 0.0f,
        heightmapSize * Vector3(0.5f, 5.0f, 0.5f));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    pointLights = new Light[LIGHT_NUM];


    for (int i = 0; i < LIGHT_NUM; ++i) {
        Light& l = pointLights[i];
        l.SetPosition(Vector3(
            rand() % (int)heightmapSize.x,
            150.0f,
            rand() % (int)heightmapSize.z
        ));

        l.SetColour(Vector4(
            0.5f + (float)(rand() / (float)RAND_MAX),
            0.5f + (float)(rand() / (float)RAND_MAX),
            0.5f + (float)(rand() / (float)RAND_MAX),
            1
        ));
        l.SetRadius(250.0f + (rand() % 250));
    }
    sceneShader = new Shader("BumpVertex.glsl", // reused !
        "bufferFragment.glsl");
    pointlightShader = new Shader("pointlightvert.glsl",
        "pointlightfrag.glsl");
    combineShader = new Shader("combinevert.glsl",
        "combinefrag.glsl");

    if (!sceneShader->LoadSuccess() || !pointlightShader->LoadSuccess() ||
        !combineShader->LoadSuccess()) {
        return;
    }
    glGenFramebuffers(1, &bufferFBO);
    glGenFramebuffers(1, &pointLightFBO);

    GLenum buffers[2] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1
    };

    // Generate our scene depth texture ...
    GenerateScreenTexture(bufferDepthTex, true);
    GenerateScreenTexture(bufferColourTex);
    GenerateScreenTexture(bufferNormalTex);
    GenerateScreenTexture(lightDiffuseTex);
    GenerateScreenTexture(lightSpecularTex);

    // And now attach them to our FBOs
    glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
    glDrawBuffers(2, buffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
    glDrawBuffers(2, buffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    waterRotate = 0.0f;
    waterCycle = 0.0f;

    init = true;


}

Renderer::~Renderer(void) {
    delete sceneShader;
    delete combineShader;
    delete pointlightShader;

    delete heightMap;
    delete camera;
    delete sphere;
    delete terrainQuad;
    delete[] pointLights;

    glDeleteTextures(1, &bufferColourTex);
    glDeleteTextures(1, &bufferNormalTex);
    glDeleteTextures(1, &bufferDepthTex);
    glDeleteTextures(1, &lightDiffuseTex);
    glDeleteTextures(1, &lightSpecularTex);

    glDeleteFramebuffers(1, &bufferFBO);
    glDeleteFramebuffers(1, &pointLightFBO);
}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth) {
    glGenTextures(1, &into);
    glBindTexture(GL_TEXTURE_2D, into);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
    GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::UpdateScene(float dt) {
    camera->UpdateCamera(dt);
    viewMatrix = camera->BuildViewMatrix();
    waterRotate += dt * 2.0f;
    waterCycle += dt * 0.25f;
}

void Renderer::RenderScene() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    DrawSkybox();


    FillBuffers();
    DrawPointLights();

    CombineBuffers();


}

void Renderer::DrawSkybox() {
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    BindShader(skyboxShader);
    Matrix4 noTranslation = viewMatrix;
    noTranslation.SetPositionVector(Vector3(0, 0, 0));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader->GetProgram(), "viewMatrix"), 1, false, noTranslation.values);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader->GetProgram(), "projMatrix"), 1, false, projMatrix.values);

    UpdateShaderMatrices();

    skyboxQuad->Draw();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}


void Renderer::FillBuffers() {
    glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    BindShader(sceneShader);
    glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
    glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex"), 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, earthTex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, earthBump);

    modelMatrix.ToIdentity();
    viewMatrix = camera->BuildViewMatrix();
    projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

    UpdateShaderMatrices();

    heightMap->Draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPointLights() {
    glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
    BindShader(pointlightShader);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glBlendFunc(GL_ONE, GL_ONE);
    glCullFace(GL_FRONT);
    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_FALSE);

    glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "depthTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

    glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "normTex"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

    glUniform3fv(glGetUniformLocation(pointlightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
    glUniform2f(glGetUniformLocation(pointlightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

    Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
    glUniformMatrix4fv(glGetUniformLocation(pointlightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

    UpdateShaderMatrices();
    for (int i = 0; i < LIGHT_NUM; ++i) {
        Light& l = pointLights[i];
        SetShaderLight(l);
        sphere->Draw();
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);

    glDepthMask(GL_TRUE);
    glClearColor(0.2f, 0.2f, 0.2f, 1);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CombineBuffers() {
    BindShader(combineShader);


    modelMatrix.ToIdentity();
    viewMatrix = camera->BuildViewMatrix();
    projMatrix = Matrix4::Perspective(1.0f, 1500.0f, float(width) / (float)height, 45.0f);
    UpdateShaderMatrices();

    glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferColourTex);

    glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseLight"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

    glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

    glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

    glEnable(GL_DEPTH_TEST);

    terrainQuad->Draw();
}
