#pragma once
#include "SceneNodeVariants.h"


void TestPlane::Draw(OGLRenderer& r) {
    if (mesh) {


        r.BindShader(shader);
        Matrix4 model = GetWorldTransform() * Matrix4::Scale(GetModelScale());
        glUniformMatrix4fv(glGetUniformLocation(GetShader()->GetProgram(), "modelMatrix"), 1, false, model.values);

        glUniform4fv(glGetUniformLocation(GetShader()->GetProgram(), "nodeColour"), 1, (float*)&GetColour());

        GLuint planeTexture = GetTexture();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, planeTexture);

        glUniform1i(glGetUniformLocation(GetShader()->GetProgram(), "useTexture"), planeTexture);
        mesh->Draw();
    }
}

void HeightMapNode::Draw(OGLRenderer& r) {
    OGLRenderer& render = r;
    render.BindShader(shader);
    render.SetShaderLight(*light);
    glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

    glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, earthTex);

    glUniform1i(glGetUniformLocation(shader->GetProgram(), "bumpTex"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, earthBump);

    // Bind grass texture
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "grassTex"), 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, grassTex);

    // Bind grass bump map
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "grassBump"), 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, grassBump);

    Vector3 heightmapSize = heightMap->GetHeightmapSize();
    render.modelMatrix = Matrix4::Translation(Vector3(-heightmapSize.x/2.0f, 0.0f, -heightmapSize.z/2.0f)) *
        Matrix4::Scale(modelScale) *
        Matrix4::Rotation(0, Vector3(1, 0, 0));
    render.textureMatrix.ToIdentity(); // New!

    render.UpdateShaderMatrices();

    heightMap->Draw();
}

void WaterNode::Draw(OGLRenderer& r) {
    r.BindShader(shader);

    glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

    glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "cubeTex"), 2);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "bumpTex"), 3);

    float col[4] = { colour.x, colour.y, colour.x, colour.w};
    glUniform4fv(glGetUniformLocation(shader->GetProgram(), "waterColour"), 1, col);

    glUniform1f(glGetUniformLocation(shader->GetProgram(), "time"), r.GetTime());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waterTex);


    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);


    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, bumpTex);

    r.modelMatrix =
        Matrix4::Translation(Vector3(0,600.0f,0)) *
        Matrix4::Scale(hSize * 0.5f) *
        Matrix4::Rotation(90, Vector3(1, 0, 0));

    r.textureMatrix =
        Matrix4::Translation(Vector3(0.0f, 0.0f, 0.0f)) *
        Matrix4::Scale(Vector3(10, 10, 10)) *
        Matrix4::Rotation(0.0f, Vector3(0, 0, 1));

    r.UpdateShaderMatrices();
    // SetShaderLight(*light); // No lighting in this shader!
    mesh->Draw();
    r.textureMatrix.ToIdentity();
}