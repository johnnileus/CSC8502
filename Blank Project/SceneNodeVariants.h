#pragma once
#include "../nclgl/SceneNode.h"
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Heightmap.h"
#include "../nclgl/Camera.h"


class TestPlane : public SceneNode {
public:
    void Draw(OGLRenderer& r) override;
};






class HeightMapNode : public SceneNode {
public:
    HeightMapNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1)) : SceneNode(m, colour) {
        drawable = true;
    }
    void Draw(OGLRenderer& r) override;

    HeightMap* heightMap;

    void SetEarthTex(GLuint tex) { earthTex = tex; if (!earthTex) {return;}}
    GLuint GetEarthTex() { return earthTex; }
    void SetEarthBump(GLuint tex) { earthBump = tex; if (!earthBump) { return; }}
    GLuint GetEarthBump() { return earthBump; }
    void SetMainLight(Light* l) { light = l; }
    void SetCamera(Camera* c) { camera = c; }

protected:
    GLuint earthTex;
    GLuint earthBump;
    Light* light;
    Camera* camera;
};
