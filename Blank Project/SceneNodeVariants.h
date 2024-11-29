#pragma once
#include "../nclgl/SceneNode.h"
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Heightmap.h"
#include "../nclgl/Camera.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"

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

    void SetEarthTex(GLuint tex) { earthTex = tex; if (!earthTex) { return; } }
    GLuint GetEarthTex() { return earthTex; }
    void SetEarthBump(GLuint tex) { earthBump = tex; if (!earthBump) { return; } }
    GLuint GetEarthBump() { return earthBump; }

    void SetGrassTex(GLuint tex) { grassTex = tex; if (!grassTex) { return; } }
    GLuint GetGrassTex() { return grassTex; }
    void SetGrassBump(GLuint tex) { grassBump = tex; if (!grassBump) { return; } }
    GLuint GetGrassBump() { return grassBump; }

    void SetMainLight(Light* l) { light = l; }
    void SetCamera(Camera* c) { camera = c; }

protected:
    GLuint earthTex;
    GLuint earthBump;
    GLuint grassTex;
    GLuint grassBump;
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
    void SetBumpTex(GLuint w) { bumpTex = w; }

    void SetCubeMap(GLuint c) { cubeMap = c; }
    void SetHSize(Vector3 h) { hSize = h; }

protected:
    Camera* camera;
    GLuint waterTex;
    GLuint bumpTex;
    GLuint cubeMap;
    Vector3 hSize;
};


class RobotNode : public SceneNode {
public:
    RobotNode(Mesh* m = NULL, Vector4 col = Vector4(1.0f, 0.0f, 0.0f, .5f)) : SceneNode(m, col) {
        drawable = true;
        colour = col;

      

    }

    void Draw(OGLRenderer& r) override;



    int currentFrame = 0;
    float frameTime = 0.0f;
    MeshAnimation* meshAnim;
    MeshMaterial* meshMat;
    vector<GLuint> matTextures;


protected:

};