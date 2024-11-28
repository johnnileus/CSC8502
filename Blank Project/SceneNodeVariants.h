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


class WaterNode : public SceneNode {
public:
    WaterNode(Mesh* m = NULL, Vector4 col = Vector4(1.0f, 0.0f, 0.0f, .5f)) : SceneNode(m, col) {
        drawable = true;
        colour = col;
    }

    void Draw(OGLRenderer& r) override;
    void SetCamera(Camera* c) { camera = c; }
    void SetWaterTex(GLuint w) { waterTex = w; }
    void SetCubeMap(GLuint c) { cubeMap = c; }
    void SetHSize(Vector3 h) { hSize = h; }

protected:
    Camera* camera;
    GLuint waterTex;
    GLuint cubeMap;
    Vector3 hSize;
};