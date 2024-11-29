#include "HeightMap.h"
#include <iostream>
#define STB_PERLIN_IMPLEMENTATION

#include "stb_perlin.h"

float getDistToOrigin(float x, float y) {
    return std::sqrt(x * x + y * y);
}

HeightMap::HeightMap(bool procedural, const std::string& name) {
    if (procedural) {

    }
    else {

    }

    int iWidth, iHeight, iChans;
    unsigned char* data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);

    if (!data) {
        std::cout << "Heightmap can't load file!\n";
        return;
    }

    numVertices = iWidth * iHeight;
    numIndices = (iWidth - 1) * (iHeight - 1) * 6;
    vertices = new Vector3[numVertices];
    textureCoords = new Vector2[numVertices];
    indices = new GLuint[numIndices];
    Vector2 textureScale = Vector2(1 / 16.0f, 1 / 16.0f);

    Vector3 vertexScale;
    if (procedural) {
        int mWidth;
        int mHeight;
        int mChans;
        unsigned char* maskData = SOIL_load_image("../Textures/noise2.png", &mWidth, &mHeight, &mChans, 1);
        vertexScale = Vector3(60.0f, 16.0f, 60.0f);
        for (int z = 0; z < iHeight; ++z) {
            for (int x = 0; x < iWidth; ++x) {
                int offset = (z * iWidth) + x;
                float height = (stb_perlin_noise3(x * 0.11f, 0.01f, z * .11f, 0, 0, 0)) * 500 * maskData[offset]/255; 
                vertices[offset] = Vector3(x, height, z) * vertexScale;
                textureCoords[offset] = Vector2(x, z) * textureScale;
                
            }
        }
    }
    else {
        vertexScale = Vector3(16.0f, 5.0f, 16.0f);
        for (int z = 0; z < iHeight; ++z) {
            for (int x = 0; x < iWidth; ++x) {
                int offset = (z * iWidth) + x;
                vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
                textureCoords[offset] = Vector2(x, z) * textureScale;
            }
        }
    }

    SOIL_free_image_data(data);


    int i = 0;

    for (int z = 0; z < iHeight - 1; ++z) {
        for (int x = 0; x < iWidth - 1; ++x) {
            int a = (z * iWidth) + x;
            int b = (z * iWidth) + (x + 1);
            int c = ((z + 1) * iWidth) + (x + 1);
            int d = ((z + 1) * iWidth) + x;

            indices[i++] = a;
            indices[i++] = c;
            indices[i++] = b;

            indices[i++] = c;
            indices[i++] = a;
            indices[i++] = d;
        }
    }

    GenerateNormals();
    GenerateTangents();
    BufferData();

    heightmapSize.x = vertexScale.x * (iWidth - 1);
    heightmapSize.y = vertexScale.y * 255.0f; // each height is a byte!
    heightmapSize.z = vertexScale.z * (iHeight - 1);

}