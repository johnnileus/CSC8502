#pragma once
#include <string>
#include "Mesh.h"

class HeightMap : public Mesh {
public:
	HeightMap(bool procedural, const std::string& name = NULL);
	~HeightMap(void) {};

	Vector3 GetHeightmapSize() const {return heightmapSize;}
protected:
	Vector3 heightmapSize;
};
