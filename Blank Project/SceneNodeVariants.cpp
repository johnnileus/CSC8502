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
    std::cout << "AAAAA" << std::endl;
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