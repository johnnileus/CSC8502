#pragma once
#include "SceneNodeVariants.h"


void TestPlane::Draw(OGLRenderer& r) {
    if (mesh) {
        r.BindShader(shader);
        Matrix4 model = GetWorldTransform() * Matrix4::Scale(GetModelScale());
        glUniformMatrix4fv(
            glGetUniformLocation(GetShader()->GetProgram(), "modelMatrix"), 1, false, model.values
        );

        glUniform4fv(
            glGetUniformLocation(GetShader()->GetProgram(), "nodeColour"), 1, (float*)&GetColour()
        );

        GLuint planeTexture = GetTexture();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, planeTexture);

        glUniform1i(
            glGetUniformLocation(GetShader()->GetProgram(), "useTexture"), planeTexture
        );
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

    render.modelMatrix.ToIdentity(); // New!
    render.textureMatrix.ToIdentity(); // New!

    render.UpdateShaderMatrices();

    heightMap->Draw();
}

void WaterNode::Draw(OGLRenderer& r) {
    r.BindShader(shader);

    glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

    glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "cubeTex"), 2);

    float col[4] = { 1.0f, 0.5f,0.3f,.5f};
    glUniform4fv(glGetUniformLocation(shader->GetProgram(), "waterColour"), 1, col);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waterTex);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

    r.modelMatrix =
        Matrix4::Translation(hSize * 0.5f) *
        Matrix4::Scale(hSize * 0.5f) *
        Matrix4::Rotation(90, Vector3(1, 0, 0));

    r.textureMatrix =
        Matrix4::Translation(Vector3(0.0f, 0.0f, 0.0f)) *
        Matrix4::Scale(Vector3(10, 10, 10)) *
        Matrix4::Rotation(0.0f, Vector3(0, 0, 1));

    r.UpdateShaderMatrices();
    // SetShaderLight(*light); // No lighting in this shader!
    mesh->Draw();
}