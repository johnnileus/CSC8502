#include "SceneNode.h"

SceneNode::SceneNode(Mesh* mesh, Vector4 colour) {
    this->mesh = mesh;
    this->colour = colour;
    parent = NULL;
    modelScale = Vector3(1, 1, 1);

    if (mesh) drawable = true;


    boundingRadius = 1.0f;
    distanceFromCamera = 0.0f;
    texture = 0;
}


SceneNode::~SceneNode(void) {
    for (unsigned int i = 0; i < children.size(); ++i) {
        delete children[i];
    }
}

Shader* SceneNode::GetShader() {
    if (shader) { return shader; }
    else { 
        std::cout << "returned default shader" << std::endl;
        return defaultShader;
    }
}

void SceneNode::AddChild(SceneNode* s) {
    children.push_back(s);
    s->parent = this;
}

void SceneNode::Draw(OGLRenderer& r) {
    if (drawable) {
        mesh->Draw();
    }
}

void SceneNode::Update(float dt) {
    if (parent) { // This node has a parent
        worldTransform = parent->worldTransform * transform;
    }
    else { // Root node, world transform is local transform
        worldTransform = transform;
    }

    for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
        (*i)->Update(dt);
    }
}
